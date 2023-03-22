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
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  DynamicTypeBuilder * type_builder_handle = fastrtps_impl->type_factory_->create_struct_builder();
  type_builder_handle->set_name(std::string(name, name_length).c_str());
  return new rosidl_dynamic_typesupport_dynamic_type_builder_impl_t{std::move(type_builder_handle)};
}


rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *
fastrtps__dynamic_type_builder_init_from_description(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_runtime_c__type_description__TypeDescription * description)
{
  // Validity check is deferred upstream

  // Create description copy to string replace "/" delimiters to "::"
  rosidl_runtime_c__type_description__TypeDescription * repl_desc =
    rosidl_runtime_c__type_description__TypeDescription__create();
  if (repl_desc == NULL) {
    return NULL;
  }

  // NOTE(methylDragon): We only in-place modify a copy, so the passed in description is not
  //                     actually modified, respecting const correctness
  rosidl_runtime_c__type_description__TypeDescription__copy(description, repl_desc);
  auto ret =
    rosidl_runtime_c_type_description_utils_repl_all_type_description_type_names_in_place(
    repl_desc, "/", "::");
  if (ret != RCUTILS_RET_OK) {
    rosidl_runtime_c__type_description__TypeDescription__destroy(repl_desc);
    return NULL;
  }

  // NOTE(methylDragon): I'm not sure if this should be here, or if I should introduce RCUTILS
  //                     just for this single logging call...
  if (!rosidl_runtime_c__type_description__TypeDescription__are_equal(description, repl_desc)) {
    std::cout << "[INFO] Input description was modified by FastRTPS to replace '/' with '::' "
              << "in type names for IDL compatibility!" << std::endl;
  }

  const rosidl_runtime_c__type_description__IndividualTypeDescription * main_description =
    &repl_desc->type_description;

  auto out = new rosidl_dynamic_typesupport_dynamic_type_builder_impl_t{std::move(
      static_cast<DynamicTypeBuilder *>(
        fastrtps__dynamic_type_builder_init(
          serialization_support_impl,
          main_description->type_name.data,
          main_description->type_name.size
        )->handle
      )
    )};

  for (size_t i = 0; i < main_description->fields.size; i++) {
    rosidl_runtime_c__type_description__Field * field = &main_description->fields.data[i];

    switch (field->type.type_id) {
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NOT_SET:
        std::cerr << "Field type_impl not set for field [" << field->name.data << "]" << std::endl;
        fastrtps__dynamic_type_builder_fini(serialization_support_impl, out);
        rosidl_runtime_c__type_description__TypeDescription__destroy(repl_desc);
        return NULL;
        break;

      // PRIMITIVES
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOOLEAN:
        fastrtps__dynamic_type_builder_add_bool_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BYTE:
        fastrtps__dynamic_type_builder_add_byte_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_CHAR:
        fastrtps__dynamic_type_builder_add_char_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT32:
        fastrtps__dynamic_type_builder_add_float32_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT64:
        fastrtps__dynamic_type_builder_add_float64_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT8:
        fastrtps__dynamic_type_builder_add_int8_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT8:
        fastrtps__dynamic_type_builder_add_uint8_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT16:
        fastrtps__dynamic_type_builder_add_int16_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT16:
        fastrtps__dynamic_type_builder_add_uint16_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT32:
        fastrtps__dynamic_type_builder_add_int32_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT32:
        fastrtps__dynamic_type_builder_add_uint32_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT64:
        fastrtps__dynamic_type_builder_add_int64_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT64:
        fastrtps__dynamic_type_builder_add_uint64_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_STRING:
        fastrtps__dynamic_type_builder_add_string_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WSTRING:
        fastrtps__dynamic_type_builder_add_wstring_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_STRING:
        fastrtps__dynamic_type_builder_add_bounded_string_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_WSTRING:
        fastrtps__dynamic_type_builder_add_bounded_wstring_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;

      // STATIC ARRAYS
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOOLEAN_ARRAY:
        fastrtps__dynamic_type_builder_add_bool_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BYTE_ARRAY:
        fastrtps__dynamic_type_builder_add_byte_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_CHAR_ARRAY:
        fastrtps__dynamic_type_builder_add_char_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT32_ARRAY:
        fastrtps__dynamic_type_builder_add_float32_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT64_ARRAY:
        fastrtps__dynamic_type_builder_add_float64_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT8_ARRAY:
        fastrtps__dynamic_type_builder_add_int8_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT8_ARRAY:
        fastrtps__dynamic_type_builder_add_uint8_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT16_ARRAY:
        fastrtps__dynamic_type_builder_add_int16_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT16_ARRAY:
        fastrtps__dynamic_type_builder_add_uint16_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT32_ARRAY:
        fastrtps__dynamic_type_builder_add_int32_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT32_ARRAY:
        fastrtps__dynamic_type_builder_add_uint32_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT64_ARRAY:
        fastrtps__dynamic_type_builder_add_int64_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT64_ARRAY:
        fastrtps__dynamic_type_builder_add_uint64_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_STRING_ARRAY:
        fastrtps__dynamic_type_builder_add_string_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WSTRING_ARRAY:
        fastrtps__dynamic_type_builder_add_wstring_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_STRING_ARRAY:
        fastrtps__dynamic_type_builder_add_bounded_string_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.string_capacity, field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_WSTRING_ARRAY:
        fastrtps__dynamic_type_builder_add_bounded_wstring_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.string_capacity, field->type.capacity);
        break;

      // UNBOUNDED SEQUENCES
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOOLEAN_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_bool_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BYTE_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_byte_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_CHAR_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_char_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT32_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_float32_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT64_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_float64_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT8_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_int8_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT8_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_uint8_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT16_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_int16_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT16_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_uint16_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT32_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_int32_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT32_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_uint32_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT64_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_int64_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT64_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_uint64_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_STRING_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_string_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WSTRING_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_wstring_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_STRING_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_bounded_string_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.string_capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_WSTRING_UNBOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_bounded_wstring_unbounded_sequence_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.string_capacity);
        break;

      // BOUNDED SEQUENCES
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOOLEAN_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_bool_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BYTE_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_byte_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_CHAR_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_char_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT32_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_float32_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_FLOAT64_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_float64_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT8_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_int8_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT8_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_uint8_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT16_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_int16_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT16_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_uint16_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT32_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_int32_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT32_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_uint32_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_INT64_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_int64_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_UINT64_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_uint64_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_STRING_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_string_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_WSTRING_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_wstring_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_STRING_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_bounded_string_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.string_capacity, field->type.capacity);
        break;
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_BOUNDED_WSTRING_BOUNDED_SEQUENCE:
        fastrtps__dynamic_type_builder_add_bounded_wstring_array_member(
          serialization_support_impl, out, i, field->name.data, field->name.size,
          field->type.string_capacity, field->type.capacity);
        break;

      // NESTED
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE:
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE_ARRAY:
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE:
      case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE:
        {
          if (field->type.nested_type_name.data == NULL) {
            std::cerr << "Nested type_impl name is missing in description for field ["
                      << field->name.data << "]" << std::endl;
            fastrtps__dynamic_type_builder_fini(serialization_support_impl, out);
            rosidl_runtime_c__type_description__TypeDescription__destroy(repl_desc);
            return NULL;
          }

          // Create a new type description to pass to the next layer
          rosidl_runtime_c__type_description__TypeDescription * recurse_desc = NULL;
          rosidl_runtime_c__type_description__IndividualTypeDescription * recurse_indiv_desc = NULL;

          // NOTE(methylDragon): recurse_indiv_desc borrows from
          //                     description->referenced_type_descriptions.
          //                     It is NOT a copy!!
          rosidl_runtime_c_type_description_utils_find_referenced_type_description(
            &repl_desc->referenced_type_descriptions,
            field->type.nested_type_name.data,
            &recurse_indiv_desc);
          if (recurse_indiv_desc == NULL) {
            fastrtps__dynamic_type_builder_fini(serialization_support_impl, out);
            rosidl_runtime_c__type_description__TypeDescription__destroy(repl_desc);
            return NULL;
          }

          rosidl_runtime_c_type_description_utils_get_referenced_type_description_as_type_description(
            &repl_desc->referenced_type_descriptions,
            recurse_indiv_desc,
            &recurse_desc,
            true);  // Coerce to valid
          if (recurse_desc == NULL) {
            rosidl_runtime_c__type_description__IndividualTypeDescription__destroy(
              recurse_indiv_desc);
            fastrtps__dynamic_type_builder_fini(serialization_support_impl, out);
            rosidl_runtime_c__type_description__TypeDescription__destroy(repl_desc);
            return NULL;
          }

          // Recurse
          rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_type_builder =
            fastrtps__dynamic_type_builder_init_from_description(
            serialization_support_impl, recurse_desc);
          rosidl_runtime_c__type_description__TypeDescription__destroy(recurse_desc);

          if (nested_type_builder == NULL) {
            std::cerr << "Could not construct nested type builder impl for field ["
                      << field->name.data << "]" << std::endl;
            rosidl_runtime_c__type_description__IndividualTypeDescription__destroy(
              recurse_indiv_desc);
            rosidl_runtime_c__type_description__TypeDescription__destroy(recurse_desc);
            fastrtps__dynamic_type_builder_fini(serialization_support_impl, out);
            rosidl_runtime_c__type_description__TypeDescription__destroy(repl_desc);
            return NULL;
          }

          switch (field->type.type_id) {
            case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE:
              fastrtps__dynamic_type_builder_add_complex_member_builder(
                serialization_support_impl, out, i, field->name.data, field->name.size,
                nested_type_builder);
              break;

            case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE_ARRAY:
              fastrtps__dynamic_type_builder_add_complex_array_member_builder(
                serialization_support_impl, out, i, field->name.data, field->name.size,
                nested_type_builder, field->type.capacity);
              break;

            case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE_UNBOUNDED_SEQUENCE:
              fastrtps__dynamic_type_builder_add_complex_unbounded_sequence_member_builder(
                serialization_support_impl, out, i, field->name.data, field->name.size,
                nested_type_builder);
              break;

            case ROSIDL_DYNAMIC_TYPESUPPORT_FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE:
              fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member_builder(
                serialization_support_impl, out, i, field->name.data, field->name.size,
                nested_type_builder, field->type.capacity);
              break;
          }
          fastrtps__dynamic_type_builder_fini(serialization_support_impl, nested_type_builder);
        }
        break;

      default:
        std::cerr << "Invalid field type_impl: " << field->type.type_id << "!" << std::endl;
        break;
    }
  }
  rosidl_runtime_c__type_description__TypeDescription__destroy(repl_desc);
  return out;
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


