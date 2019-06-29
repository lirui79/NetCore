/*****************************************************/
/*
**   Author: lirui
**   Date: 2019/06/27
**   File: NetClient.h
**   Function:  Interface of NetClient for user
**   History:
**       2019/06/27 create by lirui
**
**   Copy Right: iris corp
**
*/
/*****************************************************/
#ifndef INTERFACE_NET_CLIENT_H
#define INTERFACE_NET_CLIENT_H

#include "INetClient.h"
#include "Connect.h"
#include <list>
#include <memory>
#include <sys/prctl.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <list>

class NetClient : public INetClient {
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
    NetClient(const NetClient &module);

    NetClient& operator=(const NetClient &module);
protected:
    NetClient();

public:
    virtual ~NetClient();

    virtual int init(std::shared_ptr<INetCallBack> callback);

    virtual int add(const char *address, int port);

    virtual int del(const char *address);

    virtual int exit();

    virtual int send(unsigned int tag, const unsigned char *data, unsigned int sz);

    static std::shared_ptr<NetClient>& singleton();
protected:
   virtual int threadSend();
   virtual int threadRecv();
   virtual int         getStatus();
   virtual int         setStatus(int status);

private:
   std::list<std::shared_ptr<Connect>> mConnects;
   std::mutex              mLock;
   std::condition_variable mWait;

   std::list<std::shared_ptr<NetClient::Item>> mDatas;
   std::mutex              mDLock;

   std::list<std::shared_ptr<std::thread>>  mThreads;
   std::shared_ptr<INetCallBack>        mCallBack;
   int                     mStatus;  // 0-exit  1-initing  2-running 3-exiting
};



#endif  // INTERFACE_NET_CLIENT_H