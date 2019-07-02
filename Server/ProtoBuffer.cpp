/*****************************************************/
/*
**   Author: lirui
**   Date: 2018/11/09
**   File: ProtoBuffer.c
**   Function:  ProtoBuffer implement
**   History:
**       2018/11/09 create by lirui
**
**   Copy Right: iris corp
**
*/
/*****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ProtoBuffer.h"

static const uint32_t  DELTA_DATA_SIZE    = 1024;

typedef struct ProtocolBuffer {
    struct ProtoBuffer   mProtoBuffer ;
    ENDIANTYPE           mEType;
    uint8_t*             mData;
    size_t               mDataSize;//write pos
    size_t               mDataCapacity;//
    size_t               mDataPos;// read pos
} ProtocolBuffer;

///////////////////////////////////////////////////////////////////////////////////
static int ProtocolBuffer_default_resize_buffer(struct ProtocolBuffer *probufData , size_t sz) {
    size_t nDataCapacity = 0 ;
    size_t nDataRate = sz / DELTA_DATA_SIZE ;
    uint8_t *data = NULL ;

    nDataRate += 1 ;
    if (probufData->mData == NULL) {// no malloc memory
        probufData->mDataSize = 0 ;
        probufData->mDataPos    = 0 ;
        probufData->mDataCapacity = nDataRate * DELTA_DATA_SIZE ;
        probufData->mData = (uint8_t*) malloc(probufData->mDataCapacity * sizeof(uint8_t));
    }

    if (probufData->mDataCapacity >= (probufData->mDataSize + sz))
        return 0 ;// memory enough

    nDataCapacity = probufData->mDataCapacity + nDataRate * DELTA_DATA_SIZE ;
    data = (uint8_t*)malloc(nDataCapacity * sizeof(uint8_t)) ;
    memcpy(data , probufData->mData , probufData->mDataSize * sizeof(uint8_t)) ;
    free(probufData->mData) ;
    probufData->mData = data ;
    probufData->mDataCapacity = nDataCapacity ;
    return 0 ;
}

static int ProtocolBuffer_default_setByteOrder(struct ProtoBuffer *probuf, ENDIANTYPE val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    //printf("%s line:%d %x\n" , __FILE__ , __LINE__ , val) ;
    probufData->mEType = val ;
    return 0 ;
}

static int ProtocolBuffer_default_writeInt8(struct ProtoBuffer *probuf, int8_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    //printf("%s line:%d %x\n" , __FILE__ , __LINE__ , val) ;
    probufData->mData[probufData->mDataSize] = (uint8_t) val ;
    probufData->mDataSize += sizeof(val) ;
    return 0;
}

static int ProtocolBuffer_default_writeUInt8(struct ProtoBuffer *probuf, uint8_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    //printf("%s line:%d %x\n" , __FILE__ , __LINE__ , val) ;
    probufData->mData[probufData->mDataSize] = (uint8_t) val ;
    probufData->mDataSize += sizeof(val) ;
    return 0;
}

static int ProtocolBuffer_default_writeInt16(struct ProtoBuffer *probuf, int16_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sizeof(val)) < 0)
        return -1 ;
    //printf("%s line:%d %x\n" , __FILE__ , __LINE__ , val) ;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[probufData->mDataSize] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) val ;
    } else {
        probufData->mData[probufData->mDataSize] = (uint8_t) val ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 8) ;
    }
    probufData->mDataSize += sizeof(val) ;
    return 0;
}

static int ProtocolBuffer_default_writeUInt16(struct ProtoBuffer *probuf, uint16_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    //printf("%s line:%d %x\n" , __FILE__ , __LINE__ , val) ;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[probufData->mDataSize] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) val ;
    } else {
        probufData->mData[probufData->mDataSize] = (uint8_t) val ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 8) ;
    }
    probufData->mDataSize += sizeof(val) ;
    return 0;
}

static int ProtocolBuffer_default_writeInt32(struct ProtoBuffer *probuf, int32_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    //printf("%s line:%d %x\n" , __FILE__ , __LINE__ , val) ;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[probufData->mDataSize] = (uint8_t) (val >> 24) ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) val ;
    } else {
        probufData->mData[probufData->mDataSize] = (uint8_t) val ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) (val >> 24) ;
    }
    probufData->mDataSize += sizeof(val) ;
    return 0;
}

static int ProtocolBuffer_default_writeUInt32(struct ProtoBuffer *probuf, uint32_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    //printf("%s line:%d %x\n" , __FILE__ , __LINE__ , val) ;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[probufData->mDataSize] = (uint8_t) (val >> 24) ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) val ;
    } else {
        probufData->mData[probufData->mDataSize] = (uint8_t) val ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) (val >> 24) ;
    }
    probufData->mDataSize += sizeof(val) ;
    return 0;
}

static int ProtocolBuffer_default_writeInt64(struct ProtoBuffer *probuf, int64_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    //printf("%s line:%d %lx\n" , __FILE__ , __LINE__ , val) ;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[probufData->mDataSize] = (uint8_t) (val >> 56) ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 48) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 40) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) (val >> 32) ;
        probufData->mData[probufData->mDataSize + 4] = (uint8_t) (val >> 24) ;
        probufData->mData[probufData->mDataSize + 5] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 6] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 7] = (uint8_t) val ;
    } else {
        probufData->mData[probufData->mDataSize] = (uint8_t) val ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) (val >> 24) ;
        probufData->mData[probufData->mDataSize + 4] = (uint8_t) (val >> 32) ;
        probufData->mData[probufData->mDataSize + 5] = (uint8_t) (val >> 40) ;
        probufData->mData[probufData->mDataSize + 6] = (uint8_t) (val >> 48) ;
        probufData->mData[probufData->mDataSize + 7] = (uint8_t) (val >> 56) ;
    }
    probufData->mDataSize += sizeof(val) ;
    return 0;
}

static int ProtocolBuffer_default_writeUInt64(struct ProtoBuffer *probuf, uint64_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    //printf("%s line:%d %lx\n" , __FILE__ , __LINE__ , val) ;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[probufData->mDataSize] = (uint8_t) (val >> 56) ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 48) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 40) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) (val >> 32) ;
        probufData->mData[probufData->mDataSize + 4] = (uint8_t) (val >> 24) ;
        probufData->mData[probufData->mDataSize + 5] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 6] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 7] = (uint8_t) val ;
    } else {
        probufData->mData[probufData->mDataSize] = (uint8_t) val ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) (val >> 24) ;
        probufData->mData[probufData->mDataSize + 4] = (uint8_t) (val >> 32) ;
        probufData->mData[probufData->mDataSize + 5] = (uint8_t) (val >> 40) ;
        probufData->mData[probufData->mDataSize + 6] = (uint8_t) (val >> 48) ;
        probufData->mData[probufData->mDataSize + 7] = (uint8_t) (val >> 56) ;
    }
    probufData->mDataSize += sizeof(val) ;
    return 0;
}

static int ProtocolBuffer_default_writeFloat(struct ProtoBuffer *probuf, float value) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint32_t val = (uint32_t) *((uint32_t*)&value) ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    //printf("%s line:%d %f\n" , __FILE__ , __LINE__ , value) ;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[probufData->mDataSize] = (uint8_t) (val >> 24) ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) val ;
    } else {
        probufData->mData[probufData->mDataSize] = (uint8_t) val ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) (val >> 24) ;
    }
    probufData->mDataSize += sizeof(val) ;
    return 0;
}

static int ProtocolBuffer_default_writeDouble(struct ProtoBuffer *probuf, double value) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint64_t val = (uint64_t) *((uint64_t*)&value) ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    //printf("%s line:%d %lf\n" , __FILE__ , __LINE__ , value) ;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[probufData->mDataSize] = (uint8_t) (val >> 56) ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 48) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 40) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) (val >> 32) ;
        probufData->mData[probufData->mDataSize + 4] = (uint8_t) (val >> 24) ;
        probufData->mData[probufData->mDataSize + 5] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 6] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 7] = (uint8_t) val ;
    } else {
        probufData->mData[probufData->mDataSize] = (uint8_t) val ;
        probufData->mData[probufData->mDataSize + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[probufData->mDataSize + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[probufData->mDataSize + 3] = (uint8_t) (val >> 24) ;
        probufData->mData[probufData->mDataSize + 4] = (uint8_t) (val >> 32) ;
        probufData->mData[probufData->mDataSize + 5] = (uint8_t) (val >> 40) ;
        probufData->mData[probufData->mDataSize + 6] = (uint8_t) (val >> 48) ;
        probufData->mData[probufData->mDataSize + 7] = (uint8_t) (val >> 56) ;
    }
    probufData->mDataSize += sizeof(val) ;
    return 0;
}

static int ProtocolBuffer_default_writeCString(struct ProtoBuffer *probuf, const char* val)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    size_t sz = 0 ;
    if (val == NULL)
        return -1 ;

    sz = strlen(val) + 1 ;
    if (ProtocolBuffer_default_resize_buffer(probufData , sz) < 0)
        return -1 ;

    //printf("%s line:%d %s\n" , __FILE__ , __LINE__ , val) ;
    memcpy(probufData->mData + probufData->mDataSize , (const void*)val , sz * sizeof(uint8_t)) ;
    probufData->mDataSize += sz ;
    return 0;
}

static int ProtocolBuffer_default_write(struct ProtoBuffer *probuf, const void* data, size_t sz) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (data == NULL)
        return -1 ;

    if (ProtocolBuffer_default_resize_buffer(probufData , sz) < 0)
        return -1 ;

    //printf("%s line:%d %x\n" , __FILE__ , __LINE__ , sz) ;
    memcpy(probufData->mData + probufData->mDataSize , data , sz * sizeof(uint8_t)) ;
    probufData->mDataSize += sz ;
    return 0;
}

static int ProtocolBuffer_default_check_data(struct ProtocolBuffer *probufData , size_t sz) {
    if ((probufData == NULL) || (probufData->mData == NULL) || (probufData->mDataSize < sz))
        return -1;
    return 0;
}

static int ProtocolBuffer_default_setInt8(struct ProtoBuffer *probuf, size_t index, int8_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_check_data(probufData, index + sizeof(val)) < 0)
        return -1;
    probufData->mData[index] = (uint8_t) val ;
    return 0;
}

static int ProtocolBuffer_default_setUInt8(struct ProtoBuffer *probuf, size_t index, uint8_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_check_data(probufData, index + sizeof(val)) < 0)
        return -1;
    probufData->mData[index] = val ;
    return 0;
}

static int ProtocolBuffer_default_setInt16(struct ProtoBuffer *probuf, size_t index, int16_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_check_data(probufData, index + sizeof(val)) < 0)
        return -1;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[index] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 1] = (uint8_t) val ;
    } else {
        probufData->mData[index] = (uint8_t) val ;
        probufData->mData[index + 1] = (uint8_t) (val >> 8) ;
    }
    return 0;
}

static int ProtocolBuffer_default_setUInt16(struct ProtoBuffer *probuf, size_t index, uint16_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_check_data(probufData, index + sizeof(val)) < 0)
        return -1;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[index] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 1] = (uint8_t) val ;
    } else {
        probufData->mData[index] = (uint8_t) val ;
        probufData->mData[index + 1] = (uint8_t) (val >> 8) ;
    }
    return 0;
}

static int ProtocolBuffer_default_setInt32(struct ProtoBuffer *probuf, size_t index, int32_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_check_data(probufData, index + sizeof(val)) < 0)
        return -1;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[index] = (uint8_t) (val >> 24) ;
        probufData->mData[index + 1] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 3] = (uint8_t) val ;
    } else {
        probufData->mData[index] = (uint8_t) val ;
        probufData->mData[index + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 3] = (uint8_t) (val >> 24) ;
    }
    return 0;
}

static int ProtocolBuffer_default_setUInt32(struct ProtoBuffer *probuf, size_t index, uint32_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_check_data(probufData, index + sizeof(val)) < 0)
        return -1;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[index] = (uint8_t) (val >> 24) ;
        probufData->mData[index + 1] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 3] = (uint8_t) val ;
    } else {
        probufData->mData[index] = (uint8_t) val ;
        probufData->mData[index + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 3] = (uint8_t) (val >> 24) ;
    }
    return 0;
}

static int ProtocolBuffer_default_setInt64(struct ProtoBuffer *probuf, size_t index, int64_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_check_data(probufData, index + sizeof(val)) < 0)
        return -1;

    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[index] = (uint8_t) (val >> 56) ;
        probufData->mData[index + 1] = (uint8_t) (val >> 48) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 40) ;
        probufData->mData[index + 3] = (uint8_t) (val >> 32) ;
        probufData->mData[index + 4] = (uint8_t) (val >> 24) ;
        probufData->mData[index + 5] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 6] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 7] = (uint8_t) val ;
    } else {
        probufData->mData[index] = (uint8_t) val ;
        probufData->mData[index + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 3] = (uint8_t) (val >> 24) ;
        probufData->mData[index + 4] = (uint8_t) (val >> 32) ;
        probufData->mData[index + 5] = (uint8_t) (val >> 40) ;
        probufData->mData[index + 6] = (uint8_t) (val >> 48) ;
        probufData->mData[index + 7] = (uint8_t) (val >> 56) ;
    }
    return 0;
}

static int ProtocolBuffer_default_setUInt64(struct ProtoBuffer *probuf, size_t index, uint64_t val) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_check_data(probufData, index + sizeof(val)) < 0)
        return -1;

    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[index] = (uint8_t) (val >> 56) ;
        probufData->mData[index + 1] = (uint8_t) (val >> 48) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 40) ;
        probufData->mData[index + 3] = (uint8_t) (val >> 32) ;
        probufData->mData[index + 4] = (uint8_t) (val >> 24) ;
        probufData->mData[index + 5] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 6] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 7] = (uint8_t) val ;
    } else {
        probufData->mData[index] = (uint8_t) val ;
        probufData->mData[index + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 3] = (uint8_t) (val >> 24) ;
        probufData->mData[index + 4] = (uint8_t) (val >> 32) ;
        probufData->mData[index + 5] = (uint8_t) (val >> 40) ;
        probufData->mData[index + 6] = (uint8_t) (val >> 48) ;
        probufData->mData[index + 7] = (uint8_t) (val >> 56) ;
    }
    return 0;
}

static int ProtocolBuffer_default_setFloat(struct ProtoBuffer *probuf, size_t index, float value) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint32_t val = (uint32_t) *((uint32_t*)&value) ;
    if (ProtocolBuffer_default_check_data(probufData, index + sizeof(val)) < 0)
        return -1;
    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[index] = (uint8_t) (val >> 24) ;
        probufData->mData[index + 1] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 3] = (uint8_t) val ;
    } else {
        probufData->mData[index] = (uint8_t) val ;
        probufData->mData[index + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 3] = (uint8_t) (val >> 24) ;
    }
    return 0;
}

static int ProtocolBuffer_default_setDouble(struct ProtoBuffer *probuf, size_t index, double value) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint64_t val = (uint64_t) *((uint64_t*)&value) ;
    if (ProtocolBuffer_default_check_data(probufData, index + sizeof(val)) < 0)
        return -1;

    if (probufData->mEType == BIGENDIAN) {
        probufData->mData[index] = (uint8_t) (val >> 56) ;
        probufData->mData[index + 1] = (uint8_t) (val >> 48) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 40) ;
        probufData->mData[index + 3] = (uint8_t) (val >> 32) ;
        probufData->mData[index + 4] = (uint8_t) (val >> 24) ;
        probufData->mData[index + 5] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 6] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 7] = (uint8_t) val ;
    } else {
        probufData->mData[index] = (uint8_t) val ;
        probufData->mData[index + 1] = (uint8_t) (val >> 8) ;
        probufData->mData[index + 2] = (uint8_t) (val >> 16) ;
        probufData->mData[index + 3] = (uint8_t) (val >> 24) ;
        probufData->mData[index + 4] = (uint8_t) (val >> 32) ;
        probufData->mData[index + 5] = (uint8_t) (val >> 40) ;
        probufData->mData[index + 6] = (uint8_t) (val >> 48) ;
        probufData->mData[index + 7] = (uint8_t) (val >> 56) ;
    }
    return 0;
}

static int ProtocolBuffer_default_setCString(struct ProtoBuffer *probuf, size_t index, const char* val)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    size_t sz = 0 ;
    if (val == NULL)
        return -1 ;

    sz = strlen(val) + 1 ;
    if (ProtocolBuffer_default_check_data(probufData, index + sz) < 0)
        return -1;

    //printf("%s line:%d %s\n" , __FILE__ , __LINE__ , val) ;
    memcpy(probufData->mData + index , (const void*)val , sz * sizeof(uint8_t)) ;
    return 0;
}

static int ProtocolBuffer_default_set(struct ProtoBuffer *probuf, size_t index, const void* data, size_t sz) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (data == NULL)
        return -1 ;

    if (ProtocolBuffer_default_check_data(probufData, index + sz) < 0)
        return -1;

    //printf("%s line:%d %x\n" , __FILE__ , __LINE__ , sz) ;
    memcpy(probufData->mData + index , data , sz * sizeof(uint8_t)) ;
    return 0;
}

static void ProtocolBuffer_default_reset(struct ProtoBuffer *probuf) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (probufData == NULL)
        return  ;

    //printf("%s line:%d \n" , __FILE__ , __LINE__) ;
    probufData->mDataSize = 0;//    probufData->mDataCapacity = 0;
    probufData->mDataPos = 0;
}

static	size_t ProtocolBuffer_default_dataSize(struct ProtoBuffer *probuf) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (probufData == NULL)
        return 0 ;
    return probufData->mDataSize;
}

static	int ProtocolBuffer_default_setDataSize(struct ProtoBuffer *probuf, size_t size) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (ProtocolBuffer_default_resize_buffer(probufData , size) < 0)
        return -1 ;
    probufData->mDataSize = size;
    return 0;
}

static	size_t ProtocolBuffer_default_dataPosition(struct ProtoBuffer *probuf) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (probufData == NULL)
        return 0 ;
    return probufData->mDataPos;
}

static	int ProtocolBuffer_default_setDataPosition(struct ProtoBuffer *probuf, size_t pos) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (probufData == NULL)
        return -1 ;
    if ((probufData->mData == NULL) || (probufData->mDataSize < pos))
        return -1 ;
    probufData->mDataPos = pos;
    return 0 ;
}

static	size_t ProtocolBuffer_default_dataCapacity(struct ProtoBuffer *probuf) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (probufData == NULL)
        return 0 ;
    return probufData->mDataCapacity;
}

static	int ProtocolBuffer_default_setDataCapacity(struct ProtoBuffer *probuf, size_t size) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (probufData == NULL)
        return -1 ;
    if (probufData->mData == NULL) {
        probufData->mDataSize = 0;
        probufData->mDataPos = 0;
        uint8_t *data =(uint8_t*)malloc(size * sizeof(uint8_t)) ;
        probufData->mData = data ;
        probufData->mDataCapacity = size ;
        return 0 ;
    }

    if (probufData->mDataCapacity == size) 
        return 0;

    if (probufData->mDataCapacity < size) {
        uint8_t *data =(uint8_t*)malloc(size * sizeof(uint8_t)) ;
        if (probufData->mDataSize)
            memcpy(data , probufData->mData , probufData->mDataSize * sizeof(uint8_t)) ;
        free(probufData->mData) ;
        probufData->mData = data ;
        probufData->mDataCapacity = size ;
        return 0 ;
    }

    uint8_t *data =(uint8_t*)malloc(size * sizeof(uint8_t)) ;
    if (probufData->mDataSize > size)
        probufData->mDataSize = size;
    if (probufData->mDataPos > size)
        probufData->mDataPos = size ;
    if (probufData->mDataSize)
        memcpy(data , probufData->mData , probufData->mDataSize * sizeof(uint8_t)) ;
    free(probufData->mData) ;
    probufData->mData = data ;
    probufData->mDataCapacity = size ;
    return 0;
}

static void ProtocolBuffer_default_free(struct ProtoBuffer *probuf) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (probufData == NULL)
        return ;
    //printf("%s line:%d \n" , __FILE__ , __LINE__) ;
    if (probufData->mData)
        free(probufData->mData) ;
    probufData->mData = NULL;
    probufData->mDataSize = 0;
    probufData->mDataCapacity = 0;
    probufData->mDataPos = 0;
    free(probufData) ;
}

int ProtocolBuffer_default_getByteOrder(struct ProtoBuffer *probuf) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    //printf("%s line:%d \n" , __FILE__ , __LINE__) ;
    return probufData->mEType ;
}

void* ProtocolBuffer_default_data(struct ProtoBuffer *probuf) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    if (probufData == NULL)
        return  NULL;
    return probufData->mData;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
static int  ProtocolBuffer_default_check_buffer(struct ProtocolBuffer *probufData , size_t sz) {
    if (probufData == NULL)
        return -1 ;
    if ((probufData->mData == NULL) || (probufData->mDataSize < (probufData->mDataPos + sz)))
        return -1 ;
    return 0 ;
}

static int8_t ProtocolBuffer_default_readInt8(struct ProtoBuffer *probuf)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    int8_t val = 0 ;
    if (ProtocolBuffer_default_check_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    val = probufData->mData[probufData->mDataPos];
    probufData->mDataPos += sizeof(val) ;
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return val ;
}

static uint8_t ProtocolBuffer_default_readUInt8(struct ProtoBuffer *probuf)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint8_t val = 0 ;
    if (ProtocolBuffer_default_check_buffer(probufData , sizeof(val)) < 0)
        return -1 ;

    val = probufData->mData[probufData->mDataPos];
    probufData->mDataPos += sizeof(val) ;
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return val ;
}

static int16_t ProtocolBuffer_default_readInt16(struct ProtoBuffer *probuf)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint16_t val = 0 ;
    if (ProtocolBuffer_default_check_buffer(probufData , sizeof(val)) < 0)
        return -1 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[probufData->mDataPos] ;
        val = ((val << 8) + probufData->mData[probufData->mDataPos + 1]) ;
    } else {
        val = probufData->mData[probufData->mDataPos + 1] ;
        val = ((val << 8) + probufData->mData[probufData->mDataPos]) ;
    }
    probufData->mDataPos += sizeof(val) ;
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return (int16_t)val ;
}

static uint16_t ProtocolBuffer_default_readUInt16(struct ProtoBuffer *probuf)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint16_t val = 0 ;
    if (ProtocolBuffer_default_check_buffer(probufData , sizeof(val)) < 0)
        return 0 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[probufData->mDataPos] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
    } else {
        val = probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos] ;
    }
    probufData->mDataPos += sizeof(val) ;
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return val ;
}

static int32_t ProtocolBuffer_default_readInt32(struct ProtoBuffer *probuf)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint32_t val = 0 ;
    if (ProtocolBuffer_default_check_buffer(probufData , sizeof(val)) < 0)
        return -1 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[probufData->mDataPos] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 3] ;
    } else {
        val = probufData->mData[probufData->mDataPos + 3] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos] ;
    }
    probufData->mDataPos += sizeof(val) ;
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return (int32_t)val ;
}

static uint32_t ProtocolBuffer_default_readUInt32(struct ProtoBuffer *probuf)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint32_t val = 0 ;
    if (ProtocolBuffer_default_check_buffer(probufData , sizeof(val)) < 0)
        return -1 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[probufData->mDataPos] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 3] ;
    } else {
        val = probufData->mData[probufData->mDataPos + 3] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos] ;
    }
    probufData->mDataPos += sizeof(val) ;
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return val ;
}

static int64_t ProtocolBuffer_default_readInt64(struct ProtoBuffer *probuf)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint64_t val = 0 ;
    if (ProtocolBuffer_default_check_buffer(probufData , sizeof(val)) < 0)
        return -1 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[probufData->mDataPos] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 3] ;

        val = (val << 8) + probufData->mData[probufData->mDataPos + 4] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 5] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 6] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 7] ;
    } else {
        val = probufData->mData[probufData->mDataPos + 7] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 6] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 5] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 4] ;

        val = (val << 8) + probufData->mData[probufData->mDataPos + 3] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos] ;
    }
    probufData->mDataPos += sizeof(val) ;
    //printf("%s line:%d %ld\n" , __FILE__ , __LINE__ , val) ;
    return (int64_t)val ;
}

static uint64_t ProtocolBuffer_default_readUInt64(struct ProtoBuffer *probuf)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint64_t val = 0 ;
    if (ProtocolBuffer_default_check_buffer(probufData , sizeof(val)) < 0)
        return -1 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[probufData->mDataPos] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 3] ;

        val = (val << 8) + probufData->mData[probufData->mDataPos + 4] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 5] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 6] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 7] ;
    } else {
        val = probufData->mData[probufData->mDataPos + 7] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 6] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 5] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 4] ;

        val = (val << 8) + probufData->mData[probufData->mDataPos + 3] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos] ;
    }
    probufData->mDataPos += sizeof(val) ;
    //printf("%s line:%d %ld\n" , __FILE__ , __LINE__ , val) ;
    return val ;
}

static float ProtocolBuffer_default_readFloat(struct ProtoBuffer *probuf)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint32_t val = 0 ;
    if (ProtocolBuffer_default_check_buffer(probufData , sizeof(val)) < 0)
        return (float)0.0 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[probufData->mDataPos] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 3] ;
    } else {
        val = probufData->mData[probufData->mDataPos + 3] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos] ;
    }
    probufData->mDataPos += sizeof(val) ;
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return (float) *((float*)&val) ;
}

static double ProtocolBuffer_default_readDouble(struct ProtoBuffer *probuf)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint64_t val = 0 ;
    if (ProtocolBuffer_default_check_buffer(probufData , sizeof(val)) < 0)
        return (double)0.0 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[probufData->mDataPos] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 3] ;

        val = (val << 8) + probufData->mData[probufData->mDataPos + 4] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 5] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 6] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 7] ;
    } else {
        val = probufData->mData[probufData->mDataPos + 7] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 6] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 5] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 4] ;

        val = (val << 8) + probufData->mData[probufData->mDataPos + 3] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 2] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos + 1] ;
        val = (val << 8) + probufData->mData[probufData->mDataPos] ;
    }
    probufData->mDataPos += sizeof(val) ;
    //printf("%s line:%d %ld\n" , __FILE__ , __LINE__ , val) ;
    return (double) *((double*)&val) ; ;
}

static const char* ProtocolBuffer_default_readCString(struct ProtoBuffer *probuf) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    int sz = 0 ;
    const char *szString = NULL ;
    const char *szEOS = NULL ;
    if (probuf == NULL)
        return NULL ;
    if ((probufData->mData == NULL) || (probufData->mDataSize <= probufData->mDataPos))
        return NULL ;
    sz = probufData->mDataSize - probufData->mDataPos;
    szString = (const char*)(probufData->mData + probufData->mDataPos);
            // is the string's trailing NUL within the parcel's valid bounds?
    szEOS = (const char*)(memchr(szString, 0, (size_t)sz));

    if (!szEOS)
       return NULL ;

    sz = szEOS - szString;
    probufData->mDataPos += (sz + 1);
    return szString;
}

static int8_t ProtocolBuffer_default_getInt8(struct ProtoBuffer *probuf, size_t index) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint8_t val = 0 ;
    if (ProtocolBuffer_default_check_data(probufData , index + sizeof(val)) < 0)
        return -1;

    val = probufData->mData[index];
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return (int8_t)val ;
}

static uint8_t ProtocolBuffer_default_getUInt8(struct ProtoBuffer *probuf, size_t index) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint8_t val = 0 ;
    if (ProtocolBuffer_default_check_data(probufData , index + sizeof(val)) < 0)
        return 0;

    val = probufData->mData[index];
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return val ;
}

static int16_t ProtocolBuffer_default_getInt16(struct ProtoBuffer *probuf, size_t index)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint16_t val = 0 ;
    if (ProtocolBuffer_default_check_data(probufData , index + sizeof(val)) < 0)
        return -1;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[index] ;
        val = (val << 8) + probufData->mData[index + 1] ;
    } else {
        val = probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index] ;
    }
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return (int16_t)val ;
}

static uint16_t ProtocolBuffer_default_getUInt16(struct ProtoBuffer *probuf, size_t index)  {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint16_t val = 0 ;
    if (ProtocolBuffer_default_check_data(probufData , index + sizeof(val)) < 0)
        return 0;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[index] ;
        val = (val << 8) + probufData->mData[index + 1] ;
    } else {
        val = probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index] ;
    }
    //printf("%s line:%d %d\n" , __FILE__ , __LINE__ , val) ;
    return val ;
}

static int32_t ProtocolBuffer_default_getInt32(struct ProtoBuffer *probuf, size_t index) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint32_t val = 0 ;
    if (ProtocolBuffer_default_check_data(probufData , index + sizeof(val)) < 0)
        return -1;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[index] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 3] ;

    } else {
        val = probufData->mData[index + 3] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index] ;
    }
    //printf("%s line:%d %ld\n" , __FILE__ , __LINE__ , val) ;
    return (int32_t)val;
}

static uint32_t ProtocolBuffer_default_getUInt32(struct ProtoBuffer *probuf, size_t index) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint32_t val = 0 ;
    if (ProtocolBuffer_default_check_data(probufData , index + sizeof(val)) < 0)
        return 0;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[index] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 3] ;

    } else {
        val = probufData->mData[index + 3] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index] ;
    }
    //printf("%s line:%d %ld\n" , __FILE__ , __LINE__ , val) ;
    return val;
}

static int64_t ProtocolBuffer_default_getInt64(struct ProtoBuffer *probuf, size_t index) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint64_t val = -1 ;
    if (ProtocolBuffer_default_check_data(probufData , index + sizeof(val)) < 0)
        return (int64_t)val;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[index] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 3] ;

        val = (val << 8) + probufData->mData[index + 4] ;
        val = (val << 8) + probufData->mData[index + 5] ;
        val = (val << 8) + probufData->mData[index + 6] ;
        val = (val << 8) + probufData->mData[index + 7] ;
    } else {
        val = probufData->mData[index + 7] ;
        val = (val << 8) + probufData->mData[index + 6] ;
        val = (val << 8) + probufData->mData[index + 5] ;
        val = (val << 8) + probufData->mData[index + 4] ;

        val = (val << 8) + probufData->mData[index + 3] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index] ;
    }
    //printf("%s line:%d %ld\n" , __FILE__ , __LINE__ , val) ;
    return (int64_t)val;
}

static uint64_t ProtocolBuffer_default_getUInt64(struct ProtoBuffer *probuf, size_t index) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint64_t val = 0 ;
    if (ProtocolBuffer_default_check_data(probufData , index + sizeof(val)) < 0)
        return 0 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[index] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 3] ;

        val = (val << 8) + probufData->mData[index + 4] ;
        val = (val << 8) + probufData->mData[index + 5] ;
        val = (val << 8) + probufData->mData[index + 6] ;
        val = (val << 8) + probufData->mData[index + 7] ;
    } else {
        val = probufData->mData[index + 7] ;
        val = (val << 8) + probufData->mData[index + 6] ;
        val = (val << 8) + probufData->mData[index + 5] ;
        val = (val << 8) + probufData->mData[index + 4] ;

        val = (val << 8) + probufData->mData[index + 3] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index] ;
    }
    //printf("%s line:%d %ld\n" , __FILE__ , __LINE__ , val) ;
    return val;
}

static float ProtocolBuffer_default_getFloat(struct ProtoBuffer *probuf, size_t index) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint32_t val = 0 ;
    if (ProtocolBuffer_default_check_data(probufData , index + sizeof(val)) < 0)
        return (float)0.0 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[index] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 3] ;

    } else {
        val = probufData->mData[index + 3] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index] ;
    }
    //printf("%s line:%d %ld\n" , __FILE__ , __LINE__ , val) ;
    return (float) *((float*)&val);
}

static double ProtocolBuffer_default_getDouble(struct ProtoBuffer *probuf, size_t index) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    uint64_t val = 0 ;
    if (ProtocolBuffer_default_check_data(probufData , index + sizeof(val)) < 0)
        return (double)0.0 ;
    if (probufData->mEType == BIGENDIAN) {
        val = probufData->mData[index] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 3] ;

        val = (val << 8) + probufData->mData[index + 4] ;
        val = (val << 8) + probufData->mData[index + 5] ;
        val = (val << 8) + probufData->mData[index + 6] ;
        val = (val << 8) + probufData->mData[index + 7] ;
    } else {
        val = probufData->mData[index + 7] ;
        val = (val << 8) + probufData->mData[index + 6] ;
        val = (val << 8) + probufData->mData[index + 5] ;
        val = (val << 8) + probufData->mData[index + 4] ;

        val = (val << 8) + probufData->mData[index + 3] ;
        val = (val << 8) + probufData->mData[index + 2] ;
        val = (val << 8) + probufData->mData[index + 1] ;
        val = (val << 8) + probufData->mData[index] ;
    }
    //printf("%s line:%d %ld\n" , __FILE__ , __LINE__ , val) ;
    return (double) *((double*)&val);
}

static const char* ProtocolBuffer_default_getCString(struct ProtoBuffer *probuf, size_t index) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    int sz = 0 ;
    const char *szString = NULL ;
    const char *szEOS = NULL ;
    if (probuf == NULL)
        return NULL ;
    if ((probufData->mData == NULL) || (probufData->mDataSize <= index))
        return NULL ;
    sz = probufData->mDataSize - index;
    szString = (const char*)(probufData->mData + index);
            // is the string's trailing NUL within the parcel's valid bounds?
    szEOS = (const char*)(memchr(szString, 0, (size_t)sz));

    if (!szEOS)
       return NULL ;
    return szString;
}

static int64_t ProtocolBuffer_default_read(struct ProtoBuffer *probuf, void* data, size_t sz) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    size_t nDSize = 0 ;
    if ((probuf == NULL) || (data == NULL) || (sz <= 0))
        return -1 ;

    if ((probufData->mData == NULL) || (probufData->mDataSize <= probufData->mDataPos))
        return -1 ;

    nDSize = probufData->mDataSize - probufData->mDataPos;
    if (sz < nDSize)
        nDSize = sz ;
    memcpy(data , probufData->mData + probufData->mDataPos , nDSize * sizeof(int8_t)) ;
    probufData->mDataPos += nDSize ;
    return nDSize ;
}

static int64_t ProtocolBuffer_default_get(struct ProtoBuffer *probuf, size_t index, void* data, size_t size) {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) probuf ;
    size_t nDSize = 0 ;
    if ((probuf == NULL) || (data == NULL) || (size <= 0))
        return -1 ;

    if ((probufData->mData == NULL) || (probufData->mDataSize <= index))
        return -1 ;

    nDSize = probufData->mDataSize - index;
    if (size < nDSize)
        nDSize = size ;
    memcpy(data , probufData->mData + index , nDSize * sizeof(int8_t)) ;
    return nDSize ;
}

/*************************************************************************************/


