cmake_minimum_required(VERSION 3.14)
project(px4_swarm_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# declares a dependency on GoogleTest which is downloaded from GitHub
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

# Enable code coverage
if(COVERAGE)
  find_program(LCOV_EXEC lcov)
  find_program(GENHTML_EXEC genhtml)

  if(LCOV_EXEC AND GENHTML_EXEC)
    add_custom_target(test_coverage
      COMMAND ${LCOV_EXEC} --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info
      COMMAND ${GENHTML_EXEC} --output-directory coverage_report coverage.info
      COMMENT "Generating test coverage report"
      VERBATIM
    )
  else()
    message(WARNING "lcov or genhtml not found. Coverage report generation will not work.")
  endif()
endif()


# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(gazebo_ros REQUIRED)
find_package(px4_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(ament_cmake_gtest REQUIRED)
find_package(catch_ros2 REQUIRED)
find_package(yaml-cpp REQUIRED)

# add subdirectories
add_subdirectory(libs)
add_subdirectory(test)

# add executables
add_executable(arm src/arming.cpp)
ament_target_dependencies(arm px4_msgs rclcpp std_msgs)
target_link_libraries(arm Control)


# install executables
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
  src
  DESTINATION share/${PROJECT_NAME}/
)

# if(BUILD_TESTING)
#   find_package(ament_lint_auto REQUIRED)
#   # the following line skips the linter which checks for copyrights
#   # comment the line when a copyright and license is added to all source files
#   set(ament_cmake_copyright_FOUND TRUE)
#   # the following line skips cpplint (only works in a git repo)
#   # comment the line when this package is in a git repo and when
#   # a copyright and license is added to all source files
#   set(ament_cmake_cpplint_FOUND TRUE)
#   ament_lint_auto_find_test_dependencies()
# endif()

install(PROGRAMS
  scripts/generate_coverage_report.bash
  DESTINATION lib/${PROJECT_NAME})

ament_package()
