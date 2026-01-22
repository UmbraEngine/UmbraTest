#pragma once

#include <stdlib.h>
#include <string.h>
#include "test_case.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TestContainer {
  TestCase* data;
  size_t count;
  size_t capacity;
} TestContainer;

void test_container_init(TestContainer* test_container);
void test_container_grow(TestContainer* container, size_t new_capacity);
void test_container_push(TestContainer* self, TestCase item);
void test_container_free(TestContainer* self);

#ifdef __cplusplus
}
#endif
