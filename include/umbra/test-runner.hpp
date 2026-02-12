#pragma once
#include "test-registry.hpp"
#include <cstddef>

struct TestRunSummary {
  int total;
  int passed;
  int failed;
};

class TestRunner {
public:
  TestRunner();
  ~TestRunner() = default;

  void RunOne(const TestGroup* group, const TestCase* test, TestRunSummary* summary);
  TestRunSummary* RunGroup(TestGroup* group, TestRunSummary* summary);
  TestRunSummary RunAll(const TestRegistry* registry);

  void TestFail(const char* file, int line, const char* fmt, ...);
  void BeginContext(const char* groupName, const char* testName);
  void EndContext();

  bool IsInTest() const { return this->inTest; }
  const char* GetCurrentGroup() const { return this->currentGroup; }
  const char* GetCurrentTest() const { return this->currentTest; }
  bool IsFailure() const { return this->isFailure; }
  int GetTotalFailures() const { return this->totalFailures; }

private:

  bool inTest;
  const char* currentGroup;
  const char* currentTest;
  bool isFailure;
  int totalFailures;
};
