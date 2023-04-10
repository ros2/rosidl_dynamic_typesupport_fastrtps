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

#ifndef DETAIL__FASTRTPS_SERIALIZATION_SUPPORT_HPP_
#define DETAIL__FASTRTPS_SERIALIZATION_SUPPORT_HPP_

#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/DynamicTypeBuilderFactory.h>

#include <rcutils/types/rcutils_ret.h>
#include <rosidl_dynamic_typesupport_fastrtps/visibility_control.h>
#include <rosidl_dynamic_typesupport/api/serialization_support.h>
#include <rosidl_dynamic_typesupport/api/serialization_support_interface.h>


// CORE ============================================================================================
typedef struct fastrtps__serialization_support_impl_handle_s
{
  eprosima::fastrtps::types::DynamicTypeBuilderFactory * type_factory_;
  eprosima::fastrtps::types::DynamicDataFactory * data_factory_;
} fastrtps__serialization_support_impl_handle_t;

ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS_PUBLIC
rcutils_ret_t
fastrtps__serialization_support_impl_fini(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl);

ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS_PUBLIC
rcutils_ret_t
fastrtps__serialization_support_interface_fini(
  rosidl_dynamic_typesupport_serialization_support_interface_t * serialization_support_interface);

#endif  // DETAIL__FASTRTPS_SERIALIZATION_SUPPORT_HPP_
