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

typedef struct {
  int in_test;

  const char* current_group;
  const char* current_test;

  int is_failure;
  int total_failures;
} TestRunner;

void test_runner_init(TestRunner* runner);
TestRunSummary test_runner_run_all(TestRunner* runner, const TestRegistry* registry);
void run_one(
    TestRunner* runner,
    const TestGroup* group,
    const TestCase* test,
    TestRunSummary* summary
);
void test_runner_test_fail(const char* file, int line, const char* fmt, ...);

#ifdef __cplusplus
}
#endif
