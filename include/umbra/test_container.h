#pragma once

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TestContainer {
  void** data;
  size_t count;
  size_t capacity;
} TestContainer;

void test_container_init(TestContainer* array);
void* test_container_grow(void* ptr, size_t element_size, size_t* capacity);
void test_container_push(TestContainer* self, void* item);
void test_container_free(TestContainer* self);



#ifdef __cplusplus
}
#endif
