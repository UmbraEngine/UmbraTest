#include "umbra/test_container.h"
#include <stdio.h>

void test_container_init(TestContainer* test_container)
{
  test_container->data = NULL;
  test_container->count = 0;
  test_container->capacity = 0;
}

void test_container_grow(TestContainer* container, size_t new_capacity)
{
  if (new_capacity <= container->capacity) {
    return;
  }

  TestCase* new_data = (TestCase*)realloc(container->data, new_capacity * sizeof(TestCase));
  if (!new_data) {
    fprintf(stderr, "UmbraTest: failed to grow TestContainer\n");
    abort();
  }
  container->data = new_data;
  container->capacity = new_capacity;
}

void test_container_push(TestContainer* test_container, TestCase item)
{
  if (test_container->count >= test_container->capacity) {
    size_t new_capacity = test_container->capacity == 0 ? 8 : test_container->capacity * 2;
    test_container_grow(test_container, new_capacity);
  }
  test_container->data[test_container->count++] = item;
}

void test_container_free(TestContainer* test_container)
{
  free(test_container->data);
  test_container->data = NULL;
  test_container->count = 0;
  test_container->capacity = 0;
}
