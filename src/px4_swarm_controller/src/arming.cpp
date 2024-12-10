/**
 * @file arming.cpp
 * @author Apoorv Thapliyal
 * @brief ROS2 Service Client Node to arm the PX4
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include "Control.hpp"

int main(int argc, char *argv[]) {
  std::cout << "Arming the PX4 drones" << std::endl;
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ctrl::Control>());
  rclcpp::shutdown();
  return 0;
}
