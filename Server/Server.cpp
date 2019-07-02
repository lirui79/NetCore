#include "INetServer.h"
#include "ProtoBuffer.h"
#include <unistd.h>
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

    printf("int64_t %0lx \n", probuf->readInt64(probuf));

    printf("uint64_t %0llx \n", probuf->readUInt64(probuf));

    printf("float %f \n", probuf->readFloat(probuf));

    printf("double %lf \n", probuf->readDouble(probuf));

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

    std::shared_ptr<INetServer> netServer = INetServer::singleton();
    std::shared_ptr<INetCallBack> callback(new ECallBack());
    struct ProtoBuffer *probuf = allocateProtoBuffer();
    probuf->writeInt8(probuf, 0x75);
    probuf->writeUInt8(probuf, 0xF0);
    probuf->writeInt16(probuf, 0x4375);
    probuf->writeUInt16(probuf, 0xFA7f);
    probuf->writeInt32(probuf, 0x7500057);
    probuf->writeUInt32(probuf, 0xF000067);

    int64_t i64 = 0x4534213L;
    probuf->writeInt64(probuf, i64);
    uint64_t u64 = 0xfffadafUL;
    probuf->writeUInt64(probuf, u64);

    probuf->writeFloat(probuf, 1201.215);
    probuf->writeDouble(probuf, 432424235.24032);

    probuf->writeCString(probuf, "windows event1ad");

    uint8_t vdata[16] = {0xa, 0xad, 0xff, 0x11, 0x22, 0x33, 0x66};

    probuf->write(probuf, vdata, 16);

    
    netServer->init(callback, 8008);

    while(1) {
        INetServer::singleton()->send(0x200, (unsigned char*)probuf->data(probuf), probuf->dataSize(probuf));
 //       sleep(1);
    }

    return 0;
}