// NOTE(methylDragon): Delegated to rosidl_dynamic_typesupport interface library
// fastrtps__dynamic_type_init_from_description


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
  std::string name = type->get_name();
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
  std::string name = static_cast<const DynamicTypeBuilder *>(type_builder_impl->handle)->get_name();
  *name_length = name.size();
  return strdup(name.c_str());
}


void
fastrtps__dynamic_type_builder_set_name(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  const char * name,
  size_t name_length)
{
  (void)serialization_support_impl;
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->set_name(
    std::string(name, name_length).c_str());
}


// DYNAMIC TYPE PRIMITIVE MEMBERS ==================================================================
void
fastrtps__dynamic_type_builder_add_bool_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_bool_type());
}


void
fastrtps__dynamic_type_builder_add_byte_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_byte_type());
}


void
fastrtps__dynamic_type_builder_add_char_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_char8_type());
}


void
fastrtps__dynamic_type_builder_add_wchar_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_char16_type());
}


void
fastrtps__dynamic_type_builder_add_float32_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_float32_type());
}


void
fastrtps__dynamic_type_builder_add_float64_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_float64_type());
}


void
fastrtps__dynamic_type_builder_add_float128_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_float128_type());
}


void
fastrtps__dynamic_type_builder_add_int8_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_byte_member(
    serialization_support_impl, type_builder_impl, id, name, name_length);
}


