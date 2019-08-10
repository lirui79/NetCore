/*****************************************************/
/*
**   Author: lirui
**   Date: 2019/06/27
**   File: INetServer.h
**   Function:  Interface of INetServer for user
**   History:
**       2019/06/27 create by lirui
**
**   Copy Right: iris corp
**
*/
/*****************************************************/
#ifndef VIRTUAL_INTERFACE_NET_SERVER_H
#define VIRTUAL_INTERFACE_NET_SERVER_H

#include <memory>


class INetCallBack {
public:
    virtual ~INetCallBack() {}
    virtual int recv(unsigned int tag, const unsigned char* data, int size) = 0;
};

class INetServer {
private:
    INetServer(const INetServer &module);

    INetServer& operator=(const INetServer &module);
protected:
    INetServer(){}

public:
    virtual ~INetServer(){}

    virtual int init(std::shared_ptr<INetCallBack> callback, int port) = 0;

    virtual int exit() = 0;

    virtual int send(unsigned int tag, const unsigned char *data, unsigned int sz) = 0;

    static std::shared_ptr<INetServer> singleton();

};



#endif  // VIRTUAL_INTERFACE_NET_SERVER_H