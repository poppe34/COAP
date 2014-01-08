/*
 * single_linked_list.h
 *
 *  Created on: Dec 10, 2013
 *      Author: poppe
 */

#ifndef SINGLE_LINKED_LIST_H_
#define SINGLE_LINKED_LIST_H_

#define SLL_CONCAT2(s1, s2) s1##s2
#define SLL_CONCAT(s1, s2) SLL_CONCAT2(s1, s2)

#define SLL_NEXT(data) data = sll_next(data)

#define SLL(name) \
         static struct sll_list SLL_CONCAT(name,_list) = {NULL, 0}; \
         static sll_list_t name = (sll_list_t)&SLL_CONCAT(name,_list)
#define SLL_MALLOC(name) \
		struct sll_list *SLL_CONCAT(name, _list) = (struct sll_list *)malloc(sizeof(struct sll_list)); \
		SLL_CONCAT(name, _list)->first = NULL; \
		SLL_CONCAT(name, _list)->cnt = 0; \
		sll_list_t name = (sll_list_t)&SLL_CONCAT(name,_list)

typedef struct sll_node *sll_node_t;
typedef struct sll_list *sll_list_t;

struct sll_node {
	void *		next;
};
struct sll_list {
	void *		first;
	uint8_t		cnt;
};

/**
 * Defines
 */



/**
 * Inlines
 */

//		 void * sll_nextData __attribute__( ( __inline__ ) )(sll_node_t *n)
//		 {
//	 	 	 void *data = n->payload;
//
//		 }
/**
 * Prototypes
 */
void sll_init(sll_list_t list);
sll_node_t sll_findNodeWithData(sll_list_t list, void *data);
int sll_addData(sll_list_t list, void *data);
int sll_removeData(sll_list_t list, void *data);
void sll_insertAfter(sll_list_t list, void *old, void *new);
void sll_forEachDataWithArg(sll_list_t list, void (*CB)(void *data, void *arg), void *arg);
void sll_forEachDataWithDualArg(sll_list_t list, void (*CB)(void *data, void *arg1, void *arg2), void *arg1, void *arg2);
void *sll_searchWithin(sll_list_t list, int (*CB)(void *data, void *arg), void *arg);
void *sll_next(void *n);

#endif /* SINGLE_LINKED_LIST_H_ */
