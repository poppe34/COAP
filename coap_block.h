/*
 * coap_block.h
 *
 *  Created on: Jan 14, 2014
 *      Author: poppe
 */

#ifndef COAP_BLOCK_H_
#define COAP_BLOCK_H_

#define blockSZXtoSize(szx)		1 << (szx + 4)
#define blockSizetoSZX(size) 	size >> 5

#define declareBlockVar(buffer, size, count) \
											uint8_t buffer[size+1]; 								\
											uint8_t *buffer##Ptr = buffer; 						\
											uint32_t count##Running = 0; 						\
											uint32_t count##Start = pkt->block.num * size; 	\
											uint32_t count##End = count##Start + size; \
											uint32_t Len;


#define blockAddChar(buffer, count, char) do { 			\
	if(count##Start <= count##Running && count##Running < count##End ) 	\
	{ 													\
		*buffer##Ptr++ = char; 							\
	} 													\
	count##Running++; 									\
	}while(0)

#define blockDistToStart(count) 			(count##Running < count##Start ? count##Start - count##Running : 0)
#define blockBufferSize(count)				(count##End - count##Start)
#define blockLenInBuffer(count, size) 		(MIN((size - blockDistToStart(count)), (blockBufferSize(count))))

#define blockAddMem(buffer, count, memPtr, size) do\
	{ \
		Len = size; \
		\
		if ((count##Running + Len) >= count##Start && count##Running < count##Start)\
		{ \
			rt_memcpy(buffer##Ptr, (memPtr + blockDistToStart(count)), blockLenInBuffer(count, Len)); \
			buffer##Ptr +=  blockLenInBuffer(count, Len); \
		} else\
		if(count##Start <= count##Running && count##Running < count##End )  \
		{ \
			rt_memcpy(buffer##Ptr, memPtr, MIN(Len, (count##End - count##Running))); \
			buffer##Ptr += MIN(Len, (count##End - count##Running)); \
		} \
		count##Running += Len; \
	}while(0)


void inline blockAddMem2(uint8_t **buf, uint32_t count, uint8_t *mem, uint32_t size)
{
	uint8_t *bufPtr = *buf;

}

#define blockAddString(buffer, count, memPtr) 	blockAddMem(buffer, count, memPtr, rt_strlen(memPtr))

#define blockReturn(count) do{			\
	if(count##Running >= count##End) 	\
	{ 									\
		pkt->block.offset++; 			\
		return; 						\
	} 									\
	}while(0)


typedef struct coap_block {
	uint32_t num:20;
	uint32_t m:1;
	uint32_t szx:3;
}coap_block_t;


#define COAP_BLOCK_LAST_BYTE(opt) opt
#define COAP_ENCODE_BYTES(buffer, bytes) coap_encodeUint(buffer, bytes)
#endif /* COAP_BLOCK_H_ */
