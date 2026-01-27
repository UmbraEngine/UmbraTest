#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include "test_assertions.hpp"
extern "C" {
#include "test_runner.h"
#include "test_registry.h"
#include "test_case.h"
#include "test_group.h"
}

/* ##################################### */
/* ========= MACRO HELPERS: ============ */
/* ##################################### */

#define MACRO_CAT2(a, b) a##b
#define MACRO_CAT(a, b) MACRO_CAT2(a, b)

#if __cplusplus >= 201703L
#define UMBRA_MAYBE_UNUSED [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
#define UMBRA_MAYBE_UNUSED __attribute__((unused))
#else
#define UMBRA_MAYBE_UNUSED
#endif

#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0]))

/* ################################# */
/* ========= DESCRIBE: ============ */
/* ################################# */

inline TestGroup* _test_group = nullptr;

#define DESCRIBE(group_name, BODY)                                                                 \
  UMBRA_MAYBE_UNUSED static const bool MACRO_CAT(_test_framework_desc_reg_, __COUNTER__) = []() {  \
    TestRegistry* _test_registry = test_registry_get_default_registry();                           \
    TestGroup* _parent = _test_group != nullptr ? _test_group : _test_registry->root;              \
    TestGroup* _child = test_registry_get_child_group(_test_registry, _parent, (group_name));      \
    TestGroup* _saved = _test_group;                                                               \
    _test_group = _child;                                                                          \
    do                                                                                             \
      BODY while (0);                                                                              \
    _test_group = _saved;                                                                          \
    return true;                                                                                   \
  }();

/* ################################# */
/* ========= HOOKS: ============ */
/* ################################# */
// NOTE: Must be used inside Describe body
#define BEFORE_EACH(BODY)                                                                          \
  do {                                                                                             \
    test_registry_set_before_each(_test_group, +[](void* user) BODY, NULL, NULL);                  \
  } while (0)

#define AFTER_EACH(BODY)                                                                           \
  do {                                                                                             \
    test_registry_set_after_each(_test_group, +[](void* user) BODY, NULL, NULL);                   \
  } while (0)

#define BEFORE_ALL(BODY)                                                                           \
  do {                                                                                             \
    test_registry_set_before_all(_test_group, +[](void* user) BODY, NULL, NULL);                   \
  } while (0)

#define AFTER_ALL(BODY)                                                                            \
  do {                                                                                             \
    test_registry_set_after_all(_test_group, +[](void* user) BODY, NULL, NULL);                    \
  } while (0)

/* ################################# */
/* =========== TESTS: ============== */
/* ################################# */

#define TEST(testName, ...)                                                                        \
  UMBRA_MAYBE_UNUSED static const bool MACRO_CAT(_test_framework_test_body_reg_, __COUNTER__) =    \
      [=]() {                                                                                      \
        TestRegistry* _registry = test_registry_get_default_registry();                            \
        TestGroup* _group = _test_group ? _test_group : _registry->root;                           \
        test_registry_register_test(                                                               \
            _registry, _group, (testName),                                                         \
            +[](void* user) {                                                                      \
              (void)user;                                                                          \
              __VA_ARGS__                                                                          \
            },                                                                                     \
            NULL, NULL                                                                             \
        );                                                                                         \
        return true;                                                                               \
      }();

#define TEST_FN(testName, FN)                                                                      \
  UMBRA_MAYBE_UNUSED static const bool MACRO_CAT(_test_framework_test_fn_reg_, __COUNTER__) =      \
      [=]() {                                                                                      \
        test_registry_register_test(_test_registry, _test_group, (testName), (FN), NULL, NULL);    \
        return true;                                                                               \
      }();

#define IT(testName, BODY) TEST(testName, BODY)
#define IT_FN(testName, FN) TEST_FN(testName, FN)

/* ################################# */
/* ========= ASSERTIONS: ============ */
/* ################################# */
#define ASSERT_TRUE(cond)                                                                          \
  do {                                                                                             \
    if (!(cond))                                                                                   \
      test_runner_test_fail(__FILE__, __LINE__, "ASSERT_TRUE failed: %s", #cond);                  \
  } while (0)

#define ASSERT_EQUAL(a, b)                                                                         \
  do {                                                                                             \
    ::Umbra::Test::Assertions::assert_equal((a), (b), __FILE__, __LINE__);                         \
  } while (0)

#define REQUIRE_TRUE(cond)                                                                         \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      test_runner_test_fail(__FILE__, __LINE__, "REQUIRE_TRUE failed: %s", #cond);                 \
      return;                                                                                      \
    }                                                                                              \
  } while (0)
