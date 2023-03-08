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

#ifndef ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__SERIALIZATION_SUPPORT_H_
#define ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__SERIALIZATION_SUPPORT_H_

#include <rosidl_dynamic_typesupport_fastrtps/identifier.h>
#include <rosidl_dynamic_typesupport/api/serialization_support_interface.h>

#ifdef __cplusplus
extern "C" {
#endif


/// This is the main file to include

// CORE ============================================================================================
rosidl_dynamic_typesupport_serialization_support_impl_t *
rosidl_dynamic_typesupport_fastrtps_create_serialization_support_impl();

rosidl_dynamic_typesupport_serialization_support_interface_t *
rosidl_dynamic_typesupport_fastrtps_create_serialization_support_interface();


#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS__SERIALIZATION_SUPPORT_H_