void
fastrtps__dynamic_type_builder_add_uint8_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_byte_member(
    serialization_support_impl, type_builder_impl, id, name, name_length);
}


void
fastrtps__dynamic_type_builder_add_int16_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_int16_type());
}


void
fastrtps__dynamic_type_builder_add_uint16_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_uint16_type());
}


void
fastrtps__dynamic_type_builder_add_int32_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_int32_type());
}


void
fastrtps__dynamic_type_builder_add_uint32_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_uint32_type());
}


void
fastrtps__dynamic_type_builder_add_int64_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_int64_type());
}


void
fastrtps__dynamic_type_builder_add_uint64_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_uint64_type());
}


void
fastrtps__dynamic_type_builder_add_string_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_bounded_string_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_wstring_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_bounded_string_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t string_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_string_type(string_bound));
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t wstring_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_wstring_type(wstring_bound));
}


// DYNAMIC TYPE STATIC ARRAY MEMBERS ===============================================================
void
fastrtps__dynamic_type_builder_add_bool_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_bool_type(), {fastrtps__size_t_to_uint32_t(
          array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_byte_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_byte_type(), {fastrtps__size_t_to_uint32_t(
          array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_char_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_char8_type(),
      {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_wchar_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_char16_type(),
      {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_float32_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_float32_type(),
      {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_float64_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_float64_type(),
      {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_float128_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_float128_type(),
      {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_int8_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  fastrtps__dynamic_type_builder_add_byte_array_member(
    serialization_support_impl,
    type_builder_impl, id, name, name_length,
    array_length);
}


void
fastrtps__dynamic_type_builder_add_uint8_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  fastrtps__dynamic_type_builder_add_byte_array_member(
    serialization_support_impl,
    type_builder_impl, id, name, name_length,
    array_length);
}


void
fastrtps__dynamic_type_builder_add_int16_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_int16_type(),
      {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_uint16_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_uint16_type(),
      {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_int32_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_int32_type(),
      {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_uint32_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_uint32_type(),
      {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_int64_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_int64_type(),
      {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_uint64_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_uint64_type(),
      {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_string_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  fastrtps__dynamic_type_builder_add_bounded_string_array_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED,
    array_length);
}


void
fastrtps__dynamic_type_builder_add_wstring_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t array_length)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_array_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED,
    array_length);
}


void
fastrtps__dynamic_type_builder_add_bounded_string_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t string_bound,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_string_type(string_bound),
      {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t wstring_bound,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_wstring_type(wstring_bound),
      {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


// DYNAMIC TYPE UNBOUNDED SEQUENCE MEMBERS =========================================================
void
fastrtps__dynamic_type_builder_add_bool_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_bool_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_byte_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_byte_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_char_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_char_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_wchar_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_wchar_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_float32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_float32_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_float64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_float64_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_float128_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_float128_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_int8_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_int8_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length, CONTAINER_UNLIMITED);
}

void
fastrtps__dynamic_type_builder_add_uint8_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_uint8_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_int16_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_int16_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_uint16_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_uint16_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_int32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_int32_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_uint32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_uint32_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_int64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_int64_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_uint64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_uint64_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_string_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length)
{
  fastrtps__dynamic_type_builder_add_wstring_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_bounded_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t string_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, string_bound,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t wstring_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, wstring_bound,
    CONTAINER_UNLIMITED);
}


// DYNAMIC TYPE BOUNDED SEQUENCE MEMBERS ===========================================================
void
fastrtps__dynamic_type_builder_add_bool_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_bool_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_byte_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_byte_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_char_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_char8_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_wchar_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_char16_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_float32_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_float32_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_float64_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_float64_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_float128_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_float128_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_int8_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_byte_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length, fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_uint8_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_byte_bounded_sequence_member(
    serialization_support_impl,
    type_builder_impl, id, name,
    name_length, fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_int16_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_int16_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_uint16_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_uint16_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_int32_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_int32_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)})
  );
}


void
fastrtps__dynamic_type_builder_add_uint32_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_uint32_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_int64_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_int64_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_uint64_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_uint64_type(),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED,
    fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED,
    fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t string_bound,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_string_type(string_bound),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  size_t wstring_bound,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_wstring_type(wstring_bound),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


// DYNAMIC TYPE NESTED MEMBERS =====================================================================
void
fastrtps__dynamic_type_builder_add_complex_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct)
{
  (void) serialization_support_impl;

  auto nested_struct_dynamictype_ptr = DynamicType_ptr(
    *static_cast<DynamicType_ptr *>(nested_struct->handle));

  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), nested_struct_dynamictype_ptr);
}


