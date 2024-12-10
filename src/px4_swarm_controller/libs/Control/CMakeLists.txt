add_library(Control 
    Control.cpp
)

target_include_directories(Control PUBLIC 
    .
)

target_link_libraries(Control 
    yaml-cpp
)

ament_target_dependencies(Control 
    rclcpp
    px4_msgs
)