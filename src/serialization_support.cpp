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

#include <rosidl_dynamic_typesupport/api/serialization_support_interface.h>
#include <rosidl_dynamic_typesupport/types.h>

#include <rcutils/types/uint8_array.h>
#include <rcutils/allocator.h>
#include <rosidl_dynamic_typesupport_fastrtps/identifier.h>
#include <rosidl_dynamic_typesupport_fastrtps/serialization_support.h>

#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/DynamicTypeBuilderFactory.h>

#include "detail/fastrtps_dynamic_data.hpp"
#include "detail/fastrtps_dynamic_type.hpp"
#include "detail/fastrtps_serialization_support_impl_handle.hpp"


// =================================================================================================
// SERIALIZATION SUPPORT IMPL
// =================================================================================================
rosidl_dynamic_typesupport_serialization_support_impl_t *
rosidl_dynamic_typesupport_fastrtps_create_serialization_support_impl()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl =
    (rosidl_dynamic_typesupport_serialization_support_impl_t *) allocator.zero_allocate(
    1, sizeof(rosidl_dynamic_typesupport_serialization_support_impl_t), &allocator.state);
  serialization_support_impl->library_identifier =
    fastrtps_serialization_support_library_identifier;

  fastrtps__serialization_support_impl_handle_t * serialization_support_impl_handle =
    (fastrtps__serialization_support_impl_handle_t *) allocator.zero_allocate(
    1, sizeof(fastrtps__serialization_support_impl_handle_t), &allocator.state);

  // The actual business
  serialization_support_impl_handle->type_factory_ =
    eprosima::fastrtps::types::DynamicTypeBuilderFactory::get_instance();
  serialization_support_impl_handle->data_factory_ =
    eprosima::fastrtps::types::DynamicDataFactory::get_instance();

  serialization_support_impl->handle = serialization_support_impl_handle;

  return serialization_support_impl;
}


