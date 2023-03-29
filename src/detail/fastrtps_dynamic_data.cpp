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

#include <algorithm>
#include <codecvt>
#include <cstring>
#include <cwchar>
#include <locale>

#include <fastdds/rtps/common/SerializedPayload.h>

#include <fastrtps/types/DynamicDataHelper.hpp>
#include <fastrtps/types/DynamicPubSubType.h>
#include <fastrtps/types/DynamicTypeBuilderPtr.h>

#include "fastrtps_dynamic_data.h"
#include "fastrtps_serialization_support_impl_handle.h"
#include "utils.hpp"

#include <rcutils/types/rcutils_ret.h>
#include <rcutils/types/uint8_array.h>
#include <rosidl_dynamic_typesupport/api/serialization_support_interface.h>


using eprosima::fastrtps::types::DynamicData;
using eprosima::fastrtps::types::DynamicData_ptr;
using eprosima::fastrtps::types::DynamicDataHelper;

using eprosima::fastrtps::types::DynamicTypeBuilder;
using eprosima::fastrtps::types::DynamicTypeBuilder_ptr;


// =================================================================================================
// DYNAMIC DATA
// =================================================================================================

// DYNAMIC DATA UTILS ==============================================================================
void
fastrtps__dynamic_data_clear_all_values(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  (void) serialization_support_impl;
  static_cast<DynamicData *>(data_impl->handle)->clear_all_values();
}


void
fastrtps__dynamic_data_clear_nonkey_values(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  (void) serialization_support_impl;
  static_cast<DynamicData *>(data_impl->handle)->clear_nonkey_values();
}


void
fastrtps__dynamic_data_clear_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id)
{
  (void) serialization_support_impl;
  static_cast<DynamicData *>(data_impl->handle)->clear_value(id);
}


bool
fastrtps__dynamic_data_equals(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * other_data_impl)
{
  (void) serialization_support_impl;
  return static_cast<const DynamicData *>(data_impl->handle)->equals(
    static_cast<const DynamicData *>(other_data_impl->handle));
}


size_t
fastrtps__dynamic_data_get_item_count(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  (void) serialization_support_impl;
  return static_cast<const DynamicData *>(data_impl->handle)->get_item_count();
}


rosidl_dynamic_typesupport_member_id_t
fastrtps__dynamic_data_get_member_id_by_name(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const char * name,
  size_t name_length)
{
  (void) serialization_support_impl;
  return static_cast<const DynamicData *>(data_impl->handle)->get_member_id_by_name(
    std::string(name, name_length));
}


rosidl_dynamic_typesupport_member_id_t
fastrtps__dynamic_data_get_member_id_at_index(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  size_t index)
{
  (void) serialization_support_impl;
  return static_cast<const DynamicData *>(data_impl->handle)->get_member_id_at_index(
    fastrtps__size_t_to_uint32_t(index));
}


rosidl_dynamic_typesupport_member_id_t
fastrtps__dynamic_data_get_array_index(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  size_t index)
{
  (void) serialization_support_impl;
  return static_cast<DynamicData *>(data_impl->handle)->get_array_index(
    {fastrtps__size_t_to_uint32_t(index)});
}


rosidl_dynamic_typesupport_dynamic_data_impl_t *
fastrtps__dynamic_data_loan_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id)
{
  (void) serialization_support_impl;
  return new rosidl_dynamic_typesupport_dynamic_data_impl_t{std::move(
      static_cast<DynamicData *>(data_impl->handle)->loan_value(id))};
}


void
fastrtps__dynamic_data_return_loaned_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * inner_data_impl)
{
  (void) serialization_support_impl;
  static_cast<DynamicData *>(data_impl->handle)
    ->return_loaned_value(static_cast<const DynamicData *>(inner_data_impl->handle));
}


void
fastrtps__dynamic_data_print(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  (void) serialization_support_impl;
  DynamicDataHelper::print(static_cast<const DynamicData *>(data_impl->handle));
}


const char *
fastrtps__dynamic_data_get_name(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, size_t * name_length)
{
  (void) serialization_support_impl;
  std::string name = static_cast<DynamicData *>(data_impl->handle)->get_name();
  *name_length = name.size();
  return strdup(name.c_str());
}


