/*****************************************************/
/*
**   Author: lirui
**   Date: 2019/06/27
**   File: INetClient.h
**   Function:  Interface of INetClient for user
**   History:
**       2019/06/27 create by lirui
**
**   Copy Right: iris corp
**
*/
/*****************************************************/
#ifndef VIRTUAL_INTERFACE_NET_CLIENT_H
#define VIRTUAL_INTERFACE_NET_CLIENT_H

#include <memory>

class INetCallBack {
public:
    virtual ~INetCallBack() {}
    virtual int recv(unsigned int tag, unsigned char* data, int size) = 0;
};


class INetClient {
private:
    INetClient(const INetClient &module);

    INetClient& operator=(const INetClient &module);
protected:
    INetClient(){}


public:
    virtual ~INetClient(){}

    virtual int init(std::shared_ptr<INetCallBack> callback) = 0;

    virtual int add(const char *address, int port) = 0;

    virtual int del(const char *address) = 0;

    virtual int exit() = 0;

    virtual int send(unsigned int tag, const unsigned char *data, unsigned int sz) = 0;

    static std::shared_ptr<INetClient> singleton();

};



#endif  // VIRTUAL_INTERFACE_NET_CLIENT_H