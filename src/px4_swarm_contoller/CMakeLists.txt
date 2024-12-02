cmake_minimum_required(VERSION 3.8)
project(px4_swarm_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(gazebo_ros REQUIRED)
find_package(px4_msgs REQUIRED)
find_package(std_msgs REQUIRED)

# add subdirectories
add_subdirectory(libs)
# include_directories(libs)

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
  src
  DESTINATION share/${PROJECT_NAME}/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
