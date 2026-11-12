// Copyright 2026 Dennis Lanov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Regression test for https://github.com/ros2/rosidl_dynamic_typesupport_fastrtps/issues/17
//
// fastdds__dynamic_type_init_from_dynamic_type_builder() used to delete the type handle it
// had just stored in type_impl->handle, because it never cancelled its scope-exit guard. The
// returned dynamic type was therefore backed by freed memory, and any use of it afterwards
// (get_member_count / get_name, or initializing dynamic data from it) read freed memory and
// crashed. This test builds a type, uses it after init returns, and builds dynamic data from
// it, so the use-after-free is exercised. Run under AddressSanitizer for deterministic
// detection (both this test and the library must be instrumented).

#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include <rcutils/allocator.h>
#include <rcutils/types/rcutils_ret.h>

#include <rosidl_dynamic_typesupport/api/dynamic_data.h>
#include <rosidl_dynamic_typesupport/api/dynamic_type.h>
#include <rosidl_dynamic_typesupport/api/serialization_support.h>

#include "rosidl_dynamic_typesupport_fastrtps/serialization_support.h"

TEST(TestDynamicTypeInit, init_from_builder_returns_usable_type)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  // 1. Initialize Fast DDS serialization support.
  rosidl_dynamic_typesupport_serialization_support_impl_t impl{};
  rosidl_dynamic_typesupport_serialization_support_interface_t methods{};
  ASSERT_EQ(
    RCUTILS_RET_OK,
    rosidl_dynamic_typesupport_fastdds_init_serialization_support_impl(&allocator, &impl));
  ASSERT_EQ(
    RCUTILS_RET_OK,
    rosidl_dynamic_typesupport_fastdds_init_serialization_support_interface(&allocator, &methods));

  rosidl_dynamic_typesupport_serialization_support_t serialization_support =
    rosidl_dynamic_typesupport_get_zero_initialized_serialization_support();
  ASSERT_EQ(
    RCUTILS_RET_OK,
    rosidl_dynamic_typesupport_serialization_support_init(
      &impl, &methods, &allocator, &serialization_support));

  // 2. Construct a valid dynamic type with a single int32 member (mirrors std_msgs/msg/Int32).
  const char * type_name = "std_msgs/msg/Int32";
  const char * member_name = "data";
  const rosidl_dynamic_typesupport_member_id_t member_id = 0;

  rosidl_dynamic_typesupport_dynamic_type_builder_t builder =
    rosidl_dynamic_typesupport_get_zero_initialized_dynamic_type_builder();
  ASSERT_EQ(
    RCUTILS_RET_OK,
    rosidl_dynamic_typesupport_dynamic_type_builder_init(
      &serialization_support, type_name, strlen(type_name), &allocator, &builder));
  ASSERT_EQ(
    RCUTILS_RET_OK,
    rosidl_dynamic_typesupport_dynamic_type_builder_add_int32_member(
      &builder, member_id, member_name, strlen(member_name), "", 0));

  // 3. Initialize the dynamic type from its builder (the function fixed by issue #17).
  rosidl_dynamic_typesupport_dynamic_type_t dynamic_type =
    rosidl_dynamic_typesupport_get_zero_initialized_dynamic_type();
  ASSERT_EQ(
    RCUTILS_RET_OK,
    rosidl_dynamic_typesupport_dynamic_type_init_from_dynamic_type_builder(
      &builder, &allocator, &dynamic_type));

  // 4. Use the returned type AFTER initialization returned. Before the fix the backing handle
  //    was already freed at this point, so these calls read freed memory.
  size_t member_count = 0;
  ASSERT_EQ(
    RCUTILS_RET_OK,
    rosidl_dynamic_typesupport_dynamic_type_get_member_count(&dynamic_type, &member_count));
  EXPECT_EQ(1u, member_count);

  const char * read_name = nullptr;
  size_t read_name_length = 0;
  ASSERT_EQ(
    RCUTILS_RET_OK,
    rosidl_dynamic_typesupport_dynamic_type_get_name(
      &dynamic_type, &read_name, &read_name_length));
  ASSERT_NE(nullptr, read_name);
  EXPECT_EQ(std::string(type_name), std::string(read_name, read_name_length));
  // get_name duplicates the string with the type's allocator; release it.
  allocator.deallocate(const_cast<char *>(read_name), allocator.state);

  // 5. Initialize dynamic data from that type (the reporter's segfaulting step). Building data
  //    from the type is itself a use of the type handle, so it exercises the freed-handle path.
  rosidl_dynamic_typesupport_dynamic_data_t dynamic_data =
    rosidl_dynamic_typesupport_get_zero_initialized_dynamic_data();
  ASSERT_EQ(
    RCUTILS_RET_OK,
    rosidl_dynamic_typesupport_dynamic_data_init_from_dynamic_type(
      &dynamic_type, &allocator, &dynamic_data));

  // 6. Clean up all resources in reverse order of construction.
  EXPECT_EQ(RCUTILS_RET_OK, rosidl_dynamic_typesupport_dynamic_data_fini(&dynamic_data));
  EXPECT_EQ(RCUTILS_RET_OK, rosidl_dynamic_typesupport_dynamic_type_fini(&dynamic_type));
  EXPECT_EQ(RCUTILS_RET_OK, rosidl_dynamic_typesupport_dynamic_type_builder_fini(&builder));
  EXPECT_EQ(
    RCUTILS_RET_OK,
    rosidl_dynamic_typesupport_serialization_support_fini(&serialization_support));
}