void
fastrtps__dynamic_type_builder_add_complex_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct,
  size_t array_length)
{
  auto nested_struct_dynamictype_ptr = DynamicType_ptr(
    *static_cast<DynamicType_ptr *>(nested_struct->handle));

  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      nested_struct_dynamictype_ptr, {fastrtps__size_t_to_uint32_t(array_length)}));
}


void
fastrtps__dynamic_type_builder_add_complex_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct)
{
  fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, nested_struct,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct,
  size_t sequence_bound)
{
  auto nested_struct_dynamictype_ptr = DynamicType_ptr(
    *static_cast<DynamicType_ptr *>(nested_struct->handle));

  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      nested_struct_dynamictype_ptr, {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_complex_member_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder)
{
  (void) serialization_support_impl;
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(),
    static_cast<DynamicTypeBuilder *>(nested_struct_builder->handle));
}


void
fastrtps__dynamic_type_builder_add_complex_array_member_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      static_cast<DynamicTypeBuilder *>(nested_struct_builder->handle),
      {fastrtps__size_t_to_uint32_t(array_length)}));
}


void
fastrtps__dynamic_type_builder_add_complex_unbounded_sequence_member_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder)
{
  fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member_builder(
    serialization_support_impl, type_builder_impl, id, name, name_length, nested_struct_builder,
    CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  const char * name,
  size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * nested_struct_builder,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      static_cast<DynamicTypeBuilder *>(nested_struct_builder->handle),
      {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}
