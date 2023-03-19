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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__DETAIL__FASTRTPS_DYNAMIC_TYPE_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__DETAIL__FASTRTPS_DYNAMIC_TYPE_H_

#include <rosidl_dynamic_typesupport/api/serialization_support_interface.h>


#ifdef __cplusplus
extern "C" {
#endif


// =================================================================================================
// DYNAMIC TYPE
// =================================================================================================

// DYNAMIC TYPE UTILS =======================================================================
bool
fastrtps__dynamic_type_equals(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl, const rosidl_dynamic_typesupport_dynamic_type_impl_t * other_type_impl);

size_t
fastrtps__dynamic_type_get_member_count(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl);


// DYNAMIC TYPE CONSTRUCTION =======================================================================
rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *
fastrtps__dynamic_type_builder_init(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const char * name, size_t name_length);

// NOTE(methylDragon): Delegated to rosidl_dynamic_typesupport interface library
// fastrtps__dynamic_type_builder_init_from_description

rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *
fastrtps__dynamic_type_builder_clone(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * other);

void
fastrtps__dynamic_type_builder_fini(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl);

rosidl_dynamic_typesupport_dynamic_type_impl_t *
fastrtps__dynamic_type_init_from_dynamic_type_builder(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl);

// NOTE(methylDragon): Delegated to rosidl_dynamic_typesupport interface library
// fastrtps__dynamic_type_init_from_description

rosidl_dynamic_typesupport_dynamic_type_impl_t *
fastrtps__dynamic_type_clone(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl);

void
fastrtps__dynamic_type_fini(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl);

const char *
fastrtps__dynamic_type_get_name(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl, size_t * name_length);

const char *
fastrtps__dynamic_type_builder_get_name(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, size_t * name_length);

void
fastrtps__dynamic_type_builder_set_name(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, const char * name, size_t name_length);


// DYNAMIC TYPE PRIMITIVE MEMBERS ==================================================================
void
fastrtps__dynamic_type_builder_add_bool_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_byte_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_char_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_wchar_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_float32_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_float64_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_float128_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_int8_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_uint8_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_int16_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_uint16_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_int32_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_uint32_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_int64_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_uint64_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_string_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_wstring_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_bounded_string_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t string_bound);

void
fastrtps__dynamic_type_builder_add_bounded_wstring_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t wstring_bound);


// DYNAMIC TYPE STATIC ARRAY MEMBERS ===============================================================
void
fastrtps__dynamic_type_builder_add_bool_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_byte_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_char_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_wchar_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_float32_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_float64_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_float128_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_int8_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_uint8_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_int16_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_uint16_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_int32_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_uint32_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_int64_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_uint64_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_string_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_wstring_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length);

void
fastrtps__dynamic_type_builder_add_bounded_string_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t string_bound, size_t array_length);

void
fastrtps__dynamic_type_builder_add_bounded_wstring_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t wstring_bound, size_t array_length);


// DYNAMIC TYPE UNBOUNDED SEQUENCE MEMBERS =========================================================
void
fastrtps__dynamic_type_builder_add_bool_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_byte_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_char_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_wchar_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_float32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_float64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_float128_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_int8_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_uint8_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_int16_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_uint16_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_int32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_uint32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_int64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_uint64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length);

void
fastrtps__dynamic_type_builder_add_bounded_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t string_bound);

void
fastrtps__dynamic_type_builder_add_bounded_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t wstring_bound);


// DYNAMIC TYPE BOUNDED SEQUENCE MEMBERS ===========================================================
void
fastrtps__dynamic_type_builder_add_bool_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_byte_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_char_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_wchar_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_float32_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_float64_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_float128_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_int8_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_uint8_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_int16_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_uint16_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_int32_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_uint32_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_int64_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_uint64_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t string_bound, size_t sequence_bound);

void
fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t wstring_bound, size_t sequence_bound);


// DYNAMIC TYPE NESTED MEMBERS =====================================================================
void
fastrtps__dynamic_type_builder_add_complex_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length, rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct);

void
fastrtps__dynamic_type_builder_add_complex_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct, size_t array_length);

void
fastrtps__dynamic_type_builder_add_complex_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct);

void
fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct, size_t sequence_bound);


#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__DETAIL__FASTRTPS_DYNAMIC_TYPE_H_