/*****************************************************/
/*
**   Author: lirui
**   Date: 2019/06/27
**   File: Connect.h
**   Function:  Interface of Connect for user
**   History:
**       2019/06/27 create by lirui
**
**   Copy Right: iris corp
**
*/
/*****************************************************/
#ifndef INTERFACE_NET_CONNECT_H
#define INTERFACE_NET_CONNECT_H

#include <string>
#include <vector>
#include <mutex>

class Connect {
public:
    Connect(const char *address,int sckid);

    Connect();

    virtual ~Connect();

    virtual int connect(const char *address, int port, int domain, int type, int protocol);

    virtual int send(const std::vector<unsigned char> &data);

    virtual int recv(std::vector<unsigned char> &data);

    virtual int select(unsigned long ts, int type);

    virtual int close();

    virtual const char* getAddress();

    virtual int  getSocketID();

    virtual int  disconnect();
protected:
    int read(int sckid, unsigned  char *data , int size);
    int write(int sckid, const unsigned  char *data , int size);
private:
    std::string     mAddress;
    int             mSckID;
    std::mutex      mLock;
};



#endif  // INTERFACE_NET_CONNECT_H