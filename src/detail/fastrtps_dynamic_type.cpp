// Copyright 2022 Open Source Robotics Foundation, Inc.
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

#include <string.h>
#include <string>

#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicTypePtr.h>
#include <fastrtps/types/DynamicTypeBuilderPtr.h>
#include <fastrtps/types/TypeDescriptor.h>

#include "fastrtps_dynamic_type.h"
#include "fastrtps_serialization_support_impl_handle.h"
#include "utils.hpp"

#include <rosidl_runtime_c/type_description/field__functions.h>
#include <rosidl_runtime_c/type_description/field__struct.h>
#include <rosidl_runtime_c/type_description/field_type__functions.h>
#include <rosidl_runtime_c/type_description/field_type__struct.h>
#include <rosidl_runtime_c/type_description/individual_type_description__functions.h>
#include <rosidl_runtime_c/type_description/individual_type_description__struct.h>
#include <rosidl_runtime_c/type_description/type_description__functions.h>
#include <rosidl_runtime_c/type_description/type_description__struct.h>
#include <rosidl_runtime_c/type_description_utils.h>

#include <rosidl_dynamic_typesupport/api/serialization_support_interface.h>
#include <rosidl_dynamic_typesupport/types.h>


// using eprosima::fastrtps::types::DynamicType;  // Conflicts in this scope for some reason...
using eprosima::fastrtps::types::DynamicType_ptr;
using eprosima::fastrtps::types::DynamicTypeBuilder;
using eprosima::fastrtps::types::DynamicTypeBuilder_ptr;
using eprosima::fastrtps::types::TypeDescriptor;

#define CONTAINER_UNLIMITED 0
#define EXPAND(x) x


// =================================================================================================
// DYNAMIC TYPE
// =================================================================================================

// DYNAMIC TYPE UTILS =======================================================================
bool
fastrtps__dynamic_type_equals(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl,
  const rosidl_dynamic_typesupport_dynamic_type_impl_t * other_type_impl)
{
  (void) serialization_support_impl;

  auto type = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle));
  auto other = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(other_type_impl->handle));

  return type->equals(other.get());
}


size_t
fastrtps__dynamic_type_get_member_count(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl)
{
  (void) serialization_support_impl;
  auto type = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle));

  return type->get_members_count();
}


// DYNAMIC TYPE CONSTRUCTION =======================================================================
rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *
fastrtps__dynamic_type_builder_init(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  DynamicTypeBuilder * type_builder_handle = fastrtps_impl->type_factory_->create_struct_builder();

  // We must replace "/" with "::" in type names
  std::string name_string = fastrtps__replace_string(std::string(name, name_length), "/", "::");
  type_builder_handle->set_name(name_string);
  return new rosidl_dynamic_typesupport_dynamic_type_builder_impl_t{std::move(type_builder_handle)};
}


rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *
fastrtps__dynamic_type_builder_clone(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * other)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  DynamicTypeBuilder * type_builder_handle = fastrtps_impl->type_factory_->create_builder_copy(
    static_cast<const DynamicTypeBuilder *>(other->handle));
  return new rosidl_dynamic_typesupport_dynamic_type_builder_impl_t{std::move(type_builder_handle)};
}


void
fastrtps__dynamic_type_builder_fini(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  fastrtps_impl->type_factory_->delete_builder(
    static_cast<DynamicTypeBuilder *>(type_builder_impl->handle));
}


rosidl_dynamic_typesupport_dynamic_type_impl_t *
fastrtps__dynamic_type_init_from_dynamic_type_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl)
{
  (void) serialization_support_impl;

  // Disgusting, but unavoidable... (we can't easily transfer ownership)
  //
  // We're forcing the managed pointer to persist outside of function scope by moving ownership
  // to a new, heap-allocated DynamicType_ptr (which is a shared_ptr)
  return new rosidl_dynamic_typesupport_dynamic_type_impl_t{
    static_cast<void *>(
      new DynamicType_ptr(
        std::move(static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->build())
      )
    )
  };
}


