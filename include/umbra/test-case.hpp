#pragma once

#include <functional>

class TestCase {
public:
  using TestCaseFn = std::function<void(void*)>;
  using TestTeardownFn = std::function<void(void*)>;

  TestCase(const char* name, TestCaseFn fn, void* user = nullptr, TestTeardownFn teardown = nullptr)
      : name_(name)
      , fn_(fn)
      , user_(user)
      , teardown_(teardown)
  {
  }

  void Run() const
  {
    if (fn_) {
      fn_(user_);
    }
  }

  void Teardown() const
  {
    if (teardown_) {
      teardown_(user_);
    }
  }

  const char* GetName() const { return name_; }

private:
  const char* name_;
  TestCaseFn fn_;
  void* user_;
  TestTeardownFn teardown_;
};
