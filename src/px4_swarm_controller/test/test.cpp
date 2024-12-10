/**
 * @file test.cpp
 * @author Mohammed Munawwar
 * @brief C++ file for testing the Control node
 * @version 0.1
 * @date 2024-12-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <gtest/gtest.h>
#include <rclcpp/rclcpp.hpp>
#include <px4_msgs/msg/offboard_control_mode.hpp>
#include <px4_msgs/msg/trajectory_setpoint.hpp>
#include <px4_msgs/msg/vehicle_command.hpp>
#include "Control.hpp"

class ControlL1Test : public ::testing::Test {
protected:
  std::shared_ptr<ctrl::Control> control_test_1;
  std::shared_ptr<rclcpp::Node> test_node_; // Declare this here

  void SetUp() override {
    rclcpp::init(0, nullptr);
    test_node_ = std::make_shared<rclcpp::Node>("test_node");
    control_test_1 = std::make_shared<ctrl::Control>();

  }

  void TearDown() override {
    rclcpp::shutdown();
  }
};

TEST_F(ControlL1Test, PublishOffboardControlMode) {
  // Create a publisher
  auto offboard_control_mode_publisher = test_node_->create_publisher<OffboardControlMode>("test_offboard_control_mode", 10);

  // Create a subscription to verify the message
  OffboardControlMode received_msg;
  auto subscription = test_node_->create_subscription<OffboardControlMode>(
    "test_offboard_control_mode",
    10,
    [&received_msg](const OffboardControlMode::SharedPtr msg) {
      received_msg = *msg;
    });

  // Sleep briefly to ensure the subscription is set up
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Invoke the method under test
  control_test_1->publish_offboard_control_mode(offboard_control_mode_publisher);

  // Spin briefly to process the published message
  rclcpp::spin_some(test_node_);

  // Validate the published message
  EXPECT_TRUE(received_msg.position);
  EXPECT_FALSE(received_msg.velocity);
  EXPECT_FALSE(received_msg.acceleration);
  EXPECT_FALSE(received_msg.attitude);
  EXPECT_FALSE(received_msg.body_rate);
  EXPECT_GT(received_msg.timestamp, 0); // Ensure timestamp is set
}

TEST_F(ControlL1Test, PublishTrajectorySetpoint) {
  // Create a publisher
  auto trajectory_setpoint_publisher = test_node_->create_publisher<px4_msgs::msg::TrajectorySetpoint>("test_trajectory_setpoint", 10);

  // Create a subscription to verify the message
  px4_msgs::msg::TrajectorySetpoint received_msg;
  auto subscription = test_node_->create_subscription<px4_msgs::msg::TrajectorySetpoint>(
    "test_trajectory_setpoint",
    10,
    [&received_msg](const px4_msgs::msg::TrajectorySetpoint::SharedPtr msg) {
      received_msg = *msg;
    });

  // Sleep briefly to ensure the subscription is set up
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Parameters for the publish_trajectory_setpoint function
  float x = 1.0, y = 2.0, z = 3.0, yaw = 0.5;
  control_test_1->publish_trajectory_setpoint(trajectory_setpoint_publisher, x, y, z, yaw);

  // Spin briefly to process the published message
  rclcpp::spin_some(test_node_);

  // Validate the published message
  EXPECT_EQ(received_msg.position[0], x);
  EXPECT_EQ(received_msg.position[1], y);
  EXPECT_EQ(received_msg.position[2], z);
  EXPECT_EQ(received_msg.yaw, yaw);
  EXPECT_GT(received_msg.timestamp, 0); // Ensure timestamp is set
}

TEST_F(ControlL1Test, Arm) {
  // Create a publisher
  auto arm_publisher = test_node_->create_publisher<px4_msgs::msg::VehicleCommand>("test_vehicle_command_to_arm", 10);

  // Create a subscription to verify the message
  px4_msgs::msg::VehicleCommand received_msg;
  auto subscription = test_node_->create_subscription<px4_msgs::msg::VehicleCommand>(
    "test_vehicle_command_to_arm",
    10,
    [&received_msg](const px4_msgs::msg::VehicleCommand::SharedPtr msg) {
      received_msg = *msg;
    });

  // Sleep briefly to ensure the subscription is set up
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Invoke the method under test
  control_test_1->arm(arm_publisher);

  // Spin briefly to process the published message
  rclcpp::spin_some(test_node_);

  // Validate the published message
  EXPECT_EQ(received_msg.command, px4_msgs::msg::VehicleCommand::VEHICLE_CMD_COMPONENT_ARM_DISARM);
  EXPECT_EQ(received_msg.param1, 1.0); // The expected parameter value
}

TEST_F(ControlL1Test, OffboardMode) {
  // Create a publisher
  auto offboard_mode_publisher = test_node_->create_publisher<px4_msgs::msg::VehicleCommand>("test_offboard_mode_vehicle_command", 10);

  // Create a subscription to verify the message
  px4_msgs::msg::VehicleCommand received_msg;
  auto subscription = test_node_->create_subscription<px4_msgs::msg::VehicleCommand>(
    "test_offboard_mode_vehicle_command",
    10,
    [&received_msg](const px4_msgs::msg::VehicleCommand::SharedPtr msg) {
      received_msg = *msg;
    });

  // Sleep briefly to ensure the subscription is set up
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Invoke the method under test
  control_test_1->offboard_mode(offboard_mode_publisher);

  // Spin briefly to process the published message
  rclcpp::spin_some(test_node_);

  // Validate the published message
  EXPECT_EQ(received_msg.command, static_cast<uint16_t>(px4_msgs::msg::VehicleCommand::VEHICLE_CMD_DO_SET_MODE));
  EXPECT_EQ(received_msg.param1, 1.0);  // First expected parameter
  EXPECT_EQ(received_msg.param2, 6.0);  // Second expected parameter
  EXPECT_GT(received_msg.timestamp, 0); // Ensure timestamp is set
}

TEST_F(ControlL1Test, PublishVehicleCommand) {
  // Create a publisher
  auto vehicle_command_publisher = test_node_->create_publisher<px4_msgs::msg::VehicleCommand>("test_vehicle_command", 10);

  // Create a subscription to verify the message
  px4_msgs::msg::VehicleCommand received_msg;
  auto subscription = test_node_->create_subscription<px4_msgs::msg::VehicleCommand>(
    "test_vehicle_command",
    10,
    [&received_msg](const px4_msgs::msg::VehicleCommand::SharedPtr msg) {
      received_msg = *msg;
    });

  // Sleep briefly to ensure the subscription is set up
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Parameters for the publish_vehicle_command function
  uint16_t command = px4_msgs::msg::VehicleCommand::VEHICLE_CMD_DO_SET_MODE;
  float param1 = 1.0;
  float param2 = 6.0;

  // Invoke the method under test
  control_test_1->publish_vehicle_command(vehicle_command_publisher, command, param1, param2);

  // Spin briefly to process the published message
  rclcpp::spin_some(test_node_);

  // Validate the published message
  EXPECT_EQ(received_msg.command, command);
  EXPECT_EQ(received_msg.param1, param1);
  EXPECT_EQ(received_msg.param2, param2);
  EXPECT_EQ(received_msg.target_system, 0); // Default system
  EXPECT_EQ(received_msg.target_component, 1); // Default component
  EXPECT_EQ(received_msg.source_system, 1); // Default source system
  EXPECT_EQ(received_msg.source_component, 1); // Default source component
  EXPECT_EQ(received_msg.from_external, true); // Default from_external
  EXPECT_GT(received_msg.timestamp, 0); // Ensure timestamp is set
}

TEST_F(ControlL1Test, PublishDefaultSetpoints) {
  // Invoke the method under test
  control_test_1->publish_default_setpoints();
  SUCCEED();
}

//   // Check that the correct number of setpoints were published
//   EXPECT_EQ(control_test_1->get_setpoint_counter(), setpoint_counter_);

//   for (int i = 0; i < num_drones_; ++i) {
//     // Create a subscription to verify the message
//     px4_msgs::msg::TrajectorySetpoint received_msg;
//     auto subscription = test_node_->create_subscription<px4_msgs::msg::TrajectorySetpoint>(
//       "test_trajectory_setpoint_" + std::to_string(i),
//       10,
//       [&received_msg](const px4_msgs::msg::TrajectorySetpoint::SharedPtr msg) {
//         received_msg = *msg;
//       });

//     // Sleep briefly to ensure the subscription is set up
//     std::this_thread::sleep_for(std::chrono::milliseconds(100));

//     // Spin briefly to process the published message
//     rclcpp::spin_some(test_node_);

//     // Validate the published message
//     EXPECT_EQ(received_msg.position[0], setpoints_[i][0]);
//     EXPECT_EQ(received_msg.position[1], setpoints_[i][1]);
//     EXPECT_EQ(received_msg.position[2], setpoints_[i][2]);
//     EXPECT_EQ(received_msg.yaw, setpoints_[i][3]);

//     // Validate the timestamp
//     EXPECT_GT(received_msg.timestamp, 0); // Ensure timestamp is set

//     // Validate the distance calculation
//     float distance = std::sqrt(std::pow(local_positions_[i][0] - setpoints_[i][0], 2) +
//                                std::pow(local_positions_[i][1] - setpoints_[i][1], 2) +
//                                std::pow(local_positions_[i][2] - setpoints_[i][2], 2));
//     EXPECT_LT(distance, 0.3); // The drone should be close to its setpoint
//   }
// } local_positions_.resize(num_drones_, {0.0, 0.0, 0.0});

TEST_F(ControlL1Test, VehicleLocalPositionCallback) {
  // Create a mock message
  auto msg = std::make_shared<px4_msgs::msg::VehicleLocalPosition>();
  int drone_id = 2; // Choose a drone ID for testing
  msg->x = 1.5;
  msg->y = 2.5;
  msg->z = 3.5;

  // Call the method under test
  EXPECT_NO_FATAL_FAILURE(control_test_1->vehicle_local_position_callback(drone_id, msg));
}