#if defined(__cplusplus)
extern "C" {
#endif

__attribute__((visibility("default"))) struct ProtoBuffer* allocateProtoBuffer() {
    struct ProtocolBuffer *probufData = (struct ProtocolBuffer*) malloc(sizeof(struct ProtocolBuffer)) ;
    struct ProtoBuffer *probuf = (struct ProtoBuffer*)probufData ;
    if (probufData == NULL)
        return NULL ;

    memset(probufData , 0 , sizeof(struct ProtocolBuffer));
    probufData->mEType = BIGENDIAN ; //Big-Endian
    probufData->mDataCapacity = DELTA_DATA_SIZE ;
    probufData->mData = (uint8_t*) malloc(probufData->mDataCapacity * sizeof(uint8_t));

    probuf->reset = ProtocolBuffer_default_reset;
    probuf->free = ProtocolBuffer_default_free;

    probuf->dataSize = ProtocolBuffer_default_dataSize;
    probuf->dataPosition = ProtocolBuffer_default_dataPosition;

    probuf->setDataSize = ProtocolBuffer_default_setDataSize;
    probuf->setDataPosition = ProtocolBuffer_default_setDataPosition;

    probuf->data = ProtocolBuffer_default_data;
    probuf->dataCapacity = ProtocolBuffer_default_dataCapacity;
    probuf->setDataCapacity = ProtocolBuffer_default_setDataCapacity;
//
    probuf->setByteOrder = ProtocolBuffer_default_setByteOrder;
    probuf->getByteOrder = ProtocolBuffer_default_getByteOrder;

    probuf->writeInt8 = ProtocolBuffer_default_writeInt8;
    probuf->writeUInt8 = ProtocolBuffer_default_writeUInt8;

    probuf->writeInt16 = ProtocolBuffer_default_writeInt16;
    probuf->writeUInt16 = ProtocolBuffer_default_writeUInt16;

    probuf->writeInt32 = ProtocolBuffer_default_writeInt32;
    probuf->writeUInt32 = ProtocolBuffer_default_writeUInt32;

    probuf->writeInt64 = ProtocolBuffer_default_writeInt64;
    probuf->writeUInt64 = ProtocolBuffer_default_writeUInt64;

    probuf->writeFloat = ProtocolBuffer_default_writeFloat;
    probuf->writeDouble = ProtocolBuffer_default_writeDouble;

    probuf->writeCString = ProtocolBuffer_default_writeCString;
    probuf->write = ProtocolBuffer_default_write;
//
    probuf->setInt8 = ProtocolBuffer_default_setInt8;
    probuf->setUInt8 = ProtocolBuffer_default_setUInt8;
    probuf->setInt16 = ProtocolBuffer_default_setInt16;
    probuf->setUInt16 = ProtocolBuffer_default_setUInt16;
    probuf->setInt32 = ProtocolBuffer_default_setInt32;
    probuf->setUInt32 = ProtocolBuffer_default_setUInt32;
    probuf->setInt64 = ProtocolBuffer_default_setInt64;
    probuf->setUInt64 = ProtocolBuffer_default_setUInt64;

    probuf->setFloat = ProtocolBuffer_default_setFloat;
    probuf->setDouble = ProtocolBuffer_default_setDouble;

    probuf->setCString = ProtocolBuffer_default_setCString;

    probuf->set = ProtocolBuffer_default_set;
//

    probuf->readInt8 = ProtocolBuffer_default_readInt8;
    probuf->readUInt8 = ProtocolBuffer_default_readUInt8;

    probuf->readInt16 = ProtocolBuffer_default_readInt16;
    probuf->readUInt16 = ProtocolBuffer_default_readUInt16;

    probuf->readInt32 = ProtocolBuffer_default_readInt32;
    probuf->readUInt32 = ProtocolBuffer_default_readUInt32;

    probuf->readInt64 = ProtocolBuffer_default_readInt64;
    probuf->readUInt64 = ProtocolBuffer_default_readUInt64;

    probuf->readFloat = ProtocolBuffer_default_readFloat;
    probuf->readDouble = ProtocolBuffer_default_readDouble;

    probuf->readCString = ProtocolBuffer_default_readCString;
    probuf->read = ProtocolBuffer_default_read;

//
    probuf->getInt8 = ProtocolBuffer_default_getInt8;
    probuf->getUInt8 = ProtocolBuffer_default_getUInt8;
    probuf->getInt16 = ProtocolBuffer_default_getInt16;
    probuf->getUInt16 = ProtocolBuffer_default_getUInt16;
    probuf->getInt32 = ProtocolBuffer_default_getInt32;
    probuf->getUInt32 = ProtocolBuffer_default_getUInt32;
    probuf->getInt64 = ProtocolBuffer_default_getInt64;
    probuf->getUInt64 = ProtocolBuffer_default_getUInt64;
    probuf->getFloat = ProtocolBuffer_default_getFloat;
    probuf->getDouble = ProtocolBuffer_default_getDouble;
    probuf->getCString = ProtocolBuffer_default_getCString;

    probuf->get = ProtocolBuffer_default_get;
    return probuf ;
}

#if defined(__cplusplus)
}
#endif
