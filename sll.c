/*
 * single_linked_list.c
 *
 *  Created on: Dec 10, 2013
 *      Author: poppe
 */

#include "coap.h"
#include "sll.h"


void sll_init(sll_list_t list)
{
	list->first = NULL;
	list->cnt = 0;
}

int sll_addData(sll_list_t list, void *data)
{
	RT_ASSERT(list);
	sll_node_t n;

	n = (sll_node_t)list->first;

	if(n)
	{
		while(n->next)
		{
			n = n->next;
		}
		n->next = data;
		return 0;
	}

	list->first = data;
	return 0;

}
int sll_belongs(sll_list_t list, void *node)
{
	RT_ASSERT(list);

	sll_node_t n = *((sll_node_t *)list);

	for(;n != NULL; n=n->next)
	{
		if(n == node)
			return -1;
	}

	return 0;
}

void sll_insertAfter(sll_list_t list, void *old, void *new)
{
	RT_ASSERT(list);

	sll_removeData(list, new);

	if(sll_belongs(list, old))
	{
		((sll_node_t)new)->next = ((sll_node_t)old)->next;
		((sll_node_t)old)->next = new;
	}
}

int sll_removeData(sll_list_t list, void *data)
{
	RT_ASSERT(list);

	sll_node_t n = sll_findNodeWithData(list, data), p, c;

	p = NULL;
	c = list->first;

	while(c)
	{
		if(c == n)
		{
			if (!p)
				list->first = c->next;
			else
				p->next = c->next;
			coap_free(n);
			return 0;
		}
		p = c;
		c = c->next;
	}

	return -1;
}



sll_node_t sll_findNodeWithData(sll_list_t list, void *data)
{
	RT_ASSERT(list);

	sll_node_t n = list->first;

	while(n)
	{
		if(n == data)
			return n;

		n = n->next;
	}
	return NULL;
}


void sll_forEachDataWithArg(sll_list_t list, void (*CB)(void *data, void *arg), void *arg)
{
	RT_ASSERT(list);
	RT_ASSERT(CB);

	sll_node_t n = list->first;

	while(n)
	{
		if(CB)
			CB(n, arg);
		n = n->next;
	}
}

void sll_forEachDataWithDualArg(sll_list_t list, void (*CB)(void *data, void *arg1, void *arg2), void *arg1, void *arg2)
{
	RT_ASSERT(list);
	RT_ASSERT(CB);

	sll_node_t n = list->first;

	while(n)
	{
		if(CB)
			CB(n, arg1, arg2);
		n = n->next;
	}
}

void *sll_searchWithin(sll_list_t list, int (*CB)(void *data, void *arg), void *arg)
{
	RT_ASSERT(list);
	RT_ASSERT(CB);
	sll_node_t n = list->first;

	while(n)
	{
		if(CB)
			if(CB(n, arg))
				return n;
		n = n->next;
	}

	return NULL;
}

void *sll_next(void *n)
{
	if(n == NULL)
		return NULL;
	return ((sll_node_t)n)->next;
}
