#include "umbra/test_container.h"

void test_container_init(TestContainer* array){
  array->data = NULL;
  array->count = 0;
  array->capacity = 0;
}

void* test_container_grow(void* ptr, size_t element_size, size_t* capacity){
  size_t new_capacity = (*capacity == 0) ? 8 : (*capacity * 2);
  void* p = realloc(ptr, new_capacity * element_size);
  if(!p) abort();
  *capacity = new_capacity;
  return p;
}

void test_container_push(TestContainer* array, void* item){
  if(array->count >= array->capacity){
    size_t new_capacity = array->capacity == 0 ? 8 : array->capacity * 2;
    void** new_data = realloc(array->data, new_capacity * sizeof(void*));
    if(!new_data){
      // Side Quest: Handle out of memory
      abort();
    }
    array->data = new_data;
    array->capacity = new_capacity;
  }
  array->data[array->count++] = item;
}

void test_container_free(TestContainer* array){
  free(array->data);
  array->data = NULL;
  array->count = 0;
  array->capacity = 0;
}
