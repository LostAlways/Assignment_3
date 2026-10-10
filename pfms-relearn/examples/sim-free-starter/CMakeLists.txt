cmake_minimum_required(VERSION 3.16)
project(sim_free_starter CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Pure C++ library: no ROS, no simulator. Your real code goes here too.
add_library(platform_sim src/kinematic_ackerman.cpp)
target_include_directories(platform_sim PUBLIC include)

# Small demo: drives the fake Audi and prints CSV you can plot.
add_executable(drive_demo src/drive_demo.cpp)
target_link_libraries(drive_demo platform_sim)

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
  enable_testing()
  find_package(GTest QUIET)          # Ubuntu: sudo apt install libgtest-dev
  if(NOT GTest_FOUND)
    include(FetchContent)            # otherwise download it once
    FetchContent_Declare(googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG v1.14.0
      GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(googletest)
    add_library(GTest::gtest_main ALIAS gtest_main)
  endif()
  include(GoogleTest)
  add_executable(test_kinematic test/test_kinematic.cpp)
  target_link_libraries(test_kinematic platform_sim GTest::gtest_main)
  gtest_discover_tests(test_kinematic)
endif()
