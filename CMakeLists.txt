cmake_minimum_required(VERSION 3.5)
project(rosidl_dynamic_typesupport_fastrtps)
set(CMAKE_VERBOSE_MAKEFILE ON)


# COMPILER SETTINGS ================================================================================
# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)


# DEPS =============================================================================================
find_package(ament_cmake_ros REQUIRED)
find_package(rcutils REQUIRED)
find_package(rosidl_dynamic_typesupport REQUIRED)
find_package(fastrtps_cmake_module REQUIRED)
find_package(fastcdr REQUIRED CONFIG)
find_package(fastrtps 2.3 REQUIRED CONFIG)
find_package(FastRTPS 2.3 REQUIRED MODULE)


# TARGETS ==========================================================================================
add_library(${PROJECT_NAME}
  "src/detail/fastrtps_dynamic_data.cpp"
  "src/detail/fastrtps_dynamic_type.cpp"
  "src/detail/fastrtps_serialization_support.cpp"
  "src/detail/utils.cpp"

  "src/identifier.cpp"
  "src/serialization_support.cpp"
)
if(WIN32)
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE "ROSIDL_DYNAMIC_TYPESUPPORT_FASTRTPS_BUILDING_DLL")
endif()
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
)
target_link_libraries(${PROJECT_NAME}
  rosidl_dynamic_typesupport::rosidl_dynamic_typesupport
  fastcdr
  fastrtps
)


# INSTALL AND EXPORT ===============================================================================
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-export
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

ament_export_targets(${PROJECT_NAME}-export HAS_LIBRARY_TARGET)
ament_export_dependencies(rcutils)
ament_export_dependencies(rosidl_dynamic_typesupport)

ament_package()