// DYNAMIC DATA CONSTRUCTION =======================================================================
rosidl_dynamic_typesupport_dynamic_data_impl_t *
fastrtps__dynamic_data_init_from_dynamic_type_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl)
{
  return new rosidl_dynamic_typesupport_dynamic_data_impl_t{
    static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle)
    ->data_factory_->create_data(static_cast<DynamicTypeBuilder *>(type_builder_impl->handle))
  };
}

rosidl_dynamic_typesupport_dynamic_data_impl_t *
fastrtps__dynamic_data_init_from_dynamic_type(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl)
{
  // NOTE(methylDragon): All this casting is unfortunately necessary...
  //
  //                     create_data only takes DynamicType_ptr (aka shared_ptr)
  //                     And passing a heap allocated shared_ptr is the only way to make sure the
  //                     lifetime of the dynamic type is preserved
  return new rosidl_dynamic_typesupport_dynamic_data_impl_t{
    static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle)
    ->data_factory_->create_data(
      eprosima::fastrtps::types::DynamicType_ptr(
        *static_cast<eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle)
      )
    )
  };
}

rosidl_dynamic_typesupport_dynamic_data_impl_t *
fastrtps__dynamic_data_clone(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  return new rosidl_dynamic_typesupport_dynamic_data_impl_t{
    static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle)
    ->data_factory_->create_copy(
      static_cast<const DynamicData *>(data_impl->handle))
  };
}


void
fastrtps__dynamic_data_fini(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle)
  ->data_factory_->delete_data(static_cast<DynamicData *>(data_impl->handle));
}


// DYNAMIC DATA SERIALIZATION ======================================================================

// NOTE(methylDragon): This is implemented but not tested since its not used anywhere yet...
bool
fastrtps__dynamic_data_serialize(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rcutils_uint8_array_t * buffer)
{
  (void) serialization_support_impl;
  auto m_type = std::make_shared<eprosima::fastrtps::types::DynamicPubSubType>();
  size_t data_length = static_cast<size_t>(
    m_type->getSerializedSizeProvider(static_cast<DynamicData *>(data_impl->handle))());

  if (buffer->buffer_capacity < data_length) {
    if (rcutils_uint8_array_resize(buffer, data_length) != RCUTILS_RET_OK) {
      // NOTE(methylDragon): Warning here??
      return false;
    }
  }
  buffer->buffer_capacity = data_length;

  auto payload = std::make_shared<eprosima::fastrtps::rtps::SerializedPayload_t>(data_length);
  bool ret = m_type->serialize(data_impl->handle, payload.get());  // Serialize into payload

  if (ret) {
    buffer->buffer_length = payload->length;

    // Pass ownership of serialized buffer to buffer argument
    // NOTE(methylDragon): Dubious cast... (we're going from octet to uint8_t, but it's just bytes?)
    buffer->buffer = static_cast<uint8_t *>(payload->data);
    payload->data = nullptr;

    return true;
  } else {
    // We don't modify the buffer beyond expanding it up there
    // And we leave the serialized buffer in the payload to be deallocated on its destruction
    return false;
  }
}


bool
fastrtps__dynamic_data_deserialize(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rcutils_uint8_array_t * buffer)
{
  (void) serialization_support_impl;
  auto payload = std::make_shared<eprosima::fastrtps::rtps::SerializedPayload_t>(
    buffer->buffer_length);

  // NOTE(methylDragon): Deserialize should copy at this point, so this copy is not needed, I think
  // memcpy(payload->data, buffer->buffer, buffer->buffer_length);

  payload->data = buffer->buffer;  // Use the input buffer directly without copying
  payload->length = buffer->buffer_length;

  auto m_type = std::make_shared<eprosima::fastrtps::types::DynamicPubSubType>();

  // Deserializes payload into dynamic data. This copies!
  if (m_type->deserialize(payload.get(), data_impl->handle)) {
    payload->data = nullptr;  // Data gets freed on buffer fini outside
    return true;
  } else {
    payload->data = nullptr;  // Data gets freed on buffer fini outside
    // NOTE(methylDragon): Should I log something here???
    return false;
  }
}