// =================================================================================================
// SERIALIZATION SUPPORT INTERFACE
// =================================================================================================
rosidl_dynamic_typesupport_serialization_support_interface_t *
rosidl_dynamic_typesupport_fastrtps_create_serialization_support_interface()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosidl_dynamic_typesupport_serialization_support_interface_t * serialization_support_interface =
    (rosidl_dynamic_typesupport_serialization_support_interface_t *) allocator.zero_allocate(
    1, sizeof(rosidl_dynamic_typesupport_serialization_support_interface_t), &allocator.state);


  // CORE ==========================================================================================
  serialization_support_interface->library_identifier =
    fastrtps_serialization_support_library_identifier;

  serialization_support_interface->serialization_support_impl_handle_fini =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *))
    fastrtps__serialization_support_impl_handle_fini;


  // ===============================================================================================
  // DYNAMIC TYPE METHODS
  // ===============================================================================================
  // DYNAMIC TYPE UTILS
  serialization_support_interface->dynamic_type_equals =
    (bool (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_type_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_type_impl_t *))
    fastrtps__dynamic_type_equals;

  serialization_support_interface->dynamic_type_get_member_count =
    (size_t (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_type_impl_t *))
    fastrtps__dynamic_type_get_member_count;


  // DYNAMIC TYPE CONSTRUCTION
  serialization_support_interface->dynamic_type_builder_init =
    (rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *, const char *, size_t))
    fastrtps__dynamic_type_builder_init;

  serialization_support_interface->dynamic_type_builder_clone =
    (rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *))
    fastrtps__dynamic_type_builder_clone;

  serialization_support_interface->dynamic_type_builder_fini =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *))
    fastrtps__dynamic_type_builder_fini;

  serialization_support_interface->dynamic_type_init_from_dynamic_type_builder =
    (rosidl_dynamic_typesupport_dynamic_type_impl_t * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *))
    fastrtps__dynamic_type_init_from_dynamic_type_builder;

  serialization_support_interface->dynamic_type_clone =
    (rosidl_dynamic_typesupport_dynamic_type_impl_t * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_type_impl_t *))
    fastrtps__dynamic_type_clone;

  serialization_support_interface->dynamic_type_fini =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_impl_t *))
    fastrtps__dynamic_type_fini;

  serialization_support_interface->dynamic_type_get_name =
    (const char * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_type_impl_t *, size_t *))
    fastrtps__dynamic_type_get_name;

  serialization_support_interface->dynamic_type_builder_get_name =
    (const char * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *, size_t *))
    fastrtps__dynamic_type_builder_get_name;

  serialization_support_interface->dynamic_type_builder_set_name =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *, const char *, size_t))
    fastrtps__dynamic_type_builder_set_name;


  // DYNAMIC TYPE PRIMITIVE MEMBERS ================================================================
  #define FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(FnName) \
  serialization_support_interface->FnName = \
    (void (*)( \
      rosidl_dynamic_typesupport_serialization_support_impl_t *, \
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *, \
      rosidl_dynamic_typesupport_member_id_t, \
      const char *, size_t, \
      const char *, size_t)) \
    fastrtps__ ## FnName;

  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_bool_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_byte_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_char_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_wchar_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_float32_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_float64_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_float128_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_int8_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_uint8_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_int16_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_uint16_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_int32_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_uint32_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_int64_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_uint64_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_string_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN(dynamic_type_builder_add_wstring_member);
  #undef FASTRTPS_SERIALIZATION_SUPPORT_ADD_MEMBER_FN


  serialization_support_interface->dynamic_type_builder_add_fixed_string_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t))
    fastrtps__dynamic_type_builder_add_fixed_string_member;

  serialization_support_interface->dynamic_type_builder_add_fixed_wstring_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t))
    fastrtps__dynamic_type_builder_add_fixed_wstring_member;

  serialization_support_interface->dynamic_type_builder_add_bounded_string_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t))
    fastrtps__dynamic_type_builder_add_bounded_string_member;

  serialization_support_interface->dynamic_type_builder_add_bounded_wstring_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t))
    fastrtps__dynamic_type_builder_add_bounded_wstring_member;


  // DYNAMIC TYPE STATIC ARRAY MEMBERS =============================================================
  #define FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(FnName) \
  serialization_support_interface->FnName = \
    (void (*)( \
      rosidl_dynamic_typesupport_serialization_support_impl_t *, \
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *, \
      rosidl_dynamic_typesupport_member_id_t, \
      const char *, size_t, \
      const char *, size_t, size_t)) \
    fastrtps__ ## FnName;

  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_bool_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_byte_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_char_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_wchar_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_float32_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_float64_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_float128_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_int8_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_uint8_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_int16_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_uint16_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_int32_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_uint32_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_int64_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_uint64_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_string_array_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN(
    dynamic_type_builder_add_wstring_array_member);
  #undef FASTRTPS_SERIALIZATION_SUPPORT_ADD_ARRAY_MEMBER_FN


  serialization_support_interface->dynamic_type_builder_add_fixed_string_array_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t, size_t))
    fastrtps__dynamic_type_builder_add_fixed_string_array_member;

  serialization_support_interface->dynamic_type_builder_add_fixed_wstring_array_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t, size_t))
    fastrtps__dynamic_type_builder_add_fixed_wstring_array_member;

  serialization_support_interface->dynamic_type_builder_add_bounded_string_array_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t, size_t))
    fastrtps__dynamic_type_builder_add_bounded_string_array_member;

  serialization_support_interface->dynamic_type_builder_add_bounded_wstring_array_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t, size_t))
    fastrtps__dynamic_type_builder_add_bounded_wstring_array_member;


  // DYNAMIC TYPE UNBOUNDED SEQUENCE MEMBERS =======================================================
  #define FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(FnName) \
  serialization_support_interface->FnName = \
    (void (*)( \
      rosidl_dynamic_typesupport_serialization_support_impl_t *, \
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *, \
      rosidl_dynamic_typesupport_member_id_t, \
      const char *, size_t, \
      const char *, size_t)) \
    fastrtps__ ## FnName;

  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_bool_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_byte_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_char_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_wchar_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_float32_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_float64_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_float128_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_int8_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_uint8_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_int16_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_uint16_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_int32_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_uint32_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_int64_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_uint64_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_string_unbounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_wstring_unbounded_sequence_member);
  #undef FASTRTPS_SERIALIZATION_SUPPORT_ADD_UNBOUNDED_SEQUENCE_MEMBER_FN


  serialization_support_interface->
  dynamic_type_builder_add_fixed_string_unbounded_sequence_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t))
    fastrtps__dynamic_type_builder_add_fixed_string_unbounded_sequence_member;

  serialization_support_interface->
  dynamic_type_builder_add_fixed_wstring_unbounded_sequence_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t))
    fastrtps__dynamic_type_builder_add_fixed_wstring_unbounded_sequence_member;

  serialization_support_interface->
  dynamic_type_builder_add_bounded_string_unbounded_sequence_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t))
    fastrtps__dynamic_type_builder_add_bounded_string_unbounded_sequence_member;

  serialization_support_interface->
  dynamic_type_builder_add_bounded_wstring_unbounded_sequence_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t))
    fastrtps__dynamic_type_builder_add_bounded_wstring_unbounded_sequence_member;


  // DYNAMIC TYPE BOUNDED SEQUENCE MEMBERS =========================================================
  #define FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(FnName) \
  serialization_support_interface->FnName = \
    (void (*)( \
      rosidl_dynamic_typesupport_serialization_support_impl_t *, \
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *, \
      rosidl_dynamic_typesupport_member_id_t, \
      const char *, size_t, \
      const char *, size_t, size_t)) \
    fastrtps__ ## FnName;

  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_bool_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_byte_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_char_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_wchar_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_float32_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_float64_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_float128_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_int8_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_uint8_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_int16_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_uint16_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_int32_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_uint32_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_int64_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_uint64_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_string_bounded_sequence_member);
  FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN(
    dynamic_type_builder_add_wstring_bounded_sequence_member);
  #undef FASTRTPS_SERIALIZATION_SUPPORT_ADD_BOUNDED_SEQUENCE_MEMBER_FN


  serialization_support_interface->dynamic_type_builder_add_fixed_string_bounded_sequence_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t, size_t))
    fastrtps__dynamic_type_builder_add_fixed_string_bounded_sequence_member;

  serialization_support_interface->dynamic_type_builder_add_fixed_wstring_bounded_sequence_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t, size_t))
    fastrtps__dynamic_type_builder_add_fixed_wstring_bounded_sequence_member;

  serialization_support_interface->dynamic_type_builder_add_bounded_string_bounded_sequence_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t, size_t))
    fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member;

  serialization_support_interface->
  dynamic_type_builder_add_bounded_wstring_bounded_sequence_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t, size_t, size_t))
    fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member;


  // DYNAMIC TYPE NESTED MEMBERS
  serialization_support_interface->dynamic_type_builder_add_complex_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t,
      rosidl_dynamic_typesupport_dynamic_type_impl_t *))
    fastrtps__dynamic_type_builder_add_complex_member;

  serialization_support_interface->dynamic_type_builder_add_complex_array_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t,
      rosidl_dynamic_typesupport_dynamic_type_impl_t *, size_t))
    fastrtps__dynamic_type_builder_add_complex_array_member;

  serialization_support_interface->dynamic_type_builder_add_complex_unbounded_sequence_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t,
      rosidl_dynamic_typesupport_dynamic_type_impl_t *))
    fastrtps__dynamic_type_builder_add_complex_unbounded_sequence_member;

  serialization_support_interface->dynamic_type_builder_add_complex_bounded_sequence_member =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t,
      rosidl_dynamic_typesupport_dynamic_type_impl_t *, size_t))
    fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member;


  serialization_support_interface->dynamic_type_builder_add_complex_member_builder =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *))
    fastrtps__dynamic_type_builder_add_complex_member_builder;

  serialization_support_interface->dynamic_type_builder_add_complex_array_member_builder =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *, size_t))
    fastrtps__dynamic_type_builder_add_complex_array_member_builder;

  serialization_support_interface
  ->dynamic_type_builder_add_complex_unbounded_sequence_member_builder =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *))
    fastrtps__dynamic_type_builder_add_complex_unbounded_sequence_member_builder;

  serialization_support_interface
  ->dynamic_type_builder_add_complex_bounded_sequence_member_builder =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t,
      const char *, size_t,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *, size_t))
    fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member_builder;

  // ===============================================================================================
  // DYNAMIC DATA METHODS
  // ===============================================================================================
  // DYNAMIC DATA UTILS
  serialization_support_interface->dynamic_data_clear_all_values =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_clear_all_values;

  serialization_support_interface->dynamic_data_clear_nonkey_values =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_clear_nonkey_values;

  serialization_support_interface->dynamic_data_clear_sequence_data =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_clear_sequence_data;

  serialization_support_interface->dynamic_data_clear_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t))
    fastrtps__dynamic_data_clear_value;

  serialization_support_interface->dynamic_data_equals =
    (bool (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_equals;

  serialization_support_interface->dynamic_data_get_item_count =
    (size_t (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_get_item_count;

  serialization_support_interface->dynamic_data_get_member_id_by_name =
    (rosidl_dynamic_typesupport_member_id_t (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      const char *, size_t))
    fastrtps__dynamic_data_get_member_id_by_name;

  serialization_support_interface->dynamic_data_get_member_id_at_index =
    (rosidl_dynamic_typesupport_member_id_t (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *, size_t))
    fastrtps__dynamic_data_get_member_id_at_index;

  serialization_support_interface->dynamic_data_get_array_index =
    (rosidl_dynamic_typesupport_member_id_t (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *, size_t))
    fastrtps__dynamic_data_get_array_index;

  serialization_support_interface->dynamic_data_loan_value =
    (rosidl_dynamic_typesupport_dynamic_data_impl_t * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t))
    fastrtps__dynamic_data_loan_value;

  serialization_support_interface->dynamic_data_return_loaned_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_return_loaned_value;

  serialization_support_interface->dynamic_data_print =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_print;

  serialization_support_interface->dynamic_data_get_name =
    (const char * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *, size_t *))
    fastrtps__dynamic_data_get_name;


  // DYNAMIC DATA CONSTRUCTION
  serialization_support_interface->dynamic_data_init_from_dynamic_type_builder =
    (rosidl_dynamic_typesupport_dynamic_data_impl_t * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *))
    fastrtps__dynamic_data_init_from_dynamic_type_builder;

  serialization_support_interface->dynamic_data_init_from_dynamic_type =
    (rosidl_dynamic_typesupport_dynamic_data_impl_t * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_type_impl_t *))
    fastrtps__dynamic_data_init_from_dynamic_type;

  serialization_support_interface->dynamic_data_clone =
    (rosidl_dynamic_typesupport_dynamic_data_impl_t * (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_clone;

  serialization_support_interface->dynamic_data_fini =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_fini;


  // DYNAMIC DATA SERIALIZATION
  serialization_support_interface->dynamic_data_serialize =
    (bool (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rcutils_uint8_array_t *))
    fastrtps__dynamic_data_serialize;

  serialization_support_interface->dynamic_data_deserialize =
    (bool (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rcutils_uint8_array_t *))
    fastrtps__dynamic_data_deserialize;


  // DYNAMIC DATA PRIMITIVE MEMBER GETTERS =========================================================
  #define FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(FnName, ValueT) \
  serialization_support_interface->FnName = \
    (void (*)( \
      rosidl_dynamic_typesupport_serialization_support_impl_t *, \
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *, \
      rosidl_dynamic_typesupport_member_id_t, \
      ValueT *)) \
    fastrtps__ ## FnName;

  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_bool_value, bool);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_byte_value, unsigned char);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_char_value, char);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_wchar_value, char16_t);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_float32_value, float);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_float64_value, double);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_float128_value, long double);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_int8_value, int8_t);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_uint8_value, uint8_t);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_int16_value, int16_t);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_uint16_value, uint16_t);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_int32_value, int32_t);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_uint32_value, uint32_t);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_int64_value, int64_t);
  FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN(dynamic_data_get_uint64_value, uint64_t);
  #undef FASTRTPS_SERIALIZATION_SUPPORT_GET_VALUE_FN


  serialization_support_interface->dynamic_data_get_string_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      char **, size_t *))
    fastrtps__dynamic_data_get_string_value;

  serialization_support_interface->dynamic_data_get_wstring_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      char16_t **, size_t *))
    fastrtps__dynamic_data_get_wstring_value;

  serialization_support_interface->dynamic_data_get_fixed_string_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      char **, size_t *, size_t))
    fastrtps__dynamic_data_get_fixed_string_value;

  serialization_support_interface->dynamic_data_get_fixed_wstring_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      char16_t **, size_t *, size_t))
    fastrtps__dynamic_data_get_fixed_wstring_value;

  serialization_support_interface->dynamic_data_get_bounded_string_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      char **, size_t *, size_t))
    fastrtps__dynamic_data_get_bounded_string_value;

  serialization_support_interface->dynamic_data_get_bounded_wstring_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      char16_t **, size_t *, size_t))
    fastrtps__dynamic_data_get_bounded_wstring_value;


  // DYNAMIC DATA PRIMITIVE MEMBER SETTERS =========================================================
  #define FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(FnName, ValueT) \
  serialization_support_interface->FnName = \
    (void (*)( \
      rosidl_dynamic_typesupport_serialization_support_impl_t *, \
      rosidl_dynamic_typesupport_dynamic_data_impl_t *, \
      rosidl_dynamic_typesupport_member_id_t, \
      ValueT)) \
    fastrtps__ ## FnName;

  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_bool_value, bool);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_byte_value, unsigned char);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_char_value, char);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_wchar_value, char16_t);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_float32_value, float);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_float64_value, double);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_float128_value, long double);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_int8_value, int8_t);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_uint8_value, uint8_t);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_int16_value, int16_t);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_uint16_value, uint16_t);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_int32_value, int32_t);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_uint32_value, uint32_t);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_int64_value, int64_t);
  FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN(dynamic_data_set_uint64_value, uint64_t);
  #undef FASTRTPS_SERIALIZATION_SUPPORT_SET_VALUE_FN


  serialization_support_interface->dynamic_data_set_string_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t))
    fastrtps__dynamic_data_set_string_value;

  serialization_support_interface->dynamic_data_set_wstring_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char16_t *, size_t))
    fastrtps__dynamic_data_set_wstring_value;

  serialization_support_interface->dynamic_data_set_fixed_string_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t, size_t))
    fastrtps__dynamic_data_set_fixed_string_value;

  serialization_support_interface->dynamic_data_set_fixed_wstring_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char16_t *, size_t, size_t))
    fastrtps__dynamic_data_set_fixed_wstring_value;

  serialization_support_interface->dynamic_data_set_bounded_string_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char *, size_t, size_t))
    fastrtps__dynamic_data_set_bounded_string_value;

  serialization_support_interface->dynamic_data_set_bounded_wstring_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      const char16_t *, size_t, size_t))
    fastrtps__dynamic_data_set_bounded_wstring_value;


  // DYNAMIC TYPE SEQUENCES ========================================================================
  serialization_support_interface->dynamic_data_clear_sequence_data =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_clear_sequence_data;

  serialization_support_interface->dynamic_data_remove_sequence_data =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t))
    fastrtps__dynamic_data_remove_sequence_data;

  serialization_support_interface->dynamic_data_insert_sequence_data =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t *))
    fastrtps__dynamic_data_insert_sequence_data;

  #define FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(FnName, ValueT) \
  serialization_support_interface->FnName = \
    (void (*)( \
      rosidl_dynamic_typesupport_serialization_support_impl_t *, \
      rosidl_dynamic_typesupport_dynamic_data_impl_t *, ValueT, \
      rosidl_dynamic_typesupport_member_id_t *)) \
    fastrtps__ ## FnName;

  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_bool_value, bool);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_byte_value, unsigned char);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_char_value, char);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_wchar_value, char16_t);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_float32_value, float);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_float64_value, double);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_float128_value, long double);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_int8_value, int8_t);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_uint8_value, uint8_t);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_int16_value, int16_t);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_uint16_value, uint16_t);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_int32_value, int32_t);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_uint32_value, uint32_t);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_int64_value, int64_t);
  FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN(dynamic_data_insert_uint64_value, uint64_t);
  #undef FASTRTPS_SERIALIZATION_SUPPORT_INSERT_VALUE_FN


  serialization_support_interface->dynamic_data_insert_string_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      const char *, size_t,
      rosidl_dynamic_typesupport_member_id_t *))
    fastrtps__dynamic_data_insert_string_value;

  serialization_support_interface->dynamic_data_insert_wstring_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      const char16_t *,
      size_t,
      rosidl_dynamic_typesupport_member_id_t *))
    fastrtps__dynamic_data_insert_wstring_value;

  serialization_support_interface->dynamic_data_insert_fixed_string_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      const char *, size_t, size_t,
      rosidl_dynamic_typesupport_member_id_t *))
    fastrtps__dynamic_data_insert_fixed_string_value;

  serialization_support_interface->dynamic_data_insert_fixed_wstring_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      const char16_t *, size_t, size_t,
      rosidl_dynamic_typesupport_member_id_t *))
    fastrtps__dynamic_data_insert_fixed_wstring_value;

  serialization_support_interface->dynamic_data_insert_bounded_string_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      const char *, size_t, size_t,
      rosidl_dynamic_typesupport_member_id_t *))
    fastrtps__dynamic_data_insert_bounded_string_value;

  serialization_support_interface->dynamic_data_insert_bounded_wstring_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      const char16_t *, size_t, size_t,
      rosidl_dynamic_typesupport_member_id_t *))
    fastrtps__dynamic_data_insert_bounded_wstring_value;


  // DYNAMIC TYPE NESTED
  serialization_support_interface->dynamic_data_get_complex_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      rosidl_dynamic_typesupport_dynamic_data_impl_t **))
    fastrtps__dynamic_data_get_complex_value;

  serialization_support_interface->dynamic_data_set_complex_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *))
    fastrtps__dynamic_data_set_complex_value;

  serialization_support_interface->dynamic_data_insert_complex_value_copy =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      const rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t *))
    fastrtps__dynamic_data_insert_complex_value_copy;

  serialization_support_interface->dynamic_data_insert_complex_value =
    (void (*)(
      rosidl_dynamic_typesupport_serialization_support_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_dynamic_data_impl_t *,
      rosidl_dynamic_typesupport_member_id_t *))
    fastrtps__dynamic_data_insert_complex_value;


  return serialization_support_interface;
}
