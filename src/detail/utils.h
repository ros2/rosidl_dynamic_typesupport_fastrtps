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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__DETAIL__UTILS_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__DETAIL__UTILS_H_

#include <rosidl_dynamic_typesupport_fastrtps/visibility_control.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif


/// Convert size_t to uint32_t, with limit checking
ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS_PUBLIC
uint32_t
fastrtps__size_t_to_uint32_t(size_t in);

/// u16string copy
ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS_PUBLIC
char16_t *
fastrtps__ucsncpy(char16_t * dest, const char16_t * src, size_t n);

/// Convert u16string to wstring
ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS_PUBLIC
std::wstring
fastrtps__u16string_to_wstring(const std::u16string & u16str);

/// Convert wstring to u16string
ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS_PUBLIC
std::u16string
fastrtps__wstring_to_u16string(const std::wstring & wstr);

/// Substring replace
ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS_PUBLIC
std::string
fastrtps__replace_string(std::string str, const std::string & from, const std::string & to);


#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__DETAIL__FASTRTPS_DYNAMIC_TYPE_H_
