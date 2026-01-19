#include "umbra/test_runner.h"
#include "umbra/test_group.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

struct TestRunner {
  int in_test;

  const char* current_group;
  const char* current_test;

  int current_failures;
  int total_failures;
};

static TestRunner* g_runner = NULL;
static char* current_group = NULL;

void test_runner_init(TestRunner* registry)
{
  memset(registry, 0, sizeof(*registry));
}

void test_fail(const char* file, int line, const char* fmt, ...)
{
  if (!g_runner || !g_runner->in_test) {
    fprintf(stderr, "%s:%d: test failure outside of running test\n", file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    return;
  }

  g_runner->current_failures += 1;
  g_runner->total_failures += 1;

  fprintf(
      stderr, "%s:%d: FAIL [%s] %s: ", file, line, check_group_name(g_runner->current_group),
      g_runner->current_test ? g_runner->current_test : "(unknown)"
  );
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}

void run_one(
    TestRunner* runner,
    const char* group_name,
    const TestCase* test,
    TestRunSummary* summary
)
{
  runner->current_group = group_name;
  runner->current_test = test->name;
  runner->current_failures = 0;

  runner->in_test = 1;
  test->fn(test->user);
  runner->in_test = 0;

  summary->total += 1;
  // TODO: I think this might be a bug. Will have to test the behaviour when running the tests
  // if a single test has failed, will all subsequent tests fail because of this check?
  if (runner->current_failures > 0) {
    summary->failed += 1;
    fprintf(
        stderr, "FAILED [%s] %s (%d failures)\n", check_group_name(group_name), test->name,
        runner->current_failures
    );
  }
  else {
    summary->passed += 1;
    printf("PASS [%s] %s\n", check_group_name(group_name), test->name);
  }
}

TestRunSummary test_runner_run_all(TestRunner* runner, const TestRegistry* registry)
{
  TestRunSummary summary = {0, 0, 0};
  g_runner = runner;

  for (size_t gi = 0; gi < registry->count; ++gi) {
    const TestGroup* group = &registry->groups[gi];
    printf("\n[%s]\n", group->name);
    for (size_t ti = 0; ti < group->count; ++ti) {
      run_one(runner, group->name, &group->tests[ti], &summary);
    }
  }

  g_runner = NULL;

  printf(
      "\nSummary: %d total, %d passed, %d failed\n", summary.total, summary.passed, summary.failed
  );
  return summary;
}

void set_current_group(const char* group)
{
  free(current_group);
  current_group = strdup(group);
}

void clear_current_group(void)
{
  free(current_group);
  current_group = NULL;
}

