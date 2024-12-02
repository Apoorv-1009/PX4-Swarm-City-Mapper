add_library(Control
  Control.cpp
)

target_include_directories(Control PUBLIC
    .
)

ament_target_dependencies(Control
    rclcpp
    px4_msgs
)