// DYNAMIC DATA PRIMITIVE MEMBER GETTERS ===========================================================
#define FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(FunctionT, ValueT, DataFnT) \
  void \
  fastrtps__dynamic_data_get_ ## FunctionT ## _value( \
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, \
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, \
    rosidl_dynamic_typesupport_member_id_t id, ValueT * value) \
  { \
    (void) serialization_support_impl; \
    static_cast<const DynamicData *>(data_impl->handle)->get_ ## DataFnT ## _value(*value, id); \
  }


FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(bool, bool, bool);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(byte, unsigned char, byte);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(char, char, char8);
// FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(wchar, char16_t, char16);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(float32, float, float32);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(float64, double, float64);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(float128, long double, float128);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(int8, int8_t, int8);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(uint8, uint8_t, uint8);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(int16, int16_t, int16);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(uint16, uint16_t, uint16);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(int32, int32_t, int32);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(uint32, uint32_t, uint32);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(int64, int64_t, int64);
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(uint64, uint64_t, uint64);
#undef FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN


// This needs something different to do the conversion out
void
fastrtps__dynamic_data_get_wchar_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char16_t * value)
{
  (void) serialization_support_impl;
  wchar_t out;
  static_cast<const DynamicData *>(data_impl->handle)->get_char16_value(out, id);
  *value = static_cast<char16_t>(out);
}


void
fastrtps__dynamic_data_get_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char ** value, size_t * value_length)
{
  (void) serialization_support_impl;
  std::string tmp_string;
  static_cast<const DynamicData *>(data_impl->handle)->get_string_value(tmp_string, id);

  *value_length = tmp_string.size();
  char * out = new char[*value_length + 1];
  strncpy(out, tmp_string.c_str(), *value_length);
  out[*value_length] = '\0';
  *value = out;
}


void
fastrtps__dynamic_data_get_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char16_t ** value, size_t * value_length)
{
  (void) serialization_support_impl;
  std::wstring tmp_wstring;
  static_cast<const DynamicData *>(data_impl->handle)->get_wstring_value(tmp_wstring, id);

  *value_length = tmp_wstring.size();
  char16_t * out = new char16_t[*value_length + 1];
  fastrtps__ucsncpy(out, fastrtps__wstring_to_u16string(tmp_wstring).c_str(), *value_length);
  out[*value_length] = '\0';
  *value = out;
}


void
fastrtps__dynamic_data_get_fixed_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char ** value, size_t * value_length,
  size_t string_length)
{
  (void) serialization_support_impl;
  std::string tmp_string;
  // On the wire it's a bounded string
  static_cast<const DynamicData *>(data_impl->handle)->get_string_value(tmp_string, id);

  size_t copy_length = std::min(tmp_string.size(), string_length);
  *value_length = string_length;
  char * out = new char[*value_length + 1]();
  strncpy(out, tmp_string.c_str(), copy_length);
  out[*value_length] = '\0';
  *value = out;
}


void
fastrtps__dynamic_data_get_fixed_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char16_t ** value, size_t * value_length,
  size_t wstring_length)
{
  (void) serialization_support_impl;
  std::wstring tmp_wstring;
  // On the wire it's a bounded string
  static_cast<const DynamicData *>(data_impl->handle)->get_wstring_value(tmp_wstring, id);

  size_t copy_length = std::min(tmp_wstring.size(), wstring_length);
  *value_length = wstring_length;
  char16_t * out = new char16_t[*value_length + 1];
  fastrtps__ucsncpy(out, fastrtps__wstring_to_u16string(tmp_wstring).c_str(), copy_length);
  out[*value_length] = '\0';
  *value = out;
}


void
fastrtps__dynamic_data_get_bounded_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char ** value, size_t * value_length,
  size_t string_bound)
{
  (void) serialization_support_impl;
  std::string tmp_string;
  static_cast<const DynamicData *>(data_impl->handle)->get_string_value(tmp_string, id);

  *value_length = std::min(tmp_string.size(), string_bound);
  char * out = new char[*value_length + 1];
  strncpy(out, tmp_string.c_str(), *value_length);
  out[*value_length] = '\0';
  *value = out;
}


void
fastrtps__dynamic_data_get_bounded_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char16_t ** value, size_t * value_length,
  size_t wstring_bound)
{
  (void) serialization_support_impl;
  std::wstring tmp_wstring;
  static_cast<const DynamicData *>(data_impl->handle)->get_wstring_value(tmp_wstring, id);

  *value_length = std::min(tmp_wstring.size(), wstring_bound);
  char16_t * out = new char16_t[*value_length + 1];
  fastrtps__ucsncpy(out, fastrtps__wstring_to_u16string(tmp_wstring).c_str(), *value_length);
  out[*value_length] = '\0';
  *value = out;
}


