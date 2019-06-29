#include "NetServer.h"
#include <stdio.h>

#include <string.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>




NetServer::Item::Item(unsigned int tag, unsigned long id, const unsigned char* data, unsigned int sz): mTag(tag), mID(id) {
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

NetServer::Item::~Item() {
}

unsigned int NetServer::Item::getTag() {
    return mTag;
}

std::vector<unsigned char>& NetServer::Item::getData() {
    return mData;
}

unsigned long NetServer::Item::getID() {
    return mID;
}

//////////////////////////////////

NetServer::NetServer() : mSckId(-1), mPort(0), mStatus(0){
}

NetServer::~NetServer() {
}

int NetServer::init(std::shared_ptr<INetCallBack> callback, int port){
    int status = getStatus();
    if (status != 0)
       return status;
    setStatus(1);
    mPort = port;
    int sockId = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bzero(&(server_addr.sin_zero),8);
    socklen_t sock_len = sizeof(struct sockaddr_in);
    if(bind(sockId,(struct sockaddr *)&server_addr,sock_len) < 0) {
        printf("bind failure\n");
        return -2;
    }
    printf("bind success\n");
    if(listen(sockId,10) < 0) {
        printf("listen failure\n");
        return -3;
    }
    printf("Listening..\n");

    mSckId = sockId;
    mCallBack = callback;
    setStatus(2);
    mThreads.push_back(std::shared_ptr<std::thread>(new std::thread(&NetServer::threadRecv, this)));
    mThreads.push_back(std::shared_ptr<std::thread>(new std::thread(&NetServer::threadSend, this)));
    return 0;
}

int NetServer::send(unsigned int tag, const unsigned char *data, unsigned int sz) {
    int status = getStatus();
    if (status != 2)
       return -1;
    {    
        std::lock_guard<std::mutex> locker(mLock);
        if (mConnects.empty())
            return -2;
    }

    std::shared_ptr<Item> item(new Item(tag, 0, data, sz));    
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

int NetServer::exit() {
    setStatus(3);
    std::lock_guard<std::mutex> locker(mLock);
    mWait.notify_one();
    return 0;
}

std::shared_ptr<NetServer>& NetServer::singleton(){
    static std::mutex sMutex;
    static std::shared_ptr<NetServer> singleton = NULL;
    if (singleton != NULL)
        return singleton;
    std::lock_guard<std::mutex> locker(sMutex);
    if (singleton != NULL)
        return singleton;
    singleton = std::shared_ptr<NetServer>(new NetServer());
    return singleton;
}

std::shared_ptr<INetServer> INetServer::singleton(){
   return NetServer::singleton();
}


int NetServer::threadSend() {

    while(getStatus() == 2) {
        printf("threadLoop \n");
        std::list<std::shared_ptr<NetServer::Item>> datas;
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
        std::list<std::shared_ptr<NetServer::Item>>::iterator itr;

        for (itr = datas.begin(); itr != datas.end(); ++itr) {
            
            std::lock_guard<std::mutex> lock(mLock);
            for (it = mConnects.begin(); it != mConnects.end();) {
                int sockId = (*it)->getSocketID();
                if (sockId < 0) {
                    (*it)->close();
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

int NetServer::threadRecv() {
    fd_set rfds;
    struct timeval timeout={1,0};
    struct sockaddr_in remote_addr;
    socklen_t sock_len = sizeof(struct sockaddr_in);
    while(getStatus() == 2) {
        FD_ZERO(&rfds); //每次循环都要清空集合，否则不能检测描述符变化
        printf("threadLoop\n");
        std::list<std::shared_ptr<Connect>>::iterator it;
        int maxFd = -1;
        maxFd = (mSckId > maxFd ? mSckId : maxFd);
        FD_SET(mSckId, &rfds); //添加描述符

        {
            std::lock_guard<std::mutex> locker(mLock);
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

        if (FD_ISSET(mSckId, &rfds)) {
            int asock = accept(mSckId,(struct sockaddr *)&remote_addr, &sock_len);
            std::shared_ptr<Connect> connect(new Connect(inet_ntoa(remote_addr.sin_addr), asock));
            std::lock_guard<std::mutex> locker(mLock);
            mConnects.push_back(connect);
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

int NetServer::getStatus() {
    std::lock_guard<std::mutex> locker(mLock);
    return mStatus;
}

int NetServer::setStatus(int status) {
    std::lock_guard<std::mutex> locker(mLock);
    mStatus = status;
    return mStatus;
}