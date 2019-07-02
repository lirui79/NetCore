/*****************************************************/
/*
**   Author: lirui
**   Date: 2018/11/09
**   File: ProtoBuffer.h
**   Function:  interface of protocol buffer bytes data
**   History:
**       2018/11/09 create by lirui
**
**   Copy Right:
**
*/
/*****************************************************/
#ifndef __PROTOCOL_BYTE_BUFFER_H__
#define __PROTOCOL_BYTE_BUFFER_H__

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum ENDIANTYPE {
    LITTLEENDIAN = 0 ,   //  0 - little-Endian
    BIGENDIAN    = 1 ,   //  1 - Big-Endian
} ENDIANTYPE;


typedef struct ProtoBuffer {

    /**
     * @brief 析构ProtoBuffer指针
     *
     * @param[in] probuf  ProtoBuffer指针变量
     *
     * @example
     *  struct ProtoBuffer *probuf = allocateProtoBuffer();
     *
     *  probuf->free(probuf) ;
     */
    void (*free)(struct ProtoBuffer *probuf);

    /**
	 * @brief 重置数据包不销毁内存
     *
     * @param[in] probuf  ProtoBuffer指针变量
     *
     * @example
     *  struct ProtoBuffer *probuf = allocateProtoBuffer();
     *  if (probuf)
     *     probuf->reset(probuf) ;
     *  probuf->free(probuf) ;
     */
	void (*reset)(struct ProtoBuffer *probuf);

    /**
	 * @brief 数据包数据内容所占大小
     *
     * @param[in] probuf  ProtoBuffer指针变量
     *
     * @return 内容所占大小
     *
     * @example
     *  struct ProtoBuffer *probuf = allocateProtoBuffer();
     *  if (probuf)
     *     probuf->dataSize(probuf) ;
     *  probuf->free(probuf) ;
     */
	size_t (*dataSize)(struct ProtoBuffer *probuf);

    /**
	 * @brief 设置数据包数据内容所占大小
     *
     * @param[in] probuf  ProtoBuffer指针变量
     * @param[in] size    ProtoBuffer内容大小
     *
     * @return
     * 	<em>>=0</em> 成功
     *	<em><0</em> 失败
     *
     * @example
     *  struct ProtoBuffer *probuf = allocateProtoBuffer();
     *  if (probuf)
     *     probuf->setDataSize(probuf, 10) ;
     *  probuf->free(probuf) ;
     */
	int (*setDataSize)(struct ProtoBuffer *probuf, size_t size);

    /**
	 * @brief 数据包数据内容读取位置
     *
     * @param[in] probuf  ProtoBuffer指针变量
     *
     * @return 内容读取位置
     *
     * @example
     *  struct ProtoBuffer *probuf = allocateProtoBuffer();
     *  if (probuf)
     *     probuf->dataPosition(probuf) ;
     *  probuf->free(probuf) ;
     */
	size_t (*dataPosition)(struct ProtoBuffer *probuf);

    /**
	 * @brief 设置数据包数据内容读取位置
     *
     * @param[in] probuf  ProtoBuffer指针变量
     * @param[in] pos     ProtoBuffer内容读取位置
     *
     * @return
     * 	<em>>=0</em> 成功
     *	<em><0</em> 失败
     *
     * @example
     *  struct ProtoBuffer *probuf = allocateProtoBuffer();
     *  if (probuf)
     *     probuf->setDataPosition(probuf, 1) ;
     *  probuf->free(probuf) ;
     */
	int (*setDataPosition)(struct ProtoBuffer *probuf, size_t pos);

    size_t (*dataCapacity)(struct ProtoBuffer *probuf);

    int (*setDataCapacity)(struct ProtoBuffer *probuf, size_t size);

    void* (*data)(struct ProtoBuffer *probuf);

	int (*setByteOrder)(struct ProtoBuffer *probuf, ENDIANTYPE val);
	int (*getByteOrder)(struct ProtoBuffer *probuf);


	int (*writeInt8)(struct ProtoBuffer *probuf, int8_t val);

	int (*writeUInt8)(struct ProtoBuffer *probuf, uint8_t val);

	int (*writeInt16)(struct ProtoBuffer *probuf, int16_t val);

	int (*writeUInt16)(struct ProtoBuffer *probuf, uint16_t val);

	int (*writeInt32)(struct ProtoBuffer *probuf, int32_t val);

	int (*writeUInt32)(struct ProtoBuffer *probuf, uint32_t val);

	int (*writeInt64)(struct ProtoBuffer *probuf, int64_t val);

	int (*writeUInt64)(struct ProtoBuffer *probuf, uint64_t val);

	int (*writeFloat)(struct ProtoBuffer *probuf, float val);

	int (*writeDouble)(struct ProtoBuffer *probuf, double val);

	int (*writeCString)(struct ProtoBuffer *probuf, const char* val);

	int (*write)(struct ProtoBuffer *probuf, const void* data, size_t len);

    /**
	 * @brief 往数据包某位置写入8位有符号整型值，写入位置索引不变
	 *
	 * @param[in] probuf  ProtoBuffer指针变量
	 * @param[in] index   位置
	 * @param[in] val     8位有符号整型值
	 *
	 * @return
	 * 	<em>0</em> 成功
	 *	<em>-1</em> 失败
     *
     * @example
     *  struct ProtoBuffer *probuf = allocateProtoBuffer();
     *  int8_t ival8 = 0x7f ;
     *  probuf->writeInt8(probuf,ival8) ;
     *  probuf->free(probuf) ;
     */
	int (*setInt8)(struct ProtoBuffer *probuf, size_t index, int8_t val);

	int (*setUInt8)(struct ProtoBuffer *probuf, size_t index, uint8_t val);

	int (*setInt16)(struct ProtoBuffer *probuf, size_t index, int16_t val);

	int (*setUInt16)(struct ProtoBuffer *probuf, size_t index, uint16_t val);

	int (*setInt32)(struct ProtoBuffer *probuf, size_t index, int32_t val);

	int (*setUInt32)(struct ProtoBuffer *probuf, size_t index, uint32_t val);

	int (*setInt64)(struct ProtoBuffer *probuf, size_t index, int64_t val);

	int (*setUInt64)(struct ProtoBuffer *probuf, size_t index, uint64_t val);

	int (*setFloat)(struct ProtoBuffer *probuf, size_t index, float val);

	int (*setDouble)(struct ProtoBuffer *probuf, size_t index, double val);

	int (*setCString)(struct ProtoBuffer *probuf, size_t index, const char* val);

	int (*set)(struct ProtoBuffer *probuf, size_t index, const void* data, size_t len);

     /**
 	 * @brief 从数据包的中读取8位有符号整型值
 	 *
     * @param[in] probuf  ProtoBuffer指针变量
 	 *
 	 * @return
 	 * 	  读取8位有符号整型值
     *
     * @example
     *  struct ProtoBuffer *probuf = allocateProtoBuffer();
     *  probuf->writeInt8(probuf,98) ;
     *  printf("int8_t : %x \n" , probuf->readInt8(probuf)) ;
     *  probuf->free(probuf) ;
     */
 	 int8_t (*readInt8)(struct ProtoBuffer *probuf);

 	 uint8_t (*readUInt8)(struct ProtoBuffer *probuf);

 	int16_t (*readInt16)(struct ProtoBuffer *probuf);

 	uint16_t (*readUInt16)(struct ProtoBuffer *probuf);

 	int32_t (*readInt32)(struct ProtoBuffer *probuf);

 	uint32_t (*readUInt32)(struct ProtoBuffer *probuf);

 	int64_t (*readInt64)(struct ProtoBuffer *probuf);

 	uint64_t (*readUInt64)(struct ProtoBuffer *probuf);

 	float (*readFloat)(struct ProtoBuffer *probuf);

 	double (*readDouble)(struct ProtoBuffer *probuf);

 	const char* (*readCString)(struct ProtoBuffer *probuf);
//

    int8_t (*getInt8)(struct ProtoBuffer *probuf, size_t index);

    uint8_t (*getUInt8)(struct ProtoBuffer *probuf, size_t index);

    int16_t (*getInt16)(struct ProtoBuffer *probuf, size_t index);

    uint16_t (*getUInt16)(struct ProtoBuffer *probuf, size_t index);

    int32_t (*getInt32)(struct ProtoBuffer *probuf, size_t index);

    uint32_t (*getUInt32)(struct ProtoBuffer *probuf, size_t index);

    int64_t (*getInt64)(struct ProtoBuffer *probuf, size_t index);

    uint64_t (*getUInt64)(struct ProtoBuffer *probuf, size_t index);

    float (*getFloat)(struct ProtoBuffer *probuf, size_t index);

    double (*getDouble)(struct ProtoBuffer *probuf, size_t index);

    const char* (*getCString)(struct ProtoBuffer *probuf, size_t index);

     /**
 	 * @brief 从数据包的中读取数组
 	 *
     * @param[in] probuf  ProtoBuffer指针变量
 	 * @param[out] data   数组首地址
 	 * @param[in] len     数组字节大小
 	 *
 	 * @return
 	 * 	<em>>0</em> 成功
 	 *	<em><=0</em> 失败
     *
     * @example
     *  struct ProtoBuffer *probuf = allocateProtoBuffer();
     *  int8_t data[8] = {1 , 9 , 43 , 8 , 14 , 76 , 92 , 57} , buffer[8];
     *  int64_t sz = 0 ;
     *  probuf->write(probuf,data,8) ;
     *  sz = probuf->read(probuf,buffer,8) ;
     *  probuf->free(probuf) ;
     */
 	int64_t (*read)(struct ProtoBuffer *probuf, void* data, size_t size);

 	int64_t (*get)(struct ProtoBuffer *probuf, size_t index, void* data, size_t size);

}ProtoBuffer;

/**
 * @brief 分配一个ProtoBuffer结构
 *
 * @return
 * 	<em>非NULL</em> ProtoBuffer 对象指针
 * 	<em>NULL</em>  失败
 *
 * @example
 *  struct ProtoBuffer *probuf = allocateProtoBuffer();
 *  if (probuf)
 *     probuf->writeCString(probuf,"debug") ;
 *  probuf->free(probuf) ;
 */
typedef struct ProtoBuffer* (*allocateProtoBufferType)() ;
__attribute__((visibility("default"))) struct ProtoBuffer* allocateProtoBuffer() ;

#if defined(__cplusplus)
}
#endif

#endif //
