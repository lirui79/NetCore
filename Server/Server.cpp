#include "INetServer.h"
#include <unistd.h>

class ECallBack : public INetCallBack{
    public:
    virtual ~ECallBack() {}

    virtual int recv(unsigned int tag, unsigned char *data, int size) {
        printf("recv data: tag %d\n", tag);
        for (int i = 0 ; i < size ; ++i) {
            if ((i % 16) == 0)
               printf("\n");
            printf(" %02X ", data[i]);
        }
        
        printf("\n\n");
        return 0;
    }
};



int main(int argc, char *argv[]) {

    unsigned char buffer[20] = {1, 2, 3 , 4, 5};
    std::shared_ptr<INetServer> netServer = INetServer::singleton();
    std::shared_ptr<INetCallBack> callback(new ECallBack());
    
    netServer->init(callback, 8008);

    while(1) {
        INetServer::singleton()->send(0x200, buffer, 20);
        sleep(1);
    }

    return 0;
}