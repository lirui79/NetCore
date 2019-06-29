#include "NetClient.h"
#include <string.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>



NetClient::Item::Item(unsigned int tag, unsigned long id, const unsigned char* data, unsigned int sz): mTag(tag), mID(id) {
    unsigned int size = sz + 4;
    mData.assign(sz + 8, 0x00);
    //Length  4bytes
    mData[0] = (unsigned char) (size >> 24);
    mData[1] = (unsigned char) (size >> 16);
    mData[2] = (unsigned char) (size >> 8);
    mData[3] = (unsigned char) (size);
    // Tag    4bytes
    mData[4] = (unsigned char) (tag >> 24);
    mData[5] = (unsigned char) (tag >> 16);
    mData[6] = (unsigned char) (tag >> 8);
    mData[7] = (unsigned char) (tag);
    // Data   (Length - 4) bytes
    memcpy(&mData[8], data, sz);
}

NetClient::Item::~Item() {
}

unsigned int NetClient::Item::getTag() {
    return mTag;
}

std::vector<unsigned char>& NetClient::Item::getData() {
    return mData;
}

unsigned long NetClient::Item::getID() {
    return mID;
}

//////////////////////////////////
NetClient::NetClient() : mStatus(0) {
}

NetClient::~NetClient() {
}

int NetClient::init(std::shared_ptr<INetCallBack> callback) {
    int status = getStatus();
    if (status != 0)
       return status;
    setStatus(2);
    mCallBack = callback;
    mThreads.push_back(std::shared_ptr<std::thread>(new std::thread(&NetClient::threadSend, this)));
    mThreads.push_back(std::shared_ptr<std::thread>(new std::thread(&NetClient::threadRecv, this)));
    return 0;
}

int NetClient::add(const char *address, int port) {
    int status = getStatus();
    if (status != 2)
       return -1;
    std::shared_ptr<Connect> connect(new Connect());
    int code = connect->connect(address, port, AF_INET, SOCK_STREAM, 0);
    if (code < 0) {
        printf("add server %s port %d\n", address, port);
        return code;
    }

    std::lock_guard<std::mutex> locker(mLock);
    mConnects.push_back(connect);
    mWait.notify_one();
    return 0;
}

int NetClient::del(const char *address) {
    int status = getStatus();
    if (status != 2)
       return -1;

    std::string strAddress(address);
    int nFind = 0;
    std::lock_guard<std::mutex> locker(mLock);
    std::list<std::shared_ptr<Connect>>::iterator it;
    for (it = mConnects.begin(); it != mConnects.end(); ) {
        if (strAddress == (*it)->getAddress()) {
            if ((*it)->getSocketID() >= 0)
                (*it)->disconnect();
            (*it)->close();
            it = mConnects.erase(it);
            nFind = 1;
            continue;
        }

        int sockId = (*it)->getSocketID();
        if (sockId < 0) {
            it = mConnects.erase(it);
            continue;
        }
        ++it;
    }

    mWait.notify_one();
    return nFind;
}

int NetClient::exit() {
    setStatus(3);
    std::lock_guard<std::mutex> locker(mLock);
    mWait.notify_one();
    return 0;
}

int NetClient::send(unsigned int tag, const unsigned char *data, unsigned int sz) {
    int status = getStatus();
    if (status != 2)
       return -1;
    {    
        std::lock_guard<std::mutex> locker(mLock);
        if (mConnects.empty())
            return -2;
    }

    std::shared_ptr<NetClient::Item> item(new NetClient::Item(tag, 0, data, sz));    
    {// data
        std::lock_guard<std::mutex> lock(mDLock);
        mDatas.push_back(item);
    }

    {// connect
        std::lock_guard<std::mutex> locker(mLock);
        mWait.notify_one();
    }

    return 0;
}

std::shared_ptr<NetClient>& NetClient::singleton() {
    static std::mutex sMutex;
    static std::shared_ptr<NetClient> singleton = NULL;
    if (singleton != NULL)
        return singleton;
    std::lock_guard<std::mutex> locker(sMutex);
    if (singleton != NULL)
        return singleton;
    singleton = std::shared_ptr<NetClient>(new NetClient());
    return singleton;
}

