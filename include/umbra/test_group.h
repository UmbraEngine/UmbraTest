#pragma once
#include <stdlib.h>
#include "test_case.h"
#include "test_container.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TestHookFn)(void* user);

typedef struct TestGroup TestGroup;

typedef struct TestGroupNode {
  TestGroup* group;
  struct TestGroupNode* next;
} TestGroupNode;

struct TestGroup {
  const char* name;
  TestContainer tests;
  size_t count;
  size_t capacity;

  TestGroup* parent;
  TestGroupNode* head;
  TestGroupNode* tail;
  int length;

  TestHookFn before_all;
  TestHookFn after_all;
  TestHookFn before_each;
  TestHookFn after_each;
  void* hook_user;
  void (*hook_teardown)(void*);
};

const char* check_group_name(const char* group_name);

#ifdef __cplusplus
}
#endif
