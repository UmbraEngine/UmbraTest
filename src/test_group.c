#include "umbra/test_group.h"

static char* DEFAULT_GROUP_NAME = "Ungrouped";

const char* check_group_name(const char* group_name)
{
  return group_name ? group_name : DEFAULT_GROUP_NAME;
}
