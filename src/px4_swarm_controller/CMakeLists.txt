cmake_minimum_required(VERSION 3.14)
project(px4_swarm_controller)

# Enable compiler warnings
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable code coverage
option(COVERAGE "Enable test coverage reporting" OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")


if(COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Enabling code coverage")
    add_compile_options(--coverage)
    add_link_options(--coverage)
  else()
    message(WARNING "Coverage is only supported with GCC or Clang. Ignoring COVERAGE flag.")
  endif()
endif()

# Declares a dependency on GoogleTest which is downloaded from GitHub
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enables testing for this directory and below
enable_testing()
include(GoogleTest)

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(gazebo_ros REQUIRED)
find_package(px4_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(ament_cmake_gtest REQUIRED)
find_package(catch_ros2 REQUIRED)
find_package(yaml-cpp REQUIRED)

# Add subdirectories
add_subdirectory(libs)
add_subdirectory(test)

# Add executables
add_executable(arm src/arming.cpp)
ament_target_dependencies(arm px4_msgs rclcpp std_msgs)
target_link_libraries(arm Control)


# Install executables
install(TARGETS
  arm
  DESTINATION lib/${PROJECT_NAME}
)

# Install directories
install(DIRECTORY 
  launch
  worlds
  libs
  test
  config
  src
  DESTINATION share/${PROJECT_NAME}/
)

# Add custom target for generating coverage reports
if(COVERAGE)
  find_program(LCOV_EXEC lcov)
  find_program(GENHTML_EXEC genhtml)

  if(LCOV_EXEC AND GENHTML_EXEC)
    add_custom_target(coverage
      COMMAND ${LCOV_EXEC} --capture --directory . --output-file coverage.info
      COMMAND ${LCOV_EXEC} --remove coverage.info 
      '*test/*' 
      '*app/*' 
      '/usr/*' 
      '*/CMakeFiles/*'
      --output-file filtered_coverage.info
      COMMAND ${LCOV_EXEC} --extract filtered_coverage.info "*/libs/*" --output-file libs_coverage.info
      COMMAND ${GENHTML_EXEC} --output-directory coverage_report libs_coverage.info
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating code coverage report for the 'libs' folder"
      VERBATIM
    )
  else()
    message(WARNING "lcov or genhtml not found. Coverage report generation will not work.")
  endif()
endif()


install(PROGRAMS
  scripts/generate_coverage_report.bash
  DESTINATION lib/${PROJECT_NAME})

ament_package()
