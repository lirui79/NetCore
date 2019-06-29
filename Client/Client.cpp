#include "INetClient.h"
#include <string.h>
#include <unistd.h>
#include <vector>

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

    const char * address = argv[1] ;

    unsigned char buffer[256] = {10, 20, 30 , 40, 50, 60, 70, 80};
    std::shared_ptr<INetClient> netClient = INetClient::singleton();
        std::shared_ptr<INetCallBack> callback(new ECallBack());
    netClient->init(callback);

    netClient->add(address, 8008);
        printf("send data: \n");
        for(int i = 0 ; i < 20 ; ++i) {
            if ((i % 16) == 0)
               printf("\n");
            printf(" %02X ", buffer[i]);
        }

        printf("\n\n");

    int i = 0;
    while(1) {
        ++i;
        if (i > 256)
           i %= 256;
        if (i <= 0)
           i = 10;
        netClient->send(0x100, buffer, i);
       // sleep(1);
    }

    return 0;
}