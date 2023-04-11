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

#include "fastrtps_dynamic_data.hpp"

#include <fastdds/rtps/common/SerializedPayload.h>

#include <fastrtps/types/DynamicPubSubType.h>
#include <fastrtps/types/DynamicTypeBuilderPtr.h>

#include <string.h>

#include <rcutils/allocator.h>
#include <rcutils/strdup.h>
#include <rcutils/types/rcutils_ret.h>
#include <rcutils/types/uint8_array.h>
#include <rosidl_dynamic_typesupport/api/serialization_support_interface.h>

#include <algorithm>
#include <codecvt>
#include <cstring>
#include <cwchar>
#include <locale>
#include <memory>
#include <string>
#include <utility>

#include "macros.hpp"
#include "fastrtps_serialization_support.hpp"
#include "utils.hpp"


using eprosima::fastrtps::types::DynamicData;
using eprosima::fastrtps::types::DynamicData_ptr;

using eprosima::fastrtps::types::DynamicTypeBuilder;
using eprosima::fastrtps::types::DynamicTypeBuilder_ptr;


// =================================================================================================
// DYNAMIC DATA
// =================================================================================================

// DYNAMIC DATA UTILS ==============================================================================
rcutils_ret_t
fastrtps__dynamic_data_clear_all_values(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  (void) serialization_support_impl;
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->clear_all_values(),
    "Could not clear all values"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_clear_nonkey_values(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  (void) serialization_support_impl;
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->clear_nonkey_values(),
    "Could not clear nonkey values"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_clear_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id)
{
  (void) serialization_support_impl;
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->clear_value(
      fastrtps__size_t_to_uint32_t(id)),
    "Could not clear value"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_equals(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * other_data_impl,
  bool * equals)
{
  (void) serialization_support_impl;
  *equals = static_cast<const DynamicData *>(data_impl->handle)->equals(
    static_cast<const DynamicData *>(other_data_impl->handle));
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_item_count(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  size_t * item_count)
{
  (void) serialization_support_impl;
  *item_count = static_cast<const DynamicData *>(data_impl->handle)->get_item_count();
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_member_id_by_name(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const char * name,
  size_t name_length,
  rosidl_dynamic_typesupport_member_id_t * member_id)
{
  (void) serialization_support_impl;
  *member_id = static_cast<const DynamicData *>(data_impl->handle)->get_member_id_by_name(
    std::string(name, name_length));
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_member_id_at_index(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  size_t index,
  rosidl_dynamic_typesupport_member_id_t * member_id)
{
  (void) serialization_support_impl;
  *member_id = static_cast<const DynamicData *>(data_impl->handle)->get_member_id_at_index(
    fastrtps__size_t_to_uint32_t(index));
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_array_index(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  size_t index,
  rosidl_dynamic_typesupport_member_id_t * array_index)
{
  (void) serialization_support_impl;
  *array_index = static_cast<DynamicData *>(data_impl->handle)->get_array_index(
    {fastrtps__size_t_to_uint32_t(index)});
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_loan_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * loaned_data_impl)
{
  (void) serialization_support_impl;
  (void) allocator;

  DynamicData * loaned_data_impl_handle =
    static_cast<DynamicData *>(data_impl->handle)->loan_value(
    fastrtps__size_t_to_uint32_t(id));
  if (!loaned_data_impl_handle) {
    RCUTILS_SET_ERROR_MSG("Could not loan dynamic data");
    return RCUTILS_RET_ERROR;
  }

  loaned_data_impl->handle = std::move(loaned_data_impl_handle);
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_return_loaned_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * inner_data_impl)
{
  (void) serialization_support_impl;
  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)
    ->return_loaned_value(static_cast<const DynamicData *>(inner_data_impl->handle)),
    "Could not return loaned value"
  );
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_name(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const char ** name,
  size_t * name_length)
{
  (void) serialization_support_impl;
  std::string tmp_name = static_cast<DynamicData *>(data_impl->handle)->get_name();
  *name = rcutils_strdup(tmp_name.c_str(), rcutils_get_default_allocator());
  *name_length = tmp_name.size();
  return RCUTILS_RET_OK;
}


// DYNAMIC DATA CONSTRUCTION =======================================================================
rcutils_ret_t
fastrtps__dynamic_data_init_from_dynamic_type_builder(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_builder_impl_t * type_builder_impl,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  (void) allocator;

  auto out = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle)->data_factory_->create_data(
    static_cast<DynamicTypeBuilder *>(type_builder_impl->handle));
  if (!out) {
    RCUTILS_SET_ERROR_MSG("Could not init dynamic data from dynamic type builder");
    return RCUTILS_RET_BAD_ALLOC;
  }

  data_impl->handle = std::move(out);
  return RCUTILS_RET_OK;
}

rcutils_ret_t
fastrtps__dynamic_data_init_from_dynamic_type(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_type_impl_t * type_impl,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  (void) allocator;

  // NOTE(methylDragon): All this casting is unfortunately necessary...
  //
  //                     create_data only takes DynamicType_ptr (aka shared_ptr)
  //                     And passing a heap allocated shared_ptr is the only way to make sure the
  //                     lifetime of the dynamic type is preserved
  auto out = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle)->data_factory_->create_data(
    eprosima::fastrtps::types::DynamicType_ptr(
      *static_cast<eprosima::fastrtps::types::DynamicType_ptr *>(type_impl->handle)));
  if (!out) {
    RCUTILS_SET_ERROR_MSG("Could not init dynamic data from dynamic type");
    return RCUTILS_RET_BAD_ALLOC;
  }

  data_impl->handle = std::move(out);
  return RCUTILS_RET_OK;
}

rcutils_ret_t
fastrtps__dynamic_data_clone(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * other_data_impl,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  data_impl->allocator = *allocator;
  DynamicData * data_impl_handle = static_cast<fastrtps__serialization_support_impl_handle_t *>(
    serialization_support_impl->handle)->data_factory_->create_copy(
    static_cast<const DynamicData *>(other_data_impl->handle));
  if (!data_impl_handle) {
    RCUTILS_SET_ERROR_MSG("Could not clone struct type builder");
    return RCUTILS_RET_ERROR;
  }

  data_impl->handle = std::move(data_impl_handle);
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_fini(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle)
    ->data_factory_->delete_data(static_cast<DynamicData *>(data_impl->handle)),
    "Could not fini data"
  );
  return RCUTILS_RET_OK;
}


// DYNAMIC DATA SERIALIZATION ======================================================================

// NOTE(methylDragon): This is implemented but not tested since its not used anywhere yet...
rcutils_ret_t
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
      RCUTILS_SET_ERROR_MSG("Could not resize buffer");
      return RCUTILS_RET_BAD_ALLOC;
    }
  }
  buffer->buffer_capacity = data_length;

  auto payload = std::make_shared<eprosima::fastrtps::rtps::SerializedPayload_t>(
    fastrtps__size_t_to_uint32_t(data_length));
  bool success = m_type->serialize(data_impl->handle, payload.get());  // Serialize into payload

  if (success) {
    buffer->buffer_length = payload->length;

    // Pass ownership of serialized buffer to buffer argument
    // NOTE(methylDragon): Dubious cast... (we're going from octet to uint8_t, but it's just bytes?)
    buffer->buffer = static_cast<uint8_t *>(payload->data);
    payload->data = nullptr;

    return RCUTILS_RET_OK;
  } else {
    // We don't modify the buffer beyond expanding it up there
    // And we leave the serialized buffer in the payload to be deallocated on its destruction
    RCUTILS_SET_ERROR_MSG("Could not serialize dynamic data");
    return RCUTILS_RET_ERROR;
  }
}


