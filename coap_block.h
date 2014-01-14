/*
 * coap_block.h
 *
 *  Created on: Jan 14, 2014
 *      Author: poppe
 */

#ifndef COAP_BLOCK_H_
#define COAP_BLOCK_H_




typedef struct coap_block {
	uint32_t num:20;
	uint32_t m:1;
	uint32_t szx:3;
};


#endif /* COAP_BLOCK_H_ */
