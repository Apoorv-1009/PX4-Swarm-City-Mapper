/**
 * @file Control.hpp
 * @author Apoorv Thapliyal
 * @brief C++ Header file for the Control class
 * @version 0.1
 * @date 2024-12-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <px4_msgs/msg/offboard_control_mode.hpp>
#include <px4_msgs/msg/trajectory_setpoint.hpp>
#include <px4_msgs/msg/vehicle_control_mode.hpp>
#include <px4_msgs/msg/vehicle_local_position.hpp>
#include <px4_msgs/msg/vehicle_status.hpp>
#include <px4_msgs/srv/vehicle_command.hpp>
#include <rclcpp/rclcpp.hpp>
#include <string>
#include <vector>

#include "rclcpp/duration.hpp"
#include "rclcpp/qos.hpp"

using namespace px4_msgs::msg;

/**
 * @brief Namespace for the Control class
 *
 */
namespace ctrl {

class Control : public rclcpp::Node {
 private:
  /**
   * @brief Create an alias for the OffboardControlMode Publisher type
   *
   */
  using OffboardControlModePublisher =
      rclcpp::Publisher<OffboardControlMode>::SharedPtr;

  /**
   * @brief Create an alias for the TrajectorySetpoint Publisher type
   *
   */
  using TrajectorySetpointPublisher =
      rclcpp::Publisher<TrajectorySetpoint>::SharedPtr;

  /**
   * @brief Create an alias for the VehicleCommand Publisher type
   *
   */
  using VehicleCommandPublisher = rclcpp::Publisher<VehicleCommand>::SharedPtr;

  /**
   * @brief Create an alias for the VehicleLocalPosition Subscriber type
   *
   */
  using VehicleLocalPositionSubscriber =
      rclcpp::Subscription<VehicleLocalPosition>::SharedPtr;

  /**
   * @brief Vector of OffboardControlMode publishers
   *
   */
  std::vector<OffboardControlModePublisher> offboard_control_mode_publishers_;

  /**
   * @brief Vector of TrajectorySetpoint publishers
   *
   */
  std::vector<TrajectorySetpointPublisher> trajectory_setpoint_publishers_;

  /**
   * @brief Vector of VehicleCommand publishers
   *
   */
  std::vector<VehicleCommandPublisher> vehicle_command_publishers_;

  /**
   * @brief Vector of VehicleLocalPosition subscribers
   *
   */
  std::vector<VehicleLocalPositionSubscriber>
      vehicle_local_position_subscribers_;

  /**
   * @brief Common synced timestamp
   *
   */
  std::atomic<uint64_t> timestamp_;

  /**
   * @brief Counter for number of setpoints published
   *
   */
  int offboard_setpoint_counter_;

  /**
   * @brief Timer for publishing commands and setpoints
   *
   */
  rclcpp::TimerBase::SharedPtr timer_;

  /**
   * @brief Number of drones in the swarm
   *
   */
  int num_drones_;

  /**
   * @brief Number of setpoints for each drone
   *
   */
  int num_setpoints_;

  /**
   * @brief Boolean to check if the offboard mode and arm command has been sent
   *
   */
  bool offboard_arm_sent_;

  /**
   * @brief Vector to store each drones setpoints
   * @details Each drone has a vector of setpoints, each setpoint is a 4D array
   * [x, y, z, yaw]. It is assumed that each drone has the same number of
   * setpoints
   */
  std::vector<std::vector<std::array<float, 4>>> setpoints_;

  /**
   * @brief Counter to keep track of the current setpoint being published
   *
   */
  int setpoint_counter_;

  /**
   * @brief Vector to store the local position of each drone
   *
   */
  std::vector<std::array<float, 3>> local_positions_;

  /**
   * @brief Vector to store if each drone has reached its setpoint
   *
   */
  std::vector<bool> reached_setpoint_;

 public:
  /**
   * @brief Construct a new Control object
   *
   */
  Control();

  /**
   * @brief Function to publish offboard control mode
   *
   * @param pub_ Publisher for offboard control mode
   */
  void publish_offboard_control_mode(
      rclcpp::Publisher<OffboardControlMode>::SharedPtr pub_);

  /**
   * @brief Function to publish vehicle setpoint
   *
   * @param pub_ Publisher for vehicle setpoint
   * @param x x-coordinate
   * @param y y-coordinate
   * @param z z-coordinate
   * @param yaw yaw angle (-PI to PI)
   */
  void publish_trajectory_setpoint(
      rclcpp::Publisher<TrajectorySetpoint>::SharedPtr pub_, float x, float y,
      float z, float yaw);

  /**
   * @brief Function to arm the PX4
   *
   * @param pub_ Publisher for vehicle command
   */
  void arm(rclcpp::Publisher<VehicleCommand>::SharedPtr pub_);

  /**
   * @brief Function to change to offboard mode
   *
   * @param pub_ Publisher for offboard control mode
   */
  void offboard_mode(rclcpp::Publisher<VehicleCommand>::SharedPtr pub_);

  /**
   * @brief Function to publish vehicle command
   *
   * @param pub_ Publisher for vehicle command
   * @param command Command to be published
   * @param param1 PX4 command parameter 1
   * @param param2 PX4 command parameter 2
   */
  void publish_vehicle_command(
      rclcpp::Publisher<VehicleCommand>::SharedPtr pub_, uint16_t command,
      float param1 = 0.0, float param2 = 0.0);

  /**
   * @brief Function to publish the default setpoints read from the yaml file
   *
   */
  void publish_default_setpoints();

  /**
   * @brief Function callback for the local position subscriber
   *
   * @param drone_id Drone ID
   * @param msg VehicleLocalPosition message
   */
  void vehicle_local_position_callback(int drone_id,
                                       VehicleLocalPosition::SharedPtr msg);
};

}  // namespace ctrl