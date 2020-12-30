################################################################################
### Header
################################################################################
cmake_minimum_required(VERSION 3.10)

project(atm)

################################################################################
### Compiler Setup
################################################################################
if (CMAKE_COMPILER_IS_GNUCC)
  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  if (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    # set(DEPEND_LIBS "boost_system")
    message(STATUS "C++11 capable compiler detected")
  else()
    # gcc46 does not support C++11, and the linker is too old to know about AVX instructions
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -mno-avx")
    message(STATUS "C++11 support not available; defaulting to C++0x")
  endif()
else()
  message(WARNING "Cannot detect correct settings for your compiler")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

################################################################################
### Configure Source Files and Include Directories
################################################################################

# Find the header files
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

# Find the sources files
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Tell CMake where the headers are
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

################################################################################
### Specify Build Targets
################################################################################

##### Main Library 
# Tell cmake about the library
add_library(atm SHARED ${SOURCE_FILES} ${HEADER_FILES})

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)
target_include_directories(atm PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(atm ${HIREDIS_LIB})

# <------------ add redis-plus-plus dependency -------------->
find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(atm PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(atm ${REDIS_PLUS_PLUS_LIB})

##### UI Executables
add_subdirectory(ui)

################################################################################
### Unit Tests
################################################################################
# Add 'Catch' test directory
# Turn tests on and off using the CMake GUI
set(TESTS_ON true CACHE BOOL "Configure unit testing (uses Catch unit tests)")
if(TESTS_ON)
    enable_testing(true)
    add_subdirectory(tests)
endif(TESTS_ON)