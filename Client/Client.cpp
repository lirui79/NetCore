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

int window_ProtoBuffer_read(ProtoBuffer *probuf) {
    for (int i = 0 ; i < 2; ++i) {    
        printf("pos:");
        for (int j = 0; j < 3; ++j) {
        printf("%f ", probuf->readFloat(probuf));
        }
        printf("\n");
    }

    for (int i = 0 ; i < 2; ++i) {    
        printf("up:");
        for (int j = 0; j < 3; ++j) {
        printf("%f ", probuf->readFloat(probuf));
        }
        printf("\n");
    }

    for (int i = 0 ; i < 2; ++i) {    
        printf("dir:");
        for (int j = 0; j < 3; ++j) {
        printf("%f ", probuf->readFloat(probuf));
        }
        printf("\n");
    }

    for (int i = 0 ; i < 2; ++i) {    
        printf("project:");
        for (int j = 0; j < 16; ++j) {
        printf("%f ", probuf->readFloat(probuf));
        }
        printf("\n");
    }

/*
    for (int i = 0 ;i < 2; ++i) { 
        printf("pos :");
        for (int j = 0 ; j < 3 ; ++j) 
          printf("%f ", probuf->readFloat(probuf));
        printf("\n");
    
        printf("qua :");
        for (int j = 0 ; j < 4 ; ++j) 
          printf("%f ", probuf->readFloat(probuf));
        printf("\n");
    }*/

    return 0;
}

int color_ProtoBuffer_read(ProtoBuffer *probuf) {
   unsigned int color = probuf->readUInt32(probuf);
   printf("color %d \n", color);
   return 0;
}


int keycode_ProtoBuffer_read(ProtoBuffer *probuf) {
   unsigned int keyCode = probuf->readUInt32(probuf);
   unsigned int keyAction = probuf->readUInt32(probuf);
   printf("keyCode %d keyAction %d\n", keyCode, keyAction);
   return 0;
}

int light_ProtoBuffer_read(ProtoBuffer *probuf) {
   unsigned int light = probuf->readUInt32(probuf);
   printf("light %d \n", light);
   return 0;
}

int more_ProtoBuffer_read(ProtoBuffer *probuf) {
   unsigned int num = probuf->readUInt32(probuf);
   printf("num %d \n", num);
   for (unsigned int i = 0 ; i < num; ++i) {
       unsigned  int len =  probuf->readUInt32(probuf);
        unsigned int tag =  probuf->readUInt32(probuf);
        printf("item len %d  tag %d\n", len, tag);
        switch(tag) {
            case 0x100:
                       window_ProtoBuffer_read(probuf);
                       break;
            case 0x101:
                      color_ProtoBuffer_read(probuf);
                       break;
            case 0x102:
                       keycode_ProtoBuffer_read(probuf);
                       break;
            case 0x103:
                       light_ProtoBuffer_read(probuf);
                       break;
            case 0x110:
                       printf("tag %d can not more two layers\n", tag);
                       break;
            case 0xFFFFFFFF:// test
                      test_ProtoBuffer_read(probuf);
                       break; 
            default:
                       printf("tag %d error\n", tag);
                       break;
        }
   }
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

        //
        switch(tag) {
            case 0x100:
                       window_ProtoBuffer_read(probuf);
                       break;
            case 0x101:
                      color_ProtoBuffer_read(probuf);
                       break;
            case 0x102:
                       keycode_ProtoBuffer_read(probuf);
                       break;
            case 0x103:
                       light_ProtoBuffer_read(probuf);
                       break;
            case 0x110:
                      more_ProtoBuffer_read(probuf);
                       break;
            case 0xFFFFFFFF:// test
                      test_ProtoBuffer_read(probuf);
                       break; 
            default:
                       printf("tag %d error\n", tag);
                       break;
        }

        return 0;
    }
};

