#include "umbra/test_cpp.hpp"
#include <umbra/test.h>

DESCRIBE("Example Test", {
  DESCRIBE("Math", { TEST("Add", { ASSERT_EQUAL_INT(1, 1); }); });
  DESCRIBE("Arrays", {
    TEST("Compare", {
      int arr1[] = {1, 2};
      int arr2[] = {1, 3, 2};

      ASSERT_ARRAY_EQUAL_INT(arr1, arr2);
    });
  });
});
