#pragma once
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include "test-case.hpp"

class TestGroup {
public:
    using TestHookFn = std::function<void(void*)>;

    TestGroup(const char* name, TestGroup* parent = nullptr)
        : name_(name), parent_(parent), hookUser_(nullptr), hookTeardown_(nullptr) {}

    const char* GetName() const { return this->name_.c_str(); }
    
    std::vector<TestCase>& GetTests() { return this->tests_; }
    const std::vector<TestCase>& GetTests() const { return this->tests_; }
    
    TestGroup* GetParent() { return this->parent_; }
    const TestGroup* GetParent() const { return this->parent_; }
    
    void AddChildGroup(std::unique_ptr<TestGroup> child) {
        this->childGroups_.push_back(std::move(child));
    }
    
    size_t GetChildCount() const { return this->childGroups_.size(); }
    
    TestGroup* GetChild(size_t index) {
        return index < this->childGroups_.size() ? this->childGroups_[index].get() : nullptr;
    }
    
    void SetBeforeAll(TestHookFn hook) { this->beforeAll_ = hook; }
    void SetAfterAll(TestHookFn hook) { this->afterAll_ = hook; }
    void SetBeforeEach(TestHookFn hook) { this->beforeEach_ = hook; }
    void SetAfterEach(TestHookFn hook) { this->afterEach_ = hook; }
    
    void SetHookUser(void* user, void (*teardown)(void*) = nullptr) {
        this->hookUser_ = user;
        this->hookTeardown_ = teardown;
    }
    
    void ExecuteBeforeAll() const { if (this->beforeAll_) this->beforeAll_(this->hookUser_); }
    void ExecuteAfterAll() const { if (this->afterAll_) this->afterAll_(this->hookUser_); }
    void ExecuteBeforeEach() const { if (this->beforeEach_) this->beforeEach_(this->hookUser_); }
    void ExecuteAfterEach() const { if (this->afterEach_) this->afterEach_(this->hookUser_); }
    
    ~TestGroup() {
        if (this->hookTeardown_ && this->hookUser_) {
            this->hookTeardown_(this->hookUser_);
        }
    }

private:
    std::string name_;
    std::vector<TestCase> tests_;
    TestGroup* parent_;
    std::vector<std::unique_ptr<TestGroup>> childGroups_;
    
    TestHookFn beforeAll_;
    TestHookFn afterAll_;
    TestHookFn beforeEach_;
    TestHookFn afterEach_;
    
    void* hookUser_;
    void (*hookTeardown_)(void*);
};

const char* CheckGroupName(const char* groupName);
