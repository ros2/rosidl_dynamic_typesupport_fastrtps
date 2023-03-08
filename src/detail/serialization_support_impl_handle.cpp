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

#include <rosidl_dynamic_typesupport/api/serialization_support_interface.h>
#include "serialization_support_impl_handle.h"


void
fastrtps__serialization_support_impl_handle_fini(rosidl_dynamic_typesupport_serialization_support_impl_t * serialization_support_impl)
{
  auto fastrtps_serialization_support_handle =
    static_cast<fastrtps__serialization_support_impl_handle_t *>(serialization_support_impl->handle);
  fastrtps_serialization_support_handle->type_factory_->delete_instance();
  fastrtps_serialization_support_handle->data_factory_->delete_instance();
}