rosidl_dynamic_typesupport_dynamic_type_impl_t *
fastrtps__dynamic_type_clone(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);

  auto type = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle));

  // Disgusting, but unavoidable... (we can't easily transfer ownership)
  //
  // We're forcing the managed pointer to persist outside of function scope by moving ownership
  // to a new, heap-allocated DynamicType_ptr (which is a shared_ptr)
  return new rosidl_dynamic_typesupport_dynamic_type_impl_t{
    static_cast<void *>(
      new DynamicType_ptr(
        std::move(fastrtps_impl->type_factory_->create_alias_type(type, type->get_name()))
      )
    )
  };
}


void
fastrtps__dynamic_type_fini(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl)
{
  // You typically don't need to call this because the DynamicType_ptr should manage the
  // destruction for you
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  auto type = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle));

  fastrtps_impl->type_factory_->delete_type(type.get());
}


const char *
fastrtps__dynamic_type_get_name(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl,
  size_t * name_length)
{
  (void)serialization_support_impl;
  auto type = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle));

  // Undo the mangling
  std::string name = fastrtps__replace_string(type->get_name(), "::", "/");
  *name_length = name.size();
  return strdup(name.c_str());
}


const char *
fastrtps__dynamic_type_builder_get_name(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  size_t * name_length)
{
  (void)serialization_support_impl;

  // Undo the mangling
  std::string name = fastrtps__replace_string(
    static_cast<const DynamicTypeBuilder *>(type_builder_impl->handle)->get_name(), "::", "/");
  *name_length = name.size();
  return strdup(name.c_str());
}


void
fastrtps__dynamic_type_builder_set_name(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  const char * name, size_t name_length)
{
  (void)serialization_support_impl;
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->set_name(
    std::string(name, name_length).c_str());
}


// DYNAMIC TYPE PRIMITIVE MEMBERS ==================================================================
#define FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(FunctionT, MemberT) \
  void \
  fastrtps__dynamic_type_builder_add_ ## FunctionT ## _member( \
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, \
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, \
    rosidl_dynamic_typesupport_member_id_t id, \
    const char * name, size_t name_length, \
    const char * default_value, size_t default_value_length) \
  { \
    auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>( \
      serialization_support_impl->handle); \
    static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member( \
      id, std::string(name, name_length).c_str(), \
      fastrtps_impl->type_factory_->create_ ## MemberT ## _type(), \
      std::string(default_value, default_value_length).c_str()); \
  }


FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(bool, bool);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(byte, byte);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(char, char8);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(wchar, char16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(float32, float32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(float64, float64);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(float128, float128);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(int8, byte);  // NOTE!!
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(uint8, byte);  // NOTE!!
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(int16, int16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(uint16, uint16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(int32, int32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(uint32, uint32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(int64, int64);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN(uint64, uint64);
#undef FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_MEMBER_FN


void
fastrtps__dynamic_type_builder_add_string_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length)
{
  fastrtps__dynamic_type_builder_add_bounded_string_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_wstring_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_fixed_string_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_length)
{
  // Fixed string is bounded on the wire
  fastrtps__dynamic_type_builder_add_bounded_wstring_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    string_length);
}


void
fastrtps__dynamic_type_builder_add_fixed_wstring_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_length)
{
  // Fixed string is bounded on the wire
  fastrtps__dynamic_type_builder_add_bounded_wstring_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    wstring_length);
}


void
fastrtps__dynamic_type_builder_add_bounded_string_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_string_type(fastrtps__size_t_to_uint32_t(string_bound)),
    std::string(default_value, default_value_length).c_str());
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_wstring_type(fastrtps__size_t_to_uint32_t(wstring_bound)),
    std::string(default_value, default_value_length).c_str());
}


