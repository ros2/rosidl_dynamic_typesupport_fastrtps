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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__DETAIL__UTILS_HPP_
#define ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__DETAIL__UTILS_HPP_

#include <limits>
#include <stdexcept>

inline
uint32_t
fastrtps__size_t_to_uint32_t(size_t in)
{
  if (in > std::numeric_limits<uint32_t>::max()) {
    std::logic_error("Passed size_t will overflow when narrowed to uint32_t!");
  }
  return static_cast<uint32_t>(in);
}


#endif  // ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__DETAIL__DYNAMIC_TYPE_H_
