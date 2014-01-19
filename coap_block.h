/*
 * coap_block.h
 *
 *  Created on: Jan 14, 2014
 *      Author: poppe
 */

#ifndef COAP_BLOCK_H_
#define COAP_BLOCK_H_

#define declareBlockVar(buffer, size, count) \
											uint8_t buffer[size+1]; 								\
											uint8_t *buffer##Ptr = buffer; 						\
											uint32_t count##Running = 0; 						\
											uint32_t count##Start = pkt->block.offset * size; 	\
											uint32_t count##End = count##Start + size

#define blockAddChar(buffer, count, char) do { 			\
	if(count##Start <= count##Running && count##Running < count##End ) 	\
	{ 													\
		*buffer##Ptr++ = char; 							\
	} 													\
	count##Running++; 									\
	}while(0)

#define blockAddMem(buffer, count, memPtr, size) do\
	{ \
		if(count##Start <= count##Running && count##Running < count##End )  \
		{ \
			rt_memcpy(buffer##Ptr, memPtr, MIN(size, (count##End - count##Running))); \
			buffer##Ptr += MIN(size, (count##End - count##Running)); \
			count##Running += size; \
		} \
	}while(0)


#define blockAddString(buffer, count, memPtr) 	blockAddMem(buffer, count, memPtr, rt_strlen(memPtr))

#define blockReturn(count) do{			\
	if(count##Running >= count##End) 	\
	{ 									\
		pkt->block.offset++; 			\
		return; 						\
	} 									\
	}while(0)


typedef struct coap_block {
	union {
		struct{
		uint32_t num:20;
		uint32_t m:1;
		uint32_t szx:3;
		}__attribute__( ( __packed__ ) )bits;
		uint32_t pkt;
	}__attribute__( ( __packed__ ) );

	uint32_t offset;
}coap_block_t;


#endif /* COAP_BLOCK_H_ */