rcutils_ret_t
fastrtps__dynamic_data_deserialize(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rcutils_uint8_array_t * buffer)
{
  (void) serialization_support_impl;
  auto payload = std::make_shared<eprosima::fastrtps::rtps::SerializedPayload_t>(
    fastrtps__size_t_to_uint32_t(buffer->buffer_length));

  // NOTE(methylDragon): Deserialize should copy at this point, so this copy is not needed, I think
  // memcpy(payload->data, buffer->buffer, buffer->buffer_length);

  payload->data = buffer->buffer;  // Use the input buffer directly without copying
  payload->length = fastrtps__size_t_to_uint32_t(buffer->buffer_length);

  auto m_type = std::make_shared<eprosima::fastrtps::types::DynamicPubSubType>();
  bool success = m_type->deserialize(payload.get(), data_impl->handle);

  // Deserializes payload into dynamic data. This copies!
  if (success) {
    payload->data = nullptr;  // Data gets freed on buffer fini outside
    return RCUTILS_RET_OK;
  } else {
    payload->data = nullptr;  // Data gets freed on buffer fini outside
    RCUTILS_SET_ERROR_MSG("Could not deserialize dynamic data");
    return RCUTILS_RET_ERROR;
  }
}


// DYNAMIC DATA PRIMITIVE MEMBER GETTERS ===========================================================
#define FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(FunctionT, ValueT, DataFnT) \
  rcutils_ret_t \
  fastrtps__dynamic_data_get_ ## FunctionT ## _value( \
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, \
    const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, \
    rosidl_dynamic_typesupport_member_id_t id, ValueT * value) \
  { \
    (void) serialization_support_impl; \
    FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG( \
      static_cast<const DynamicData *>(data_impl->handle)->get_ ## DataFnT ## _value( \
        *value, fastrtps__size_t_to_uint32_t(id)), \
      "Could not get `" #FunctionT "` value (of type `" #ValueT "`)" \
    ); \
  }


FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(bool, bool, bool)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(byte, unsigned char, byte)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(char, char, char8)
// FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(wchar, char16_t, char16)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(float32, float, float32)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(float64, double, float64)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(float128, long double, float128)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(int8, int8_t, int8)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(uint8, uint8_t, uint8)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(int16, int16_t, int16)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(uint16, uint16_t, uint16)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(int32, int32_t, int32)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(uint32, uint32_t, uint32)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(int64, int64_t, int64)
FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN(uint64, uint64_t, uint64)
#undef FASTRTPS_DYNAMIC_DATA_GET_VALUE_FN


// This needs something different to do the conversion out
rcutils_ret_t
fastrtps__dynamic_data_get_wchar_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char16_t * value)
{
  (void) serialization_support_impl;
  wchar_t tmp_out;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<const DynamicData *>(data_impl->handle)->get_char16_value(
      tmp_out, fastrtps__size_t_to_uint32_t(id)),
    "Could not get `wchar` value (of type `char16_t`)"
  );

  *value = static_cast<char16_t>(tmp_out);
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char ** value, size_t * value_length)
{
  (void) serialization_support_impl;
  std::string tmp_string;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<const DynamicData *>(data_impl->handle)->get_string_value(
      tmp_string, fastrtps__size_t_to_uint32_t(id)),
    "Could not get `string` value (of type `string`)"
  );

  *value_length = tmp_string.size();
  char * tmp_out = new char[*value_length + 1];  // TODO(methylDragon): Use alloc here
  memcpy(tmp_out, tmp_string.c_str(), *value_length);
  tmp_out[*value_length] = '\0';
  *value = tmp_out;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char16_t ** value, size_t * value_length)
{
  (void) serialization_support_impl;
  std::wstring tmp_wstring;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<const DynamicData *>(data_impl->handle)->get_wstring_value(
      tmp_wstring, fastrtps__size_t_to_uint32_t(id)),
    "Could not get `wstring` value (of type `char16_t *`)"
  );

  *value_length = tmp_wstring.size();
  char16_t * tmp_out = new char16_t[*value_length + 1];  // TODO(methylDragon): Use alloc here
  fastrtps__ucsncpy(tmp_out, fastrtps__wstring_to_u16string(tmp_wstring).c_str(), *value_length);
  tmp_out[*value_length] = '\0';
  *value = tmp_out;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_fixed_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char ** value, size_t * value_length,
  size_t string_length)
{
  (void) serialization_support_impl;
  std::string tmp_string;

  // On the wire it's a bounded string
  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<const DynamicData *>(data_impl->handle)->get_string_value(
      tmp_string, fastrtps__size_t_to_uint32_t(id)),
    "Could not get fixed `string` value (of type `char *`)"
  );

  size_t copy_length = std::min(tmp_string.size(), string_length);
  *value_length = string_length;
  char * tmp_out = new char[*value_length + 1]();  // TODO(methylDragon): Use alloc here
  memcpy(tmp_out, tmp_string.c_str(), copy_length);
  tmp_out[*value_length] = '\0';
  *value = tmp_out;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_fixed_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char16_t ** value, size_t * value_length,
  size_t wstring_length)
{
  (void) serialization_support_impl;
  std::wstring tmp_wstring;

  // On the wire it's a bounded string
  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<const DynamicData *>(data_impl->handle)->get_wstring_value(
      tmp_wstring, fastrtps__size_t_to_uint32_t(id)),
    "Could not get fixed `wstring` value (of type `char16_t *`)"
  );

  size_t copy_length = std::min(tmp_wstring.size(), wstring_length);
  *value_length = wstring_length;
  char16_t * tmp_out = new char16_t[*value_length + 1];  // TODO(methylDragon): Use alloc here
  fastrtps__ucsncpy(tmp_out, fastrtps__wstring_to_u16string(tmp_wstring).c_str(), copy_length);
  tmp_out[*value_length] = '\0';
  *value = tmp_out;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_bounded_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char ** value, size_t * value_length,
  size_t string_bound)
{
  (void) serialization_support_impl;
  std::string tmp_string;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<const DynamicData *>(data_impl->handle)->get_string_value(
      tmp_string, fastrtps__size_t_to_uint32_t(id)),
    "Could not get bounded `string` value (of type `char *`)"
  );

  *value_length = std::min(tmp_string.size(), string_bound);
  char * tmp_out = new char[*value_length + 1];  // TODO(methylDragon): Use alloc here
  memcpy(tmp_out, tmp_string.c_str(), *value_length);
  tmp_out[*value_length] = '\0';
  *value = tmp_out;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_get_bounded_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, char16_t ** value, size_t * value_length,
  size_t wstring_bound)
{
  (void) serialization_support_impl;
  std::wstring tmp_wstring;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<const DynamicData *>(data_impl->handle)->get_wstring_value(
      tmp_wstring, fastrtps__size_t_to_uint32_t(id)),
    "Could not get bounded `wstring` value (of type `char16_t *`)"
  );

  *value_length = std::min(tmp_wstring.size(), wstring_bound);
  char16_t * tmp_out = new char16_t[*value_length + 1];  // TODO(methylDragon): Use alloc here
  fastrtps__ucsncpy(tmp_out, fastrtps__wstring_to_u16string(tmp_wstring).c_str(), *value_length);
  tmp_out[*value_length] = '\0';
  *value = tmp_out;
  return RCUTILS_RET_OK;
}


