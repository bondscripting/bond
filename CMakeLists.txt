cmake_minimum_required(VERSION 3.16)
project(Bond LANGUAGES CXX)

option(BOND_BUILD_TOOLS "Build Bond command line tools" ON)
option(BOND_BUILD_TEST_FRAMEWORK "Build Bond test framework library" ON)
option(BOND_BUILD_TESTS "Build Bond unit tests" ON)
option(BOND_BUILD_UTILS "Build Bond utility programs" ON)
option(BOND_ENABLE_DEFAULT_WARNINGS "Enable default warning flags for GCC/Clang" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")
set(TOOLS_DIR "${CMAKE_SOURCE_DIR}/tools")
set(UTIL_DIR "${CMAKE_SOURCE_DIR}/util")

file(GLOB BOND_SOURCES "${SOURCE_DIR}/*.cpp")
file(GLOB TOOL_SOURCES "${TOOLS_DIR}/*.cpp")
file(GLOB UTIL_SOURCES "${UTIL_DIR}/*.cpp")
file(GLOB TEST_SOURCES "${TEST_DIR}/*.cpp")
file(GLOB TEST_FRAMEWORK_SOURCES "${TEST_DIR}/framework/*.cpp")

add_library(bond_lib STATIC ${BOND_SOURCES})
target_include_directories(bond_lib PUBLIC ${INCLUDE_DIR} ${SOURCE_DIR})
set_target_properties(bond_lib PROPERTIES
  OUTPUT_NAME bond
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if(BOND_ENABLE_DEFAULT_WARNINGS)
  if(MSVC)
    target_compile_options(bond_lib PRIVATE /W4 /permissive-)
  else()
    target_compile_options(bond_lib PRIVATE
      -Wall -Wextra -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization
      -Winit-self -Wold-style-cast -Wredundant-decls -Wshadow -Wsign-promo
      -Wstrict-null-sentinel -Wundef -Wno-unused
    )
  endif()
endif()

set(TOOLS_TARGETS)
if(BOND_BUILD_TOOLS)
  foreach(tool_src IN LISTS TOOL_SOURCES)
    get_filename_component(tool_name ${tool_src} NAME_WE)
    add_executable(${tool_name} ${tool_src})
    target_include_directories(${tool_name} PRIVATE ${INCLUDE_DIR} ${SOURCE_DIR})
    target_link_libraries(${tool_name} PRIVATE bond_lib)
    set_target_properties(${tool_name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
      EXCLUDE_FROM_ALL TRUE
    )
    list(APPEND TOOLS_TARGETS ${tool_name})
  endforeach()
  add_custom_target(tools DEPENDS ${TOOLS_TARGETS})
endif()

set(FRAMEWORK_TARGETS)
if(BOND_BUILD_TEST_FRAMEWORK)
  add_library(framework_lib STATIC ${TEST_FRAMEWORK_SOURCES})
  target_include_directories(framework_lib PUBLIC ${INCLUDE_DIR} ${TEST_DIR} "${TEST_DIR}/framework")
  set_target_properties(framework_lib PROPERTIES
    OUTPUT_NAME framework
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    EXCLUDE_FROM_ALL TRUE
  )
  if(BOND_ENABLE_DEFAULT_WARNINGS)
    if(MSVC)
      target_compile_options(framework_lib PRIVATE /W4 /permissive-)
    else()
      target_compile_options(framework_lib PRIVATE
        -Wall -Wextra -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization
        -Winit-self -Wold-style-cast -Wredundant-decls -Wshadow -Wsign-promo
        -Wstrict-null-sentinel -Wundef -Wno-unused
      )
    endif()
  endif()
  list(APPEND FRAMEWORK_TARGETS framework_lib)
  add_custom_target(framework DEPENDS framework_lib)
endif()

set(TEST_TARGETS)
if(BOND_BUILD_TESTS)
  foreach(test_src IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_include_directories(${test_name} PRIVATE ${INCLUDE_DIR} ${TEST_DIR})
    target_link_libraries(${test_name} PRIVATE bond_lib framework_lib)
    set_target_properties(${test_name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/testbin"
      EXCLUDE_FROM_ALL TRUE
    )
    list(APPEND TEST_TARGETS ${test_name})
  endforeach()
  add_custom_target(test DEPENDS ${TEST_TARGETS})
endif()

set(UTIL_TARGETS)
if(BOND_BUILD_UTILS)
  foreach(util_src IN LISTS UTIL_SOURCES)
    get_filename_component(util_name ${util_src} NAME_WE)
    add_executable(${util_name} ${util_src})
    target_compile_options(${util_name} PRIVATE
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
    )
    set_target_properties(${util_name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/util"
      EXCLUDE_FROM_ALL TRUE
    )
    list(APPEND UTIL_TARGETS ${util_name})
  endforeach()
  add_custom_target(utils DEPENDS ${UTIL_TARGETS})
endif()

# Default output directories for any targets that do not specify their own
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Provide an easy top-level target for the library-only default build
# add_custom_target(bond ALL DEPENDS bond_lib)
