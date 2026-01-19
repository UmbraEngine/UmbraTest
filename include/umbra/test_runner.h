#pragma once
#include "test_registry.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int total;
  int passed;
  int failed;
} TestRunSummary;

typedef struct TestRunner TestRunner;

void test_runner_init(TestRunner* runner);
TestRunSummary test_runner_run_all(TestRunner* runner, const TestRegistry* registry);
void run_one(TestRunner* runner, const char* group_name, const TestCase* test, TestRunSummary* summary);
void test_fail(const char* file, int line, const char* fmt, ...);

#ifdef __cplusplus
}
#endif
