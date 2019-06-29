#include "Connect.h"
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


Connect::Connect(const char *address,int sckid) : mAddress(address), mSckID(sckid) {
}

Connect::Connect() : mAddress(""), mSckID(-1)  {
}

Connect::~Connect() {
}

int Connect::connect(const char *address, int port, int domain, int type, int protocol){
    int sckid = socket(domain, type, protocol);
    struct  sockaddr_in  servAddr;
    servAddr.sin_family = domain;
    servAddr.sin_port  = htons(port);
    servAddr.sin_addr.s_addr = inet_addr(address);
    int code = ::connect(sckid, (struct sockaddr*)&servAddr, sizeof(servAddr));
    if (code < 0)
    {
        ::close(sckid);
        sckid = -1 ;
        return code ;
    }
    
    std::lock_guard<std::mutex> lock(mLock);
    mSckID = sckid;
    mAddress = address;
    return 0;
}

int Connect::write(int sckid, const unsigned  char *data , int size) {
    if (sckid < 0)
        return -1;
    int length = size, sz;
    while(length > 0) {
        sz = ::send(sckid, data, length, MSG_NOSIGNAL|MSG_WAITALL);
	    if (sz <= 0) {
            if (errno == EINTR)
                sz = 0 ;
            else if (errno == EAGAIN)
                sz = 0 ;
            else
				return -2 ;
		}
        length -= sz ;
        data += sz ;
    }
    return (size - length);
}

int Connect::send(const std::vector<unsigned char> &data) {
    std::lock_guard<std::mutex> lock(mLock);
    return write(mSckID, &data[0], data.size());
}

int Connect::read(int sckid, unsigned  char *data , int size){
    int length = size, sz;
    if (sckid < 0)
         return -1 ;

    while(length > 0) {
        sz = ::recv(sckid, data, length, MSG_NOSIGNAL|MSG_WAITALL) ;
        if (sz <= 0) {
            if (errno == EINTR)
                sz = 0 ;
            else if (errno == EAGAIN)
                sz = 0 ;
            else
                return -2 ;
        }
        length -= sz ;
        data += sz ;
    }
    return (size - length) ;
}

int Connect::recv(std::vector<unsigned char> &data){
    data.assign(4, 0); 

    std::lock_guard<std::mutex> lock(mLock);
    if (read(mSckID, &data[0], 4) != 4) {
        return -1;
    }

    int sz = data[0];
    sz = (sz << 8) + data[1];
    sz = (sz << 8) + data[2];
    sz = (sz << 8) + data[3];
    data.assign(sz, 0);
    if (read(mSckID, &data[0], sz) != sz) {
        return -2;
    }

    return sz;
}

int Connect::select(unsigned long ts, int type){
    int sckid = -1; 
    {
        std::lock_guard<std::mutex> lock(mLock);
        sckid = mSckID ;
    }

    fd_set fd;
	struct timeval tm;
	int code ;
	if (sckid < 0)
        return -1 ;
	tm.tv_sec = ts / 1000 ;
	tm.tv_usec = (ts % 1000) * 1000 ;
   	FD_ZERO(&fd);
	FD_SET(sckid, &fd);
    switch(type) {
        case 0: //read
           code = ::select(sckid + 1, &fd, NULL, NULL, &tm) ;
           break;
        case 1: //write
           code = ::select(sckid + 1, NULL, &fd, NULL, &tm) ;
           break;
        case 2: //except
           code = ::select(sckid + 1, NULL, NULL, &fd, &tm) ;
           break;
        default:
           return -4;
    }

    if (code > 0)
        return 0;
    else if (code == 0)
        return -2;
    return -3;
}

int Connect::close(){
    std::lock_guard<std::mutex> lock(mLock);
    if (mSckID < 0) {
        return 1;
    }

    ::close(mSckID);
    mSckID = -1;
    return 0;
}

int Connect::disconnect() {
    std::lock_guard<std::mutex> lock(mLock);
    if (mSckID < 0) {
        return -1;
    }

    unsigned char vData[12] = {0,0,0,8,0,0,0,0,0,0,0,0};
    write(mSckID, vData, 12);
    ::close(mSckID);
    mSckID = -1;
    return 0;
}

const char* Connect::getAddress(){
    return mAddress.c_str();
}

int  Connect::getSocketID(){
    std::lock_guard<std::mutex> lock(mLock);
    return mSckID;
}