// DYNAMIC DATA PRIMITIVE MEMBER SETTERS ===========================================================
#define FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(FunctionT, ValueT, DataFnT) \
  void \
  fastrtps__dynamic_data_set_ ## FunctionT ## _value( \
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, \
    rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, \
    rosidl_dynamic_typesupport_member_id_t id, ValueT value) \
  { \
    (void) serialization_support_impl; \
    static_cast<DynamicData *>(data_impl->handle)->set_ ## DataFnT ## _value(value, id); \
  }

FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(bool, bool, bool);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(byte, unsigned char, byte);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(char, char, char8);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(wchar, char16_t, char16);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(float32, float, float32);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(float64, double, float64);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(float128, long double, float128);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(int8, int8_t, int8);  // NOTE!!
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(uint8, uint8_t, uint8);  // NOTE!!
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(int16, int16_t, int16);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(uint16, uint16_t, uint16);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(int32, int32_t, int32);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(uint32, uint32_t, uint32);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(int64, int64_t, int64);
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(uint64, uint64_t, uint64);
#undef FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN


void
fastrtps__dynamic_data_set_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * value, size_t value_length)
{
  (void) serialization_support_impl;
  const std::string tmp_string(value, value_length);
  static_cast<DynamicData *>(data_impl->handle)->set_string_value(tmp_string, id);
}


void
fastrtps__dynamic_data_set_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char16_t * value, size_t value_length)
{
  (void) serialization_support_impl;
  const std::u16string tmp_u16string(value, value_length);
  static_cast<DynamicData *>(data_impl->handle)->set_wstring_value(
    fastrtps__u16string_to_wstring(tmp_u16string), id);
}


void
fastrtps__dynamic_data_set_fixed_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * value, size_t value_length,
  size_t string_length)
{
  (void) serialization_support_impl;
  std::string tmp_string(value, std::min(value_length, string_length));
  tmp_string.resize(string_length, '\0');
  static_cast<DynamicData *>(data_impl->handle)->set_string_value(tmp_string, id);
}


void
fastrtps__dynamic_data_set_fixed_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char16_t * value, size_t value_length,
  size_t wstring_length)
{
  (void) serialization_support_impl;
  std::u16string tmp_u16string(value, std::min(value_length, wstring_length));
  tmp_u16string.resize(wstring_length, '\0');
  static_cast<DynamicData *>(data_impl->handle)->set_wstring_value(
    fastrtps__u16string_to_wstring(tmp_u16string), id);
}


void
fastrtps__dynamic_data_set_bounded_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * value, size_t value_length,
  size_t string_bound)
{
  (void) serialization_support_impl;
  const std::string tmp_string(value, std::min(value_length, string_bound));
  static_cast<DynamicData *>(data_impl->handle)->set_string_value(tmp_string, id);
}


void
fastrtps__dynamic_data_set_bounded_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char16_t * value, size_t value_length,
  size_t wstring_bound)
{
  (void) serialization_support_impl;
  const std::u16string tmp_u16string(value, std::min(value_length, wstring_bound));
  static_cast<DynamicData *>(data_impl->handle)->set_wstring_value(
    fastrtps__u16string_to_wstring(tmp_u16string), id);
}


// DYNAMIC DATA SEQUENCES ==========================================================================
void
fastrtps__dynamic_data_clear_sequence_data(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  (void) serialization_support_impl;
  static_cast<DynamicData *>(data_impl->handle)->clear_data();
}


void
fastrtps__dynamic_data_remove_sequence_data(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id)
{
  (void) serialization_support_impl;
  static_cast<DynamicData *>(data_impl->handle)->remove_sequence_data(id);
}


void
fastrtps__dynamic_data_insert_sequence_data(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  static_cast<DynamicData *>(data_impl->handle)->insert_sequence_data(tmp_id);
  *out_id = tmp_id;
}


