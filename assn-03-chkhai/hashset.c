#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0 && numBuckets > 0);
	assert(hashfn != NULL && comparefn != NULL);

	h->size = 0;
	h->num_buckets = numBuckets;
	h->size_of_elem = elemSize;
	h->comp = comparefn;
	h->hash = hashfn;
	h->free = freefn;

	int size_of_mem = sizeof(vector) * h->num_buckets;

	h->data = malloc(size_of_mem);

	assert(h->data != NULL);

	int initialAlloc = 4;

	for(int i = 0; i < h->num_buckets; i++) {
		vector* currV = &(h->data[i]);
		VectorNew(currV, h->size_of_elem, h->free, initialAlloc);
	}
}

void HashSetDispose(hashset *h)
{
	for(int i = 0; i < h->num_buckets; i++){
		vector* currV = &(h->data[i]);
		VectorDispose(currV);
	}
	free(h->data);
}

int HashSetCount(const hashset *h)
{ return h->size; }

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn != NULL);
	for(int i = 0; i < h->num_buckets; i++){
		vector* currV = &(h->data[i]);
		VectorMap(currV, mapfn, auxData);
	}
}

int detectHashCode(const hashset *h, const void *elemAddr)
{
	int hash_code = h->hash(elemAddr, h->num_buckets);
	assert(hash_code >= 0 && hash_code < h->num_buckets);
	return hash_code;
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);

	int hash_code = detectHashCode(h, elemAddr);

	vector* currV = &(h->data[hash_code]);
	int curr_position = VectorSearch(currV, elemAddr, h->comp, 0, false);

	if(curr_position != -1) VectorReplace(currV, elemAddr, curr_position);
	else{
		VectorAppend(currV, elemAddr);
		h->size++;
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
	assert(elemAddr != NULL);

	int hash_code = detectHashCode(h, elemAddr);

	vector* currV = &(h->data[hash_code]);

	int curr_position = VectorSearch(currV, elemAddr, h->comp, 0, false);

	if(curr_position == -1) return NULL;
	void* res = VectorNth(currV, curr_position);
	return res;
}
