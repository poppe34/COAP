/*
 * coap.h
 *
 *  Created on: Aug 1, 2013
 *      Author: poppe
 */

#ifndef COAP_H_
#define COAP_H_

#include <stdint.h>
#include "lwip/def.h"
#include "coap_block.h"
#include "coap_msg.h"
#include "coap_err.h"

#include "rtthread.h"
#include "lwip/debug.h"
#include "sll.h"

#define COAP_VER_MAJOR 0
#define COAP_VER_MINOR 1


#define COAP_DEFAULT_PORT 5683
#define COAP_MIN_PKT_SIZE 4

#define COAP_BLOCK_SIZE	64

#define NSTART 				1
#define MAX_RETRANSMIT		4
/* Time in ms */
#define ACK_TIMEOUT			2000
#define ACK_RANDOM_FACTOR 	1500
#define DEFAULT_LEASURE		5000
#define PROBING_RATE		1 //Bytes per second
#ifndef COAP_DEBUG
#define COAP_DEBUG	LWIP_DBG_ON
#endif

#define COAP_TRUE	-1
#define COAP_FALSE	 0

#ifndef size_t
#define size_t int
#endif
#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#define coap_malloc rt_malloc
#define coap_free	rt_free

#define COAP_CODE(num) (((num / 100) << 5) | ((num % 100) & 0x1f))

#define COAP_CON 0
#define COAP_NON 1
#define COAP_ACK 2
#define COAP_RST 3

#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif /* MIN */

extern uint8_t proxy_on;

/**
 * coap_os.c
 */
void coapd_init(struct ip_addr *local_addr, int portNum);

/**
 * coap_client
 */
void coap_reply(coap_pkt_t *pkt, uint8_t *data, size_t len, uint8_t code);

/**
 * coap_parse.c
 */
uint32_t coap_Hash(const char *str);
uint32_t coap_runningHash(const char *str, uint32_t runHash);

/**
 * coap_net
 */
coap_err_t coapd_sendto(uint8_t *buf, uint32_t len, ip_addr_t *addr, u16_t port);

/**
 * coap_encode.c
 */
uint8_t *coap_encodeOptions(coap_pkt_t *pkt, uint8_t *buf, uint32_t size);

/**
 * coap_option.c
 */
coap_option_t *coap_findOption(coap_pkt_t *pkt, uint16_t num);
void coap_addOption(coap_pkt_t *pkt, uint8_t optNum, uint8_t *data, uint32_t len, uint8_t optFormat);

/**
 * coap_resources.c
 */
coap_resource_t *coap_resourceRegister(const char *URI);
void coap_resourceRegisterCB(coap_resource_t *rSrc, coap_resourceCB callback, coap_methods_t method );
coap_err_t coap_resourceDiscovery(coap_pkt_t *pkt, coap_resource_t **foundRes);
#endif /* COAP_H_ */
