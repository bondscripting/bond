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

# Static by default; BUILD_SHARED_LIBS=ON builds the runtime as a shared library.
add_library(bond_lib ${BOND_SOURCES})
target_include_directories(bond_lib PUBLIC ${INCLUDE_DIR} ${SOURCE_DIR})
# Debugging asserts are enabled by default in bond/conf.h; disable them in all non-Debug configs.
# PUBLIC so that header templates such as AutoStack are compiled consistently by all consumers.
target_compile_definitions(bond_lib PUBLIC $<$<NOT:$<CONFIG:Debug>>:BOND_ASSERTS_ENABLED=0>)
set_target_properties(bond_lib PROPERTIES
  OUTPUT_NAME bond
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(BUILD_SHARED_LIBS)
  message(STATUS "Bond runtime library: SHARED")
  target_compile_definitions(bond_lib PRIVATE BOND_BUILD_SHARED PUBLIC BOND_SHARED)
  # Distinct PDB name so the DLL's PDB does not collide with the bond tool's PDB in bin/.
  set_target_properties(bond_lib PROPERTIES PDB_NAME bond_lib)
  if(NOT MSVC)
    # Hide everything not marked BOND_API so missing exports fail to link, as they do on Windows.
    set_target_properties(bond_lib PROPERTIES
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN ON
    )
  endif()
else()
  message(STATUS "Bond runtime library: STATIC")
endif()

# Windows has no RPATH, so the DLL must sit beside every executable that loads it.
function(bond_copy_runtime_dll target)
  if(BUILD_SHARED_LIBS AND WIN32)
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:bond_lib> $<TARGET_FILE_DIR:${target}>
    )
  endif()
endfunction()

if(BOND_ENABLE_DEFAULT_WARNINGS)
  if(MSVC)
    target_compile_options(bond_lib PRIVATE /W4 /wd"4100" /permissive-)
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
      target_compile_options(framework_lib PRIVATE /W4 /wd"4100" /permissive-)
    else()
      target_compile_options(framework_lib PRIVATE
        -Wall -Wextra -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization
        -Winit-self -Wold-style-cast -Wredundant-decls -Wshadow -Wsign-promo
        -Wstrict-null-sentinel -Wundef -Wno-unused
      )
    endif()
  endif()
  # Links bond_lib so framework sources get BOND_SHARED (dllimport) in shared builds.
  target_link_libraries(framework_lib PUBLIC bond_lib)
  list(APPEND FRAMEWORK_TARGETS framework_lib)
  add_custom_target(framework DEPENDS framework_lib)
endif()

set(TEST_TARGETS)
if(BOND_BUILD_TESTS)
  foreach(test_src IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_include_directories(${test_name} PRIVATE ${INCLUDE_DIR} ${TEST_DIR})
    target_link_libraries(${test_name} PRIVATE bond_lib framework_lib ${CMAKE_DL_LIBS})
    set_target_properties(${test_name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/testbin"
      EXCLUDE_FROM_ALL TRUE
    )
    bond_copy_runtime_dll(${test_name})
    list(APPEND TEST_TARGETS ${test_name})
  endforeach()
  add_custom_target(test DEPENDS ${TEST_TARGETS})

  # Tests open scripts/*.bond by relative path, so they must run from the source root.
  set(RUN_TEST_COMMANDS)
  foreach(test_name IN LISTS TEST_TARGETS)
    list(APPEND RUN_TEST_COMMANDS COMMAND $<TARGET_FILE:${test_name}>)
  endforeach()
  add_custom_target(run_tests
    ${RUN_TEST_COMMANDS}
    DEPENDS ${TEST_TARGETS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running Bond unit tests"
    VERBATIM
  )
endif()

set(UTIL_TARGETS)
if(BOND_BUILD_UTILS)
  foreach(util_src IN LISTS UTIL_SOURCES)
    get_filename_component(util_name ${util_src} NAME_WE)
    add_executable(${util_name} ${util_src})
    target_compile_options(${util_name} PRIVATE
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
    )
    target_compile_definitions(${util_name} PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
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