// DYNAMIC TYPE STATIC ARRAY MEMBERS ===============================================================
#define FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(FunctionT, MemberT) \
  void \
  fastrtps__dynamic_type_builder_add_ ## FunctionT ## _array_member( \
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, \
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, \
    rosidl_dynamic_typesupport_member_id_t id, \
    const char * name, size_t name_length, \
    const char * default_value, size_t default_value_length, \
    size_t array_length) \
  { \
    auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>( \
      serialization_support_impl->handle); \
    static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member( \
      id, std::string(name, name_length).c_str(), \
      fastrtps_impl->type_factory_->create_array_builder( \
        fastrtps_impl->type_factory_->create_ ## MemberT ## _type(), \
        {fastrtps__size_t_to_uint32_t(array_length)}), \
      std::string(default_value, default_value_length).c_str()); \
  }


FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(bool, bool);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(byte, byte);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(char, char8);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(wchar, char16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(float32, float32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(float64, float64);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(float128, float128);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(int8, byte);  // NOTE!!
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(uint8, byte);  // NOTE!!
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(int16, int16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(uint16, uint16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(int32, int32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(uint32, uint32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(int64, int64);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN(uint64, uint64);
#undef FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_ARRAY_MEMBER_FN


void
fastrtps__dynamic_type_builder_add_string_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length)
{
  fastrtps__dynamic_type_builder_add_bounded_string_array_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    CONTAINER_UNLIMITED, array_length);
}


void
fastrtps__dynamic_type_builder_add_wstring_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t array_length)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_array_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    CONTAINER_UNLIMITED, array_length);
}


void
fastrtps__dynamic_type_builder_add_fixed_string_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_length,
  size_t array_length)
{
  // Fixed string is bounded on the wire
  fastrtps__dynamic_type_builder_add_bounded_string_array_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    string_length, array_length);
}


void
fastrtps__dynamic_type_builder_add_fixed_wstring_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_length,
  size_t array_length)
{
  // Fixed string is bounded on the wire
  fastrtps__dynamic_type_builder_add_bounded_wstring_array_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    wstring_length, array_length);
}


void
fastrtps__dynamic_type_builder_add_bounded_string_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_bound,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_string_type(fastrtps__size_t_to_uint32_t(string_bound)),
      {fastrtps__size_t_to_uint32_t(array_length)}),
    std::string(default_value, default_value_length).c_str());
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_bound,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_wstring_type(fastrtps__size_t_to_uint32_t(wstring_bound)),
      {fastrtps__size_t_to_uint32_t(array_length)}),
    std::string(default_value, default_value_length).c_str());
}


// DYNAMIC TYPE UNBOUNDED SEQUENCE MEMBERS =========================================================
#define FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(FunctionT) \
  void \
  fastrtps__dynamic_type_builder_add_ ## FunctionT ## _unbounded_sequence_member( \
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, \
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, \
    rosidl_dynamic_typesupport_member_id_t id, \
    const char * name, size_t name_length, \
    const char * default_value, size_t default_value_length) \
  { \
    fastrtps__dynamic_type_builder_add_ ## FunctionT ## _bounded_sequence_member( \
      serialization_support_impl, type_builder_impl, \
      id, name, name_length, default_value, default_value_length, \
      EXPAND(CONTAINER_UNLIMITED)); \
  }