void getwindowdata(struct ProtoBuffer *probuf) {

	float pos[2][3] = {0.10, 0.20, 0.30, 0.40, 0.5, 0.6}, up[2][3] = {0.13, 0.24, 0.36, 0.43, 0.51, 0.62}, dir[2][3] = {0.31, 0.42, 0.65, 0.76, 0.89, 0.91};
	float proj[2][16] = {0.1, 0.2, 0.3, 1.0, 
                         0.1, 0.2, 0.3, 1.0, 
                         0.1, 0.2, 0.3, 1.0, 
                         0.1, 0.2, 0.3, 1.0, 
                         0.1, 0.2, 0.3, 1.0, 
                         0.1, 0.2, 0.3, 1.0, 
                         0.1, 0.2, 0.3, 1.0, 
                         0.1, 0.2, 0.3, 1.0};
    //float posa[3] = {1.0, 1.1, 1.2};
    //float qua[4] = {0.1, 0.11, 0.2, 1.0};

    for (int i = 0 ;i < 2; ++i) {
        for (int j = 0 ; j < 3 ; ++j) 
           probuf->writeFloat(probuf, pos[i][j]);
    }

    for (int i = 0 ;i < 2; ++i) {
        for (int j = 0 ; j < 3 ; ++j) 
           probuf->writeFloat(probuf, up[i][j]);
    }

    for (int i = 0 ;i < 2; ++i) {
        for (int j = 0 ; j < 3 ; ++j) 
           probuf->writeFloat(probuf, dir[i][j]);
    }

    for (int i = 0 ;i < 2; ++i) {
        for (int j = 0 ; j < 16 ; ++j) 
           probuf->writeFloat(probuf, proj[i][j]);
    }

/*
    for (int i = 0 ;i < 2; ++i) {
        for (int j = 0 ; j < 3 ; ++j) 
           probuf->writeFloat(probuf, posa[j]);
        for (int j = 0 ; j < 4 ; ++j) 
           probuf->writeFloat(probuf, qua[j]);
    }*/
}

void getTestData(struct ProtoBuffer *probuf) {
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
}


void getcolordata(struct ProtoBuffer *probuf) {
       probuf->writeUInt32(probuf, 2);
}

void getkeydata(struct ProtoBuffer *probuf) {
       probuf->writeUInt32(probuf, 0);
       probuf->writeUInt32(probuf, 1);
}

void getlightdata(struct ProtoBuffer *probuf) {
       probuf->writeUInt32(probuf, 2);
}

void getmoredata(struct ProtoBuffer *probuf) {
        struct ProtoBuffer *buf = allocateProtoBuffer();
        unsigned int num = 5;
        unsigned int tag[5] = {0x100, 0x101, 0x102, 0x103,0xFFFFFFFF};
        probuf->writeUInt32(probuf, num);
        for (unsigned int i = 0 ; i < num; ++i) {
            switch(tag[i]) {
                case 0x100:
                        getwindowdata(buf);
                        break;
                case 0x101:
                        getcolordata(buf);
                        break;
                case 0x102:
                        getkeydata(buf);
                        break;
                case 0x103:
                        getlightdata(buf);
                        break;
                case 0x110:
                        break;
                case 0xFFFFFFFF:// test
                        getTestData(buf);
                        break; 
                default:
                        printf("tag %d error\n", tag);
                        break;
            }
            probuf->writeUInt32(probuf, buf->dataSize(buf) + 4);
            probuf->writeUInt32(probuf, tag[i]);//tag
            probuf->write(probuf, buf->data(buf), buf->dataSize(buf));
            buf->reset(buf);
        }
       
        buf->free(buf);
}


int main(int argc, char *argv[]) {
  const char * address = argv[1] ;

    std::shared_ptr<INetClient> netClient = INetClient::singleton();
        std::shared_ptr<INetCallBack> callback(new ECallBack());
    netClient->init(callback);

    netClient->add(address, 8008);
  
    struct ProtoBuffer *probuf = allocateProtoBuffer();

    unsigned int tag = 0x100, ntag = 0x100;
    while(1) {
        //
        probuf->reset(probuf);
        switch(tag) {
            case 0x100:
                       getwindowdata(probuf);
                       ntag = 0x101;
                       break;
            case 0x101:
                      getcolordata(probuf);
                       ntag = 0x102;
                       break;
            case 0x102:
                       getkeydata(probuf);
                       ntag = 0x103;
                       break;
            case 0x103:
                       getlightdata(probuf);
                       ntag = 0x110;
                       break;
            case 0x110:
                      getmoredata(probuf);
                       ntag = 0xFFFFFFFF;
                       break;
            case 0xFFFFFFFF:// test
                       getTestData(probuf);
                       ntag = 0x100;
                       break; 
            default:
                       printf("tag %d error\n", tag);
                       break;
        }

        netClient->send(tag, (unsigned char*)probuf->data(probuf), probuf->dataSize(probuf));
        tag = ntag;
        sleep(1);
    }
    probuf->free(probuf);
    return 0;
}