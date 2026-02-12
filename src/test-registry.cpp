#include "umbra/test-registry.hpp"
#include "umbra/test-case.hpp"
#include "umbra/test-group.hpp"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

static TestGroup* FindGroup(TestGroup* parent, const char* groupName)
{
    for (size_t i = 0; i < parent->GetChildCount(); ++i) {
        TestGroup* child = parent->GetChild(i);
        if (strcmp(child->GetName(), groupName) == 0) {
            return child;
        }
    }
    return nullptr;
}

TestGroup* TestRegistry::AddChildToGroup(TestGroup* parent, const char* groupName)
{
    auto group = std::make_unique<TestGroup>(this->StoreString(groupName), parent);
    TestGroup* groupPtr = group.get();
    parent->AddChildGroup(std::move(group));
    return groupPtr;
}

TestRegistry::TestRegistry()
    : root(std::make_unique<TestGroup>(this->StoreString("ROOT"), nullptr))
{
}

TestRegistry* TestRegistry::GetDefaultRegistry()
{
    static TestRegistry registry;
    return &registry;
}

char* TestRegistry::StoreString(const char* str)
{
    size_t len = strlen(str) + 1;
    auto copy = std::make_unique<char[]>(len);
    memcpy(copy.get(), str, len);
    
    char* ptr = copy.get();
    this->stringPool[std::string(str)] = std::move(copy);
    return ptr;
}

TestGroup* TestRegistry::FindOrCreateGroup(const char* groupName)
{
    return this->GetChildGroup(this->root.get(), groupName);
}

TestGroup* TestRegistry::GetChildGroup(TestGroup* parent, const char* groupName)
{
    TestGroup* existing = FindGroup(parent, groupName);
    if (existing)
        return existing;
    return AddChildToGroup(parent, groupName);
}

void TestRegistry::RegisterTest(
    TestGroup* group,
    const char* testName,
    TestCase::TestCaseFn fn,
    void* user,
    TestCase::TestTeardownFn teardownFn
)
{
    if (!group) {
        std::fprintf(stderr, "UmbraTest: attempted to register test '%s' with NULL group\n", testName);
        std::abort();
    }

    TestCase test(this->StoreString(testName), fn, user, teardownFn);
    group->GetTests().push_back(std::move(test));

#ifdef ENABLE_DEBUG
    std::fprintf(
        stderr, "REGISTER TEST: '%s' into group=%p (%s)\n", testName, 
        static_cast<void*>(group), group ? group->GetName() : "NULL"
    );
#endif
}

static void SetHook(
    TestGroup* group,
    void (TestGroup::*setter)(TestGroup::TestHookFn),
    TestGroup::TestHookFn fn,
    void* hookUser,
    void (*hookTeardown)(void*)
)
{
    (group->*setter)(fn);
    group->SetHookUser(hookUser, hookTeardown);
}

void TestRegistry::SetBeforeEach(
    TestGroup* group,
    TestGroup::TestHookFn fn,
    void* hookUser,
    void (*hookTeardown)(void*)
)
{
    SetHook(group, &TestGroup::SetBeforeEach, fn, hookUser, hookTeardown);
}

void TestRegistry::SetAfterEach(
    TestGroup* group,
    TestGroup::TestHookFn fn,
    void* hookUser,
    void (*hookTeardown)(void*)
)
{
    SetHook(group, &TestGroup::SetAfterEach, fn, hookUser, hookTeardown);
}

void TestRegistry::SetBeforeAll(
    TestGroup* group,
    TestGroup::TestHookFn fn,
    void* hookUser,
    void (*hookTeardown)(void*)
)
{
    SetHook(group, &TestGroup::SetBeforeAll, fn, hookUser, hookTeardown);
}

void TestRegistry::SetAfterAll(
    TestGroup* group,
    TestGroup::TestHookFn fn,
    void* hookUser,
    void (*hookTeardown)(void*)
)
{
    SetHook(group, &TestGroup::SetAfterAll, fn, hookUser, hookTeardown);
}
