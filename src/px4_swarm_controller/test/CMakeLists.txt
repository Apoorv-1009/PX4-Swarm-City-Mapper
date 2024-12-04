# Any C++ source files needed to build this target
add_executable(cpp-test
  test.cpp
  main.cpp
  # src/px4_swarm_contoller/libs/Control/Control.cpp
  # src/px4_swarm_contoller/libs/Control/Control.hpp
  )

target_include_directories(cpp-test PUBLIC
  # list of include directories:
  # ../vendor/googletest/googletest/include
  ${CMAKE_SOURCE_DIR}/include
  )

  # install(FILES src/px4_swarm_contoller/config/config.yaml
  #       DESTINATION share/${PROJECT_NAME}/config)

# Any dependent libraires needed to build this target
target_link_libraries(cpp-test PUBLIC
  # list of libraries:
  gtest
  gmock
  gtest_main
  Control
  )

# Enable CMake’s test runner to discover the tests included in the
# binary, using the GoogleTest CMake module.
gtest_discover_tests(cpp-test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})