// DYNAMIC DATA PRIMITIVE MEMBER SETTERS ===========================================================
#define FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(FunctionT, ValueT, DataFnT) \
  rcutils_ret_t \
  fastrtps__dynamic_data_set_ ## FunctionT ## _value( \
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, \
    rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, \
    rosidl_dynamic_typesupport_member_id_t id, ValueT value) \
  { \
    (void) serialization_support_impl; \
    FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG( \
      static_cast<DynamicData *>(data_impl->handle)->set_ ## DataFnT ## _value( \
        value, fastrtps__size_t_to_uint32_t(id)), \
      "Could not set `" #FunctionT "` value (of type `" #ValueT "`)" \
    ); \
  }

FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(bool, bool, bool)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(byte, unsigned char, byte)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(char, char, char8)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(wchar, char16_t, char16)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(float32, float, float32)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(float64, double, float64)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(float128, long double, float128)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(int8, int8_t, int8)  // NOTE!!
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(uint8, uint8_t, uint8)  // NOTE!!
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(int16, int16_t, int16)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(uint16, uint16_t, uint16)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(int32, int32_t, int32)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(uint32, uint32_t, uint32)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(int64, int64_t, int64)
FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN(uint64, uint64_t, uint64)
#undef FASTRTPS_DYNAMIC_DATA_SET_VALUE_FN