std::shared_ptr<INetClient> INetClient::singleton() {
    return NetClient::singleton();
}

int NetClient::threadRecv() {
    fd_set rfds;
    struct timeval timeout={1,0};
    struct sockaddr_in remote_addr;
    socklen_t sock_len = sizeof(struct sockaddr_in);
    while(getStatus() == 2) {
        FD_ZERO(&rfds); //每次循环都要清空集合，否则不能检测描述符变化
        printf("threadLoop\n");
        std::list<std::shared_ptr<Connect>>::iterator it;
        int maxFd = -1;

        {
            std::unique_lock<std::mutex> locker(mLock);
            if (mConnects.empty()) {
                mWait.wait(locker);
                continue;
            }

            for (it = mConnects.begin(); it != mConnects.end(); ) {
               int sockId = (*it)->getSocketID();
               if (sockId < 0) {
                   it = mConnects.erase(it);
                   break;
               }
               maxFd = (sockId > maxFd ? sockId : maxFd);
               FD_SET(sockId, &rfds);
               ++it;
            }

        }

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        int code = select(maxFd + 1, &rfds, NULL, NULL, &timeout);
        if (code <= 0) {
            printf("NetServer select\n");
            continue;
        }

        std::list<std::shared_ptr<Connect>> connects;
        {
            std::lock_guard<std::mutex> locker(mLock);
            connects = mConnects;
        }

        std::vector<unsigned char> data;
        for (it = connects.begin(); it != connects.end(); ++it) {
            int sockId = (*it)->getSocketID();
            if (!FD_ISSET(sockId, &rfds)) {
                continue;
            }
            
            code = (*it)->recv(data);// dispatch data 
            if (code < 0) {
                printf("recv code %d\n", code);
                (*it)->disconnect();
                continue;
            }

            unsigned int tag = data[0];
            tag = (tag << 8) + data[1];
            tag = (tag << 8) + data[2];
            tag = (tag << 8) + data[3];

            if (tag < 0x100) {
                switch(tag) {
                    case 0x00:
                    (*it)->close();
                    break;
                }
                printf("interal data tag%d\n", tag);
                continue;
            }

            mCallBack->recv(tag, &data[4], data.size() - 4);
        }

    }

    printf("exit\n");
    setStatus(0);
    return 0;
}

int NetClient::threadSend() {

    while(getStatus() == 2) {
        printf("threadLoop \n");
        std::list<std::shared_ptr<NetClient::Item>> datas;
        {
          std::lock_guard<std::mutex> lock(mDLock);
          datas = mDatas;
          mDatas.clear();
        }

        if (datas.empty()) {
            std::unique_lock<std::mutex> lock(mLock);
            mWait.wait(lock);
            continue;
        }

        {
            std::unique_lock<std::mutex> lock(mLock);
            if (mConnects.empty()) {
                mWait.wait(lock);
                continue;                
            }
        }

        std::list<std::shared_ptr<Connect>>::iterator it;
        std::list<std::shared_ptr<NetClient::Item>>::iterator itr;

        for (itr = datas.begin(); itr != datas.end(); ++itr) {
            
            std::lock_guard<std::mutex> lock(mLock);
            for (it = mConnects.begin(); it != mConnects.end();) {
                int sockId = (*it)->getSocketID();
                if (sockId < 0) {
                    it = mConnects.erase(it);
                    continue;
                }

                int code = (*it)->send((*itr)->getData());//
                if (code < 0) {
                    (*it)->disconnect();
                    it = mConnects.erase(it);
                    continue;
                }
                ++it;
            }
        }

    }

    printf("exit\n");
    setStatus(0);
    return 0;
}

int NetClient::getStatus() {
    std::lock_guard<std::mutex> locker(mLock);
    return mStatus;
}

int NetClient::setStatus(int status) {
    std::lock_guard<std::mutex> locker(mLock);
    mStatus = status;
    return mStatus;
}