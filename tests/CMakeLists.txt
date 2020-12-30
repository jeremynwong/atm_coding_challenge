################################################################################
### Header
################################################################################
cmake_minimum_required(VERSION 3.10)

project(catch_tests)

################################################################################
### Misc
################################################################################
# Expose required variable (CATCH_INCLUDE_DIR) to parent scope
set(CATCH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../catch2"
        CACHE INTERNAL "Path to include folder for Catch")

# Disable compiler warnings that Catch throws
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-variadic-macros")
elseif(MSVC)
endif()


################################################################################
### Specify Build Targets
################################################################################
include_directories(${CATCH_INCLUDE_DIR})

# Build test executable
add_executable(atm_unit_tests
  ${CMAKE_CURRENT_SOURCE_DIR}/unit_tests.cpp
)
target_link_libraries(atm_unit_tests atm)

# Add cmake unit tests
add_test(NAME atm_unit_tests COMMAND atm_unit_tests)