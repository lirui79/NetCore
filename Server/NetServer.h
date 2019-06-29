/*****************************************************/
/*
**   Author: lirui
**   Date: 2019/06/27
**   File: NetServer.h
**   Function:  Interface of NetServer for user
**   History:
**       2019/06/27 create by lirui
**
**   Copy Right: iris corp
**
*/
/*****************************************************/
#ifndef INTERFACE_NET_SERVER_H
#define INTERFACE_NET_SERVER_H

#include "INetServer.h"
#include "Connect.h"
#include <list>
#include <memory>
#include <sys/prctl.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <list>




class NetServer: public INetServer {
private:
    class Item {
    public:
        Item(unsigned int tag, unsigned long id, const unsigned char* data, unsigned int sz);

        ~Item();

        unsigned int getTag();

        std::vector<unsigned char>& getData();

        unsigned long getID();
    private:
        unsigned int  mTag;
        unsigned long mID ;
        std::vector<unsigned char> mData;
    };
private:
    NetServer(const NetServer &module);

    NetServer& operator=(const NetServer &module);
protected:
    NetServer();

public:
    virtual ~NetServer();

    virtual int init(std::shared_ptr<INetCallBack> callback, int port);

    virtual int exit();

    virtual int send(unsigned int tag, const unsigned char *data, unsigned int sz);

    static std::shared_ptr<NetServer>& singleton();
protected:
   virtual int threadSend();
   virtual int threadRecv();
   virtual int         getStatus();
   virtual int         setStatus(int status);

private:
    int    mSckId;
    int    mPort;
    std::shared_ptr<INetCallBack>    mCallBack;
    std::list<std::shared_ptr<std::thread>>  mThreads;

    std::list<std::shared_ptr<Connect>> mConnects;
    std::mutex              mLock;
    std::condition_variable mWait;

    std::list<std::shared_ptr<NetServer::Item>> mDatas;
    std::mutex              mDLock;

    int                     mStatus;  // 0-exit  1-initing  2-running 3-exiting
};



#endif  // INTERFACE_NET_SERVER_H