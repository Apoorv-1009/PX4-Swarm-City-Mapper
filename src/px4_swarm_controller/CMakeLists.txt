cmake_minimum_required(VERSION 3.14)
project(px4_swarm_controller)

# Enable compiler warnings
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

############################################################################
# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(gazebo_ros REQUIRED)
find_package(px4_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(ament_cmake_gtest REQUIRED)
find_package(catch_ros2 REQUIRED)
find_package(yaml-cpp REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/Control)
############################################################################

# Add executables
add_executable(arm src/arming.cpp)
ament_target_dependencies(arm px4_msgs rclcpp std_msgs)
target_include_directories(arm PUBLIC Control)
target_link_libraries(arm Control)

############################################################################
add_executable(level2_test test/test_level2.cpp)
ament_target_dependencies(level2_test px4_msgs rclcpp std_msgs)
target_link_libraries(level2_test catch_ros2::catch_ros2_with_node_main Control)


############################################################################
if(BUILD_TESTING)
  set(TEST_FILE test/test.cpp
  test/main.cpp)

  #Add Gtest
  ament_add_gtest(${PROJECT_NAME}_test ${TEST_FILE})
  ament_target_dependencies(${PROJECT_NAME}_test px4_msgs rclcpp std_msgs)
  target_include_directories(${PROJECT_NAME}_test PUBLIC include/px4_swarm_controller Control)
  target_link_libraries(${PROJECT_NAME}_test Control)
  target_compile_definitions(${PROJECT_NAME}_test PUBLIC "TEST_SRC_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/test/\"")

  install(TARGETS ${PROJECT_NAME}_test
    DESTINATION lib/${PROJECT_NAME})
  # ament_lint_auto_find_test_dependencies()
endif()

############################################################################
catch_ros2_add_integration_test (Level2_Integration_Test
  LAUNCH_FILE integration_test.launch.yaml  # example in YAML
)

############################################################################
# Install executables
install(TARGETS
  arm
  level2_test
  DESTINATION lib/${PROJECT_NAME}
)

############################################################################
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

if(COVERAGE)
  message("Enabling coverage")
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")
  include(CodeCoverage)
  # The following line come from cmake-modules/CodeCoverage.cmake
  append_coverage_compiler_flags() # will add "--coverage" compilation flag
  # Exclude certain directories from coverage analysis
  # set(COVERAGE_EXCLUDES
  #   ${PROJECT_BINARY_DIR}
  #   )
endif()

# create targets for building code coverage reports
if(COVERAGE)
  if(UNIT_TEST_ALREADY_RAN)
    unset(EXECUTABLE)          # don't run unit test again, just generate the coverage report
  else()
    set(EXECUTABLE ctest test/)   # what to run insdie the build directory?
  endif()

  # unit test coverage:
  setup_target_for_coverage_lcov ( # use lcov to generate coverage report
    NAME test_coverage             # target name
    EXECUTABLE ${EXECUTABLE}
    EXCLUDE
      "libs/*"
      "/usr/include/*"          # Don't analyze system headers
      "/opt/*"                  # Don't analyze system headers
    )
endif()

# Declares a dependency on GoogleTest which is downloaded from GitHub
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Add subdirectories
add_subdirectory(libs)
# add_subdirectory(test)



# Install directories
install(DIRECTORY 
  launch
  worlds
  libs
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