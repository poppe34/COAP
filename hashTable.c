/*
 * hashTable.c
 *
 *  Created on: Jul 31, 2013
 *      Author: poppe
 */
/* OS headers */
//#include "arch.h"

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/ip_addr.h"
#include "lwip/tcp.h"

#include "coap.h"

typedef struct pair Pair;
typedef struct htMap Map;
typedef unsigned long Hash;

struct pair
{
	Hash hash;
	char *key;
	void *value;
	Pair *next;
};

struct htMap
{
	int cnt;
	Pair *pairs;
};

static Hash ht_hash(const char *str);

Map* ht_newMap(void)
{
	Map *map;
	map = (Map *)mem_malloc(sizeof(Map));

	LWIP_ASSERT(("hashtable: Out of Memory"), map);

	return map;
}

void ht_delMap(Map *map)
{
	Pair *pair = map->pairs;
	int map_cnt = map->cnt;

	while(map_cnt)
	{
	/* cycle thru the pairs until the end */
		for(int x = 0; x<map_cnt; x++)
			pair = pair->next;

		mem_free(pair);
		pair = map->pairs;
		--map_cnt;
	}

	mem_free(map);
}

Pair *ht_add(Map *map, const char *key, void *value)
{
	Pair *pair;

	for(int x = 0; x<map->cnt; x++)
		pair = pair->next;

	pair->next = (Pair *)mem_malloc(sizeof(Pair));
	pair = pair->next;
	if(pair)
	{
		pair->hash = ht_hash(key);
		pair->key = key;
		pair->value = value;

		map->cnt++;
	}
	else
	{
		LWIP_DEBUGF(COAP_DEBUG, ("hashTable: Out of Memory\n"));
	}
	return pair;
}


Pair *ht_remove(Map *map, const char *key)
{
	LWIP_ASSERT(("hashTable: Invalid Map\n"), map);
	LWIP_ASSERT(("hashTable: Invalid Key\n"), key);

	Pair *pair = map->pairs, *old;
	Hash hash = ht_hash(key);

	if(pair->hash == hash)
	{
		map->pairs = pair->next;
		map->cnt--;

		mem_free(pair);
		return pair;
	}
	else
	{
		old = pair;
		pair = pair->next;

		while(pair)
		{
			if(pair->hash == hash)
			{
				old->next = pair->next;
				mem_free(pair);
				return(pair);
			}
			old = pair;
			pair = pair->next;
		}
	}

	return NULL;
}
/*
 *
 */
static Hash ht_hash(const char *str)
{
	unsigned long hash = 5381;
	char c;

	while (c = *str++) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}
