#include "INetClient.h"
#include "ProtoBuffer.h"
#include <string.h>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int test_ProtoBuffer_read(ProtoBuffer *probuf) {
    printf("test_ProtoBuffer_read\n");
   printf("int8_t %x \n", probuf->readInt8(probuf));

    printf("uint8_t %x \n", probuf->readUInt8(probuf));

    printf("int16_t %x \n", probuf->readInt16(probuf));

    printf("uint16_t %x \n", probuf->readUInt16(probuf));

    printf("int32_t %x \n", probuf->readInt32(probuf));

    printf("uint32_t %x \n", probuf->readUInt32(probuf));

    printf("int64_t %lx \n", probuf->readInt64(probuf));

    printf("uint64_t %0llx \n", probuf->readUInt64(probuf));

    printf("float %f \n", probuf->readFloat(probuf));

    printf("double %f \n", probuf->readDouble(probuf));

    printf("c string %s \n", probuf->readCString(probuf));

       uint8_t vdata[16] = {0};
    int sz = probuf->read(probuf, vdata, 16);
    for (int i = 0 ; i < 16; ++i) 
      printf(" %x ", vdata[i]);
    printf("\n");
    return 0;
}


class ECallBack : public INetCallBack{
    private:
    struct  ProtoBuffer *probuf;
    
    public:
    ECallBack() {
        probuf = allocateProtoBuffer();
    }
    virtual ~ECallBack() {
        probuf->free(probuf);
    }

    virtual int recv(unsigned int tag, unsigned char *data, int size) {
        printf("recv data: tag %d\n", tag);
        for (int i = 0 ; i < size ; ++i) {
            if ((i % 16) == 0)
               printf("\n");
            printf(" %02X ", data[i]);
        }
        
        printf("\n\n");
         
        probuf->write(probuf, data, size);

        test_ProtoBuffer_read(probuf);

        return 0;
    }
};


int main(int argc, char *argv[]) {

    const char * address = argv[1] ;

    std::shared_ptr<INetClient> netClient = INetClient::singleton();
        std::shared_ptr<INetCallBack> callback(new ECallBack());
    netClient->init(callback);

    netClient->add(address, 8008);
    struct ProtoBuffer *probuf = allocateProtoBuffer();
    probuf->writeInt8(probuf, 0x45);
    probuf->writeUInt8(probuf, 0xF2);
    probuf->writeInt16(probuf, 0x3343);
    probuf->writeUInt16(probuf, 0xFFFA);
    probuf->writeInt32(probuf, 0x69121928);
    probuf->writeUInt32(probuf, 0xFf671721);

    int64_t i64 = 0x77700000213L;
    probuf->writeInt64(probuf, i64);
    uint64_t u64 = 0xFF33232323888UL;
    probuf->writeUInt64(probuf, u64);

    probuf->writeFloat(probuf, 434034.012);
    probuf->writeDouble(probuf, 8945239.6565);

    probuf->writeCString(probuf, "windows callbacks32");

    uint8_t vdata[16] = {0xee, 0xcc, 0xaa, 0x44, 0x66, 0x99, 0x22};

    probuf->write(probuf, vdata, 16);

    while(1) {

        netClient->send(0x100, (unsigned char*)probuf->data(probuf), probuf->dataSize(probuf));
       // sleep(1);
    }

    return 0;
}