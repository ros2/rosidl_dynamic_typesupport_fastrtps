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

#include "macros.hpp"

#include "fastrtps_serialization_support_impl_handle.hpp"
#include <fastrtps/types/TypesBase.h>
#include <rcutils/types/rcutils_ret.h>
#include <rosidl_dynamic_typesupport/api/serialization_support_interface.h>


rcutils_ret_t
fastrtps__serialization_support_impl_handle_fini(
  rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl)
{
  auto fastrtps_serialization_support_handle =
    static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);

  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    fastrtps_serialization_support_handle->type_factory_->delete_instance(),
    "Could not delete dynamic type factory when finalizing serialization support");

  FASTRTPS_CHECK_RET_FOR_NOT_OK_AND_RETURN_WITH_MSG(
    fastrtps_serialization_support_handle->data_factory_->delete_instance(),
    "Could not delete dynamic data factory when finalizing serialization support");
}