rcutils_ret_t
fastrtps__dynamic_data_set_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * value, size_t value_length)
{
  (void) serialization_support_impl;
  const std::string tmp_string(value, value_length);
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->set_string_value(
      tmp_string, fastrtps__size_t_to_uint32_t(id)),
    "Could not set `string` value (of type `char *`)"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_set_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char16_t * value, size_t value_length)
{
  (void) serialization_support_impl;
  const std::u16string tmp_u16string(value, value_length);
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->set_wstring_value(
      fastrtps__u16string_to_wstring(tmp_u16string), fastrtps__size_t_to_uint32_t(id)),
    "Could not set `wstring` value (of type `char16_t *`)"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_set_fixed_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * value, size_t value_length,
  size_t string_length)
{
  (void) serialization_support_impl;
  std::string tmp_string(value, std::min(value_length, string_length));
  tmp_string.resize(string_length, '\0');
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->set_string_value(
      tmp_string, fastrtps__size_t_to_uint32_t(id)),
    "Could not set fixed `string` value (of type `char *`)"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_set_fixed_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char16_t * value, size_t value_length,
  size_t wstring_length)
{
  (void) serialization_support_impl;
  std::u16string tmp_u16string(value, std::min(value_length, wstring_length));
  tmp_u16string.resize(wstring_length, '\0');
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->set_wstring_value(
      fastrtps__u16string_to_wstring(tmp_u16string), fastrtps__size_t_to_uint32_t(id)),
    "Could not set fixed `wstring` value (of type `char16_t *`)"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_set_bounded_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char * value, size_t value_length,
  size_t string_bound)
{
  (void) serialization_support_impl;
  const std::string tmp_string(value, std::min(value_length, string_bound));
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->set_string_value(
      tmp_string, fastrtps__size_t_to_uint32_t(id)),
    "Could not set bounded `string` value (of type `char *`)"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_set_bounded_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, const char16_t * value, size_t value_length,
  size_t wstring_bound)
{
  (void) serialization_support_impl;
  const std::u16string tmp_u16string(value, std::min(value_length, wstring_bound));
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->set_wstring_value(
      fastrtps__u16string_to_wstring(tmp_u16string), fastrtps__size_t_to_uint32_t(id)),
    "Could not set bounded `wstring` value (of type `char16_t *`)"
  );
}


// DYNAMIC DATA SEQUENCES ==========================================================================
rcutils_ret_t
fastrtps__dynamic_data_clear_sequence_data(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl)
{
  (void) serialization_support_impl;
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->clear_data(),
    "Could not clear sequence data"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_remove_sequence_data(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id)
{
  (void) serialization_support_impl;
  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->remove_sequence_data(
      fastrtps__size_t_to_uint32_t(id)),
    "Could not remove sequence data"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_insert_sequence_data(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->insert_sequence_data(tmp_id),
    "Could not insert sequence data"
  );
  *out_id = tmp_id;
  return RCUTILS_RET_OK;
}


#define FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(FunctionT, ValueT, DataFnT) \
  rcutils_ret_t \
  fastrtps__dynamic_data_insert_ ## FunctionT ## _value( \
    rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl, \
    rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, ValueT value, \
    rosidl_dynamic_typesupport_member_id_t * out_id) \
  { \
    (void) serialization_support_impl; \
    eprosima::fastrtps::types::MemberId tmp_id; \
 \
    FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG( \
      static_cast<DynamicData *>(data_impl->handle)->insert_ ## DataFnT ## _value(value, tmp_id), \
      "Could not insert `" #FunctionT "` value (of type `" #ValueT "`)" \
    ); \
    *out_id = tmp_id; \
    return RCUTILS_RET_OK; \
  }

FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(bool, bool, bool)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(byte, unsigned char, byte)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(char, char, char8)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(wchar, char16_t, char16)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(float32, float, float32)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(float64, double, float64)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(float128, long double, float128)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(int8, int8_t, char8)  // There is no int8 method
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(uint8, uint8_t, byte)  // There is no uint8 method
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(int16, int16_t, int16)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(uint16, uint16_t, uint16)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(int32, int32_t, int32)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(uint32, uint32_t, uint32)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(int64, int64_t, int64)
FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN(uint64, uint64_t, uint64)
#undef FASTRTPS_DYNAMIC_DATA_INSERT_VALUE_FN


rcutils_ret_t
fastrtps__dynamic_data_insert_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, const char * value,
  size_t value_length, rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->insert_string_value(
      std::string(value, value_length), tmp_id),
    "Could not insert `string` value (of type `char *`)"
  );
  *out_id = tmp_id;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_insert_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const char16_t * value,
  size_t value_length,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->insert_wstring_value(
      fastrtps__u16string_to_wstring(std::u16string(value, value_length)), tmp_id),
    "Could not insert `wstring` value (of type `char16_t *`)"
  );
  *out_id = tmp_id;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_insert_fixed_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, const char * value,
  size_t value_length, size_t string_length, rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;
  std::string tmp_string = std::string(value, std::min(value_length, string_length));
  tmp_string.resize(string_length, '\0');

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->insert_string_value(tmp_string, tmp_id),
    "Could not insert fixed `string` value (of type `char *`)"
  );
  *out_id = tmp_id;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
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

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->insert_wstring_value(
      fastrtps__u16string_to_wstring(tmp_ustring), tmp_id),
    "Could not insert fixed `wstring` value (of type `char16_t *`)"
  );
  *out_id = tmp_id;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_insert_bounded_string_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, const char * value,
  size_t value_length, size_t string_bound, rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->insert_string_value(
      std::string(value, std::min(value_length, string_bound)), tmp_id),
    "Could not insert bounded `string` value (of type `char *`)"
  );
  *out_id = tmp_id;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_insert_bounded_wstring_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl, const char16_t * value,
  size_t value_length, size_t wstring_bound,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->insert_wstring_value(
      fastrtps__u16string_to_wstring(std::u16string(value, std::min(value_length, wstring_bound))),
      tmp_id),
    "Could not insert bounded `wstring` value (of type `char16_t *`)"
  );
  *out_id = tmp_id;
  return RCUTILS_RET_OK;
}


// DYNAMIC DATA NESTED =============================================================================
rcutils_ret_t
fastrtps__dynamic_data_get_complex_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id,
  rcutils_allocator_t * allocator,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * value)
{
  (void) serialization_support_impl;
  (void) allocator;

  auto tmp_data = static_cast<DynamicData *>(value->handle);

  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<const DynamicData *>(data_impl->handle)->get_complex_value(
      &tmp_data, fastrtps__size_t_to_uint32_t(id)),
    "Could not get complex value"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_set_complex_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_member_id_t id, rosidl_dynamic_typesupport_dynamic_data_impl_t * value)
{
  (void) serialization_support_impl;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->set_complex_value(
      static_cast<DynamicData *>(value->handle), fastrtps__size_t_to_uint32_t(id)),
    "Could not set complex value"
  );
}


rcutils_ret_t
fastrtps__dynamic_data_insert_complex_value_copy(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  const rosidl_dynamic_typesupport_dynamic_data_impl_t * value,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->insert_complex_value(
      static_cast<const DynamicData *>(value->handle), tmp_id),
    "Could not insert complex value copy"
  );
  *out_id = tmp_id;
  return RCUTILS_RET_OK;
}


rcutils_ret_t
fastrtps__dynamic_data_insert_complex_value(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * data_impl,
  rosidl_dynamic_typesupport_dynamic_data_impl_t * value,
  rosidl_dynamic_typesupport_member_id_t * out_id)
{
  (void) serialization_support_impl;
  eprosima::fastrtps::types::MemberId tmp_id;

  FASTRTPS_CHECK_RET_FOR_NOT_OK_WITH_MSG(
    static_cast<DynamicData *>(data_impl->handle)->insert_complex_value(
      static_cast<DynamicData *>(value->handle), tmp_id),
    "Could not insert complex value"
  );
  *out_id = tmp_id;
  return RCUTILS_RET_OK;
}
