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

#include "dynamic_type.h"
#include "serialization_support_impl_handle.h"
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
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl, const rosidl_dynamic_typesupport_dynamic_type_impl_t * other_type_impl)
{
  (void) serialization_support_impl;

  auto type = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle));
  auto other = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(other_type_impl->handle));

  return type->equals(other.get());
}


size_t
fastrtps__dynamic_type_get_member_count(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl)
{
  (void) serialization_support_impl;
  auto type = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle));

  return type->get_members_count();
}


// DYNAMIC TYPE CONSTRUCTION =======================================================================
rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *
fastrtps__dynamic_type_builder_init(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  DynamicTypeBuilder * type_builder_handle = fastrtps_impl->type_factory_->create_struct_builder();
  type_builder_handle->set_name(std::string(name, name_length).c_str());
  return new rosidl_dynamic_typesupport_dynamic_type_builder_impl_t{std::move(type_builder_handle)};
}


// NOTE(methylDragon): Delegated to rosidl_dynamic_typesupport interface library
// fastrtps__dynamic_type_builder_init_from_description


rosidl_dynamic_typesupport_dynamic_type_builder_impl_t *
fastrtps__dynamic_type_builder_clone(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * other)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  DynamicTypeBuilder * type_builder_handle = fastrtps_impl->type_factory_->create_builder_copy(static_cast<const DynamicTypeBuilder *>(other->handle));
  return new rosidl_dynamic_typesupport_dynamic_type_builder_impl_t{std::move(type_builder_handle)};
}


void
fastrtps__dynamic_type_builder_fini(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  fastrtps_impl->type_factory_->delete_builder(
    static_cast<DynamicTypeBuilder *>(type_builder_impl->handle));
}


rosidl_dynamic_typesupport_dynamic_type_impl_t *
fastrtps__dynamic_type_init_from_dynamic_type_builder(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl)
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
fastrtps__dynamic_type_clone(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);

  auto type = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle));

  return new rosidl_dynamic_typesupport_dynamic_type_impl_t{
    static_cast<void *>(
      new DynamicType_ptr(
        std::move(fastrtps_impl->type_factory_->create_alias_type(type, type->get_name()))
      )
    )
  };
}


void
fastrtps__dynamic_type_fini(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl)
{
  // You typically don't need to call this because the DynamicType_ptr should manage the
  // destruction for you
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  auto type = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle));

  fastrtps_impl->type_factory_->delete_type(type.get());
}


const char *
fastrtps__dynamic_type_get_name(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl, size_t * name_length)
{
  (void)serialization_support_impl;
  auto type = eprosima::fastrtps::types::DynamicType_ptr(
    *static_cast<const eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle));
  std::string name = type->get_name();
  *name_length = name.size();
  return strdup(name.c_str());
}


const char *
fastrtps__dynamic_type_builder_get_name(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, const rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, size_t * name_length)
{
  (void)serialization_support_impl;
  std::string name = static_cast<const DynamicTypeBuilder *>(type_builder_impl->handle)->get_name();
  *name_length = name.size();
  return strdup(name.c_str());
}


void
fastrtps__dynamic_type_builder_set_name(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, const char * name, size_t name_length)
{
  (void)serialization_support_impl;
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->set_name(std::string(name, name_length).c_str());
}


// DYNAMIC TYPE PRIMITIVE MEMBERS ==================================================================
void
fastrtps__dynamic_type_builder_add_bool_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_bool_type()
  );
}


void
fastrtps__dynamic_type_builder_add_byte_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_byte_type()
  );
}


void
fastrtps__dynamic_type_builder_add_char_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_char8_type()
  );
}


void
fastrtps__dynamic_type_builder_add_wchar_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_char16_type()
  );
}


void
fastrtps__dynamic_type_builder_add_float32_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_float32_type()
  );
}


void
fastrtps__dynamic_type_builder_add_float64_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_float64_type()
  );
}


void
fastrtps__dynamic_type_builder_add_float128_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_float128_type()
  );
}


void
fastrtps__dynamic_type_builder_add_int8_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_byte_member(serialization_support_impl, type_builder_impl, id, name, name_length);
}


void
fastrtps__dynamic_type_builder_add_uint8_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_byte_member(serialization_support_impl, type_builder_impl, id, name, name_length);
}


void
fastrtps__dynamic_type_builder_add_int16_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_int16_type()
  );
}


void
fastrtps__dynamic_type_builder_add_uint16_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_uint16_type()
  );
}


void
fastrtps__dynamic_type_builder_add_int32_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_int32_type()
  );
}


void
fastrtps__dynamic_type_builder_add_uint32_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_uint32_type()
  );
}


void
fastrtps__dynamic_type_builder_add_int64_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_int64_type()
  );
}


void
fastrtps__dynamic_type_builder_add_uint64_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_uint64_type()
  );
}


void
fastrtps__dynamic_type_builder_add_string_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_bounded_string_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_wstring_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_bounded_string_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length, size_t string_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_string_type(string_bound));
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length, size_t wstring_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_wstring_type(wstring_bound));
}