#define FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(FunctionT, ValueT, DataFnT) \
  void \
  fastrtps__dynamic_data_insert_ ## FunctionT ## _value( \
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, \
    rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, ValueT value, \
    rosidl_dynamic_typesupport_member_id_t * out_id) \
  { \
    (void) serialization_support_impl; \
    eprosima::fastrtps::types::MemberId tmp_id; \
    static_cast<DynamicData *>(data_impl->handle)->insert_ ## DataFnT ## _value(value, tmp_id); \
    *out_id = tmp_id; \
  }

FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(bool, bool, bool);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(byte, unsigned char, byte);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(char, char, char8);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(wchar, char16_t, char16);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(float32, float, float32);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(float64, double, float64);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(float128, long double, float128);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(int8, int8_t, char8);  // There is no int8 method
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(uint8, uint8_t, byte);  // There is no uint8 method
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(int16, int16_t, int16);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(uint16, uint16_t, uint16);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(int32, int32_t, int32);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(uint32, uint32_t, uint32);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(int64, int64_t, int64);
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(uint64, uint64_t, uint64);
#undef FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN


void
fastrtps__dynamic_data_insert_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, const char * value,
  size_t value_length, rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  static_cast<DynamicData *>(data_impl->handle)->insert_string_value(
    std::string(value, value_length), tmp_id);
  *out_id = tmp_id;
}


void
fastrtps__dynamic_data_insert_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const char16_t * value,
  size_t value_length,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  static_cast<DynamicData *>(data_impl->handle)->insert_wstring_value(
    fastrtps__u16string_to_wstring(std::u16string(value, value_length)), tmp_id);
  *out_id = tmp_id;
}


void
fastrtps__dynamic_data_insert_fixed_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, const char * value,
  size_t value_length, size_t string_length, rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  std::string tmp_string = std::string(value, std::min(value_length, string_length));
  tmp_string.resize(string_length, '\0');
  static_cast<DynamicData *>(data_impl->handle)->insert_string_value(
    tmp_string, tmp_id);
  *out_id = tmp_id;
}


void
fastrtps__dynamic_data_insert_fixed_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, const char16_t * value,
  size_t value_length, size_t wstring_length,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  std::u16string tmp_ustring = std::u16string(value, std::min(value_length, wstring_length));
  tmp_ustring.resize(wstring_length, '\0');
  static_cast<DynamicData *>(data_impl->handle)->insert_wstring_value(
    fastrtps__u16string_to_wstring(tmp_ustring), tmp_id);
  *out_id = tmp_id;
}


void
fastrtps__dynamic_data_insert_bounded_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, const char * value,
  size_t value_length, size_t string_bound, rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  static_cast<DynamicData *>(data_impl->handle)->insert_string_value(
    std::string(value, std::min(value_length, string_bound)), tmp_id);
  *out_id = tmp_id;
}


void
fastrtps__dynamic_data_insert_bounded_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, const char16_t * value,
  size_t value_length, size_t wstring_bound,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  static_cast<DynamicData *>(data_impl->handle)->insert_wstring_value(
    fastrtps__u16string_to_wstring(std::u16string(value, std::min(value_length, wstring_bound))),
    tmp_id);
  *out_id = tmp_id;
}


// DYNAMIC DATA NESTED =============================================================================
void
fastrtps__dynamic_data_get_complex_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  rosidl_dynamic_typesupport_dynamic_data_impl_t ** value)
{
  (void) serialization_support_impl;
  auto tmp_data = static_cast<DynamicData *>((*value)->handle);
  static_cast<const DynamicData *>(data_impl->handle)->get_complex_value(&tmp_data, id);
}


void
fastrtps__dynamic_data_set_complex_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, rosidl_dynamic_typesupport_dynamic_data_impl_t * value)
{
  (void) serialization_support_impl;
  static_cast<DynamicData *>(data_impl->handle)->set_complex_value(
    static_cast<DynamicData *>(value->handle), id);
}


void
fastrtps__dynamic_data_insert_complex_value_copy(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * value,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  static_cast<DynamicData *>(data_impl->handle)->insert_complex_value(
    static_cast<const DynamicData *>(value->handle), tmp_id);
  *out_id = tmp_id;
}


void
fastrtps__dynamic_data_insert_complex_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * value,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  static_cast<DynamicData *>(data_impl->handle)->insert_complex_value(
    static_cast<DynamicData *>(value->handle), tmp_id);
  *out_id = tmp_id;
}
