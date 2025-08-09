#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    //done
    assert(initialAllocation >= 0);
    assert(elemSize > 0);

    v->log_length = 0;
    v->size_of_elem = elemSize;
    v->vec_free = freeFn;
    if (initialAllocation == 0) initialAllocation = 4;
    v->alloc_length = initialAllocation;
    int size_of_mem = elemSize * initialAllocation;
    v->data = malloc(size_of_mem);

    assert(v->data != NULL);
}

void VectorDispose(vector *v)
{
    //done
    if(v->vec_free != NULL){
        for(int i = 0; i < v->log_length; i++){
           void* elem_to_delete = VectorNth(v, i); 
           v->vec_free(elem_to_delete); 
        }
    }
    free(v->data);
}

//done
int VectorLength(const vector *v)
{ return v->log_length; }

void *VectorNth(const vector *v, int position)
{ 
    //done
    assert(position >= 0);
    assert(position < v->log_length);

    void* res = (char *) v->data + position * v->size_of_elem;

    return res;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
    //done
    assert(position >= 0);
    assert(position < v->log_length);

    void* pos = VectorNth(v, position);
    if(v->vec_free != NULL) v->vec_free(pos);
    memcpy(pos, elemAddr, v->size_of_elem);
}

void grow_check(vector *v)
{      
    //done
    if(v->alloc_length > v->log_length) return;
    
    v->alloc_length *= 2;
    int size_of_mem = v->alloc_length * v->size_of_elem; 
    v->data = realloc(v->data, size_of_mem);
    assert(v->data != NULL);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
    //done
    assert(position >= 0);
    assert(position <= v->log_length);

    grow_check(v);
    void* pos = (char*)v->data + v->size_of_elem * position;
    int elems_to_shift = v->size_of_elem*(v->log_length - position);
    void* new_pos = (char*)pos + v->size_of_elem; // new position if elems are supposed to shift
    if(elems_to_shift > 0) memmove(new_pos, pos, elems_to_shift);
    memcpy(pos, elemAddr, v->size_of_elem);
    v->log_length += 1;
}

void VectorAppend(vector *v, const void *elemAddr)
{
    //done
    grow_check(v);
    void* pos = (char*)v->data + v->log_length*v->size_of_elem;
    memcpy(pos, elemAddr, v->size_of_elem);

    v->log_length+=1;
}

void VectorDelete(vector *v, int position)
{
    //done
    assert(position >= 0);
    assert(position < v->log_length);

    void* pos = VectorNth(v, position);
    if(v->vec_free != NULL) v->vec_free(pos);
    int shift = (v->log_length - position) * v->size_of_elem;
    if(shift > 0) memmove(pos, (char*)v->data + (position + 1) * v->size_of_elem, shift);
    v->log_length--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
    //done
    assert(compare != NULL);
    qsort(v->data, v->log_length, v->size_of_elem, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
    //done
    assert(mapFn != NULL);
    for(int i = 0; i < v->log_length; i++){
        void* elem_ptr = VectorNth(v, i);
        mapFn(elem_ptr, auxData);
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
    //done
    assert(key != NULL);
    assert(searchFn != NULL);
    assert(startIndex <= v->log_length);
    assert(startIndex>=0);

    void* start_ptr = (void*)((char*)v->data + v->size_of_elem*startIndex);
    void* res = NULL;

    size_t lng = (size_t)(v->log_length);
    if(!isSorted) res = lfind(key, start_ptr, &lng, (size_t)(v->size_of_elem), searchFn);
    else res = bsearch(key, start_ptr, (size_t)v->log_length, (size_t)v->size_of_elem, searchFn);
    
    if(res == NULL) return kNotFound;

    int pos = ((char*)res - (char*)v->data)/v->size_of_elem;
    return pos;  
} 
