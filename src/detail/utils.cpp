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

#include "utils.h"

#include <limits>
#include <stdexcept>
#include <string>

uint32_t
fastrtps__size_t_to_uint32_t(size_t in)
{
  if (in > std::numeric_limits<uint32_t>::max()) {
    std::logic_error("Passed size_t will overflow when narrowed to uint32_t!");
  }
  return static_cast<uint32_t>(in);
}


char16_t *
fastrtps__ucsncpy(char16_t * dest, const char16_t * src, size_t n)
{
  if (dest == NULL) {
    return NULL;
  }
  char16_t * out = dest;
  while (*src && n--) {
    *dest = *src;
    dest++;
    src++;
  }
  *dest = '\0';
  return out;
}


std::wstring
fastrtps__u16string_to_wstring(const std::u16string & u16str)
{
  std::wstring wstr;
  wstr.resize(u16str.size());
  for (size_t i = 0; i < u16str.size(); ++i) {
    wstr[i] = static_cast<wchar_t>(u16str[i]);
  }
  return wstr;
}


std::u16string
fastrtps__wstring_to_u16string(const std::wstring & wstr)
{
  std::u16string u16str;
  u16str.resize(wstr.size());
  for (size_t i = 0; i < wstr.size(); ++i) {
    u16str[i] = static_cast<wchar_t>(wstr[i]);
  }
  return u16str;
}


std::string
fastrtps__replace_string(std::string str, const std::string & from, const std::string & to)
{
  size_t pos = 0;
  while ((pos = str.find(from, pos)) != std::string::npos) {
    str.replace(pos, from.length(), to);
    pos += to.length();
  }
  return str;
}