// NOTE(methylDragon): These call the bounded sequence versions with "unlimited" bound!
//                     So the type args are the same!!
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(bool);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(byte);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(char);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(wchar);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(float32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(float64);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(float128);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(int8);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(uint8);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(int16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(uint16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(int32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(uint32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(int64);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(uint64);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(string);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(wstring);
#undef FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN


void
fastrtps__dynamic_type_builder_add_fixed_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_length)
{
  // Fixed string is bounded on the wire
  fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    string_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_fixed_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_length)
{
  // Fixed string is bounded on the wire
  fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    wstring_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_bounded_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    string_bound,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    wstring_bound,
    CONTAINER_UNLIMITED);
}


// DYNAMIC TYPE BOUNDED SEQUENCE MEMBERS ===========================================================
#define FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(FunctionT, MemberT) \
  void \
  fastrtps__dynamic_type_builder_add_ ## FunctionT ## _bounded_sequence_member( \
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, \
    rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, \
    rosidl_dynamic_typesupport_member_id_t id, \
    const char * name, size_t name_length, \
    const char * default_value, size_t default_value_length, \
    size_t sequence_bound) \
  { \
    auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>( \
      serialization_support_impl->handle); \
    static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member( \
      id, std::string(name, name_length).c_str(), \
      fastrtps_impl->type_factory_->create_sequence_builder( \
        fastrtps_impl->type_factory_->create_ ## MemberT ## _type(), \
        {fastrtps__size_t_to_uint32_t(sequence_bound)}), \
      std::string(default_value, default_value_length).c_str()); \
  }


FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(bool, bool);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(byte, byte);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(char, char8);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(wchar, char16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(float32, float32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(float64, float64);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(float128, float128);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(int8, byte);  // NOTE!!
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(uint8, byte);  // NOTE!!
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(int16, int16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(uint16, uint16);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(int32, int32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(uint32, uint32);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(int64, int64);
FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN(uint64, uint64);
#undef FASTRTPS_DYNAMIC_TYPE_BUILDER_ADD_BOUNDED_SEQUENCE_MEMBER_FN


void
fastrtps__dynamic_type_builder_add_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    CONTAINER_UNLIMITED, fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    CONTAINER_UNLIMITED, fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_fixed_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    string_length, fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_fixed_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    wstring_length, fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t string_bound,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_string_type(fastrtps__size_t_to_uint32_t(string_bound)),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}),
    std::string(default_value, default_value_length).c_str());
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  size_t wstring_bound,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_wstring_type(fastrtps__size_t_to_uint32_t(wstring_bound)),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}),
    std::string(default_value, default_value_length).c_str());
}


// DYNAMIC TYPE NESTED MEMBERS =====================================================================
void
fastrtps__dynamic_type_builder_add_complex_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct)
{
  (void) serialization_support_impl;

  auto nested_struct_dynamictype_ptr = DynamicType_ptr(
    *static_cast<DynamicType_ptr *>(nested_struct->handle));

  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    nested_struct_dynamictype_ptr,
    std::string(default_value, default_value_length).c_str());
}


void
fastrtps__dynamic_type_builder_add_complex_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct,
  size_t array_length)
{
  auto nested_struct_dynamictype_ptr = DynamicType_ptr(
    *static_cast<DynamicType_ptr *>(nested_struct->handle));

  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      nested_struct_dynamictype_ptr, {fastrtps__size_t_to_uint32_t(array_length)}),
    std::string(default_value, default_value_length).c_str());
}


void
fastrtps__dynamic_type_builder_add_complex_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct)
{
  fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    nested_struct, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct,
  size_t sequence_bound)
{
  auto nested_struct_dynamictype_ptr = DynamicType_ptr(
    *static_cast<DynamicType_ptr *>(nested_struct->handle));

  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      nested_struct_dynamictype_ptr, {fastrtps__size_t_to_uint32_t(sequence_bound)}),
    std::string(default_value, default_value_length).c_str());
}


void
fastrtps__dynamic_type_builder_add_complex_member_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder)
{
  (void) serialization_support_impl;
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    static_cast<DynamicTypeBuilder *>(nested_struct_builder->handle),
    std::string(default_value, default_value_length).c_str());
}


void
fastrtps__dynamic_type_builder_add_complex_array_member_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      static_cast<DynamicTypeBuilder *>(nested_struct_builder->handle),
      {fastrtps__size_t_to_uint32_t(array_length)}),
    std::string(default_value, default_value_length).c_str());
}


void
fastrtps__dynamic_type_builder_add_complex_unbounded_sequence_member_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder)
{
  fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member_builder(
    serialization_support_impl, type_builder_impl,
    id, name, name_length, default_value, default_value_length,
    nested_struct_builder,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name, size_t name_length,
  const char * default_value, size_t default_value_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      static_cast<DynamicTypeBuilder *>(nested_struct_builder->handle),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}),
    std::string(default_value, default_value_length).c_str());
}
