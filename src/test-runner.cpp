#include "umbra/test-runner.hpp"
#include "umbra/test-group.hpp"
#include "umbra/test-log.hpp"
#include <source_location>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cassert>

static TestRunner* g_runner = nullptr;
using namespace Umbra::Test::Log;

void TestRunner::BeginContext(const char* groupName, const char* testName)
{
  this->currentGroup = groupName;
  this->currentTest = testName;
  this->inTest = true;
}

void TestRunner::EndContext()
{
  this->inTest = false;
}

TestRunner::TestRunner()
    : inTest(false)
    , currentGroup(nullptr)
    , currentTest(nullptr)
    , isFailure(false)
    , totalFailures(0)
{
}

void TestRunner::TestFail(const char* file, int line, const char* fmt, ...)
{
  if (!g_runner || !g_runner->inTest) {
    std::fprintf(
        stderr, LOG_COLOR_CODE_RED TAG_ERROR "%s:%d: test failure outside of running test\n", file,
        line
    );
    va_list args;
    va_start(args, fmt);
    std::vfprintf(stderr, fmt, args);
    va_end(args);
    std::fprintf(stderr, LOG_COLOR_CODE_DEFAULT "\n");
    return;
  }

  g_runner->isFailure = true;
  g_runner->totalFailures += 1;

  std::fprintf(
      stderr, "%s:%d: FAIL [%s] %s: ", file, line, CheckGroupName(g_runner->currentGroup),
      g_runner->currentTest ? g_runner->currentTest : "(unknown)"
  );
  va_list args;
  va_start(args, fmt);
  std::vfprintf(stderr, fmt, args);
  va_end(args);
  std::fprintf(stderr, "\n");
}

void TestRunner::RunOne(const TestGroup* group, const TestCase* test, TestRunSummary* summary)
{
  this->isFailure = false;

  if (group->GetBeforeEach()) {
    this->BeginContext(group->GetName(), "(before_each)");
    group->ExecuteBeforeEach();
    EndContext(this);
  }

  this->BeginContext(group->GetName(), test->GetName());
  test->Run();
  EndContext(this);

  if (group->GetAfterEach()) {
    this->BeginContext(group->GetName(), "(after_each)");
    group->ExecuteAfterEach();
    EndContext(this);
  }

  summary->total += 1;
  if (this->isFailure) {
    summary->failed += 1;
  }
  else {
    summary->passed += 1;
    TEST_LOG_PRINT(
        LogColor::Green, "PASS [%s] %s", CheckGroupName(group->GetName()), test->GetName()
    );
  }
}

TestRunSummary* RunGroup(TestGroup* group, TestRunSummary* summary)
{
  if (!group) {
    return summary;
  }

#ifdef ENABLE_DEBUG
  TEST_LOG_PRINT(
      LogColor::Yellow, "[%s] - Test Count: %zu", group->GetName(), group->GetTests().size()
  );
#else
  if (strcmp(group->GetName(), "ROOT")) {
    TEST_LOG_PRINT(LogColor::Cyan, "[%s]", group->GetName());
  }
#endif

  if (group->GetBeforeAll()) {
    this->BeginContext(group->GetName(), "(before_all)");
    group->ExecuteBeforeAll();
    EndContext();
  }

  const auto& tests = group->GetTests();
  for (size_t i = 0; i < tests.size(); ++i) {
    this->RunOne(group, &tests[i], summary);
  }

  for (size_t i = 0; i < group->GetChildCount(); ++i) {
    this->RunGroup(group->GetChild(i), summary);
    std::printf("\n");
  }

  if (group->GetAfterAll()) {
    this->BeginContext(group->GetName(), "(after_all)");
    group->ExecuteAfterAll();
    EndContext();
  }

  return summary;
}

TestRunSummary TestRunner::RunAll(const TestRegistry* registry)
{
  g_runner = this;

  if (!g_runner) {
    TEST_LOG_ERROR("Runner was unable to be set");
    std::abort();
  }

  if (!registry || !registry->GetRootGroup()) {
    TEST_LOG_ERROR("root TestGroup was unable to be set on default registry");
    std::abort();
  }

  TestRunSummary summary = {0};

  this->RunGroup(registry->GetRootGroup(), &summary);

  g_runner = nullptr;

  std::printf(
      LOG_COLOR_CODE_WHITE "\nSummary: " LOG_COLOR_CODE_YELLOW "%d total" LOG_COLOR_CODE_DEFAULT
                           ", " LOG_COLOR_CODE_GREEN "%d passed" LOG_COLOR_CODE_DEFAULT
                           ", " LOG_COLOR_CODE_RED "%d failed\n\n" LOG_COLOR_CODE_DEFAULT,
      summary.total, summary.passed, summary.failed
  );

  return summary;
}