// DYNAMIC TYPE STATIC ARRAY MEMBERS ===============================================================
void
fastrtps__dynamic_type_builder_add_bool_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_bool_type(), {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_byte_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_byte_type(), {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_char_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_char8_type(), {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_wchar_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_char16_type(), {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_float32_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_float32_type(), {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_float64_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_float64_type(), {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_float128_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_float128_type(), {fastrtps__size_t_to_uint32_t(array_length)})
  );
}


void
fastrtps__dynamic_type_builder_add_int8_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  fastrtps__dynamic_type_builder_add_byte_array_member(serialization_support_impl, type_builder_impl, id, name, name_length, array_length);
}


void
fastrtps__dynamic_type_builder_add_uint8_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  fastrtps__dynamic_type_builder_add_byte_array_member(serialization_support_impl, type_builder_impl, id, name, name_length, array_length);
}


void
fastrtps__dynamic_type_builder_add_int16_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_int16_type(), {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_uint16_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_uint16_type(), {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_int32_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_int32_type(), {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_uint32_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_uint32_type(), {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_int64_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_int64_type(), {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_uint64_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_uint64_type(), {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_string_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  fastrtps__dynamic_type_builder_add_bounded_string_array_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED, array_length);
}


void
fastrtps__dynamic_type_builder_add_wstring_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t array_length)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_array_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED, array_length);
}


void
fastrtps__dynamic_type_builder_add_bounded_string_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t string_bound, size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_string_type(string_bound), {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t wstring_bound, size_t array_length)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_array_builder(
      fastrtps_impl->type_factory_->create_wstring_type(wstring_bound), {fastrtps__size_t_to_uint32_t(array_length)}
    )
  );
}


// DYNAMIC TYPE UNBOUNDED SEQUENCE MEMBERS =========================================================
void
fastrtps__dynamic_type_builder_add_bool_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_bool_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_byte_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_byte_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_char_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_char_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_wchar_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_wchar_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_float32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_float32_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_float64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_float64_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_float128_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_float128_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_int8_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_int8_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}

void
fastrtps__dynamic_type_builder_add_uint8_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_uint8_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_int16_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_int16_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_uint16_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_uint16_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_int32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_int32_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_uint32_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_uint32_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_int64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_int64_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_uint64_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_uint64_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_string_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length)
{
  fastrtps__dynamic_type_builder_add_wstring_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_bounded_string_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t string_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, string_bound, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t wstring_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, wstring_bound, CONTAINER_UNLIMITED);
}


// DYNAMIC TYPE BOUNDED SEQUENCE MEMBERS ===========================================================
void
fastrtps__dynamic_type_builder_add_bool_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_bool_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_byte_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_byte_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_char_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_char8_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_wchar_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_char16_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_float32_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_float32_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_float64_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_float64_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_float128_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_float128_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_int8_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_byte_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_uint8_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_byte_bounded_sequence_member(serialization_support_impl, type_builder_impl, id, name, name_length, fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_int16_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_int16_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_uint16_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_uint16_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_int32_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_int32_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)})
  );
}


void
fastrtps__dynamic_type_builder_add_uint32_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_uint32_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_int64_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_int64_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_uint64_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_uint64_type(), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED, fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t sequence_bound)
{
  fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, CONTAINER_UNLIMITED, fastrtps__size_t_to_uint32_t(sequence_bound));
}


void
fastrtps__dynamic_type_builder_add_bounded_string_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t string_bound, size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_string_type(string_bound), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


void
fastrtps__dynamic_type_builder_add_bounded_wstring_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  size_t wstring_bound, size_t sequence_bound)
{
  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      fastrtps_impl->type_factory_->create_wstring_type(wstring_bound), {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}


// DYNAMIC TYPE NESTED MEMBERS =====================================================================
void
fastrtps__dynamic_type_builder_add_complex_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length, rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct)
{
  (void) serialization_support_impl;

  auto nested_struct_dynamictype_ptr = DynamicType_ptr(
    *static_cast<DynamicType_ptr *>(nested_struct->handle));

  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), nested_struct_dynamictype_ptr);
}


void
fastrtps__dynamic_type_builder_add_complex_array_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct, size_t array_length)
{
  auto nested_struct_dynamictype_ptr = DynamicType_ptr(
    *static_cast<DynamicType_ptr *>(nested_struct->handle));

  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id, std::string(name, name_length).c_str(), fastrtps_impl->type_factory_->create_array_builder(
      nested_struct_dynamictype_ptr, {fastrtps__size_t_to_uint32_t(array_length)}));
}


void
fastrtps__dynamic_type_builder_add_complex_unbounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct)
{
  fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member(
    serialization_support_impl, type_builder_impl, id, name, name_length, nested_struct, CONTAINER_UNLIMITED);
}


void
fastrtps__dynamic_type_builder_add_complex_bounded_sequence_member(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl, rosidl_dynamic_typesupport_member_id_t id, const char * name, size_t name_length,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * nested_struct, size_t sequence_bound)
{
  auto nested_struct_dynamictype_ptr = DynamicType_ptr(
    *static_cast<DynamicType_ptr *>(nested_struct->handle));

  auto fastrtps_impl = static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  static_cast<DynamicTypeBuilder *>(type_builder_impl->handle)->add_member(
    id,
    std::string(name, name_length).c_str(),
    fastrtps_impl->type_factory_->create_sequence_builder(
      nested_struct_dynamictype_ptr, {fastrtps__size_t_to_uint32_t(sequence_bound)}
    )
  );
}
