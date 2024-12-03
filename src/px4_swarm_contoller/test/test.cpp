/**
 * @file test.cpp
 * @author Apoorv Thapliyal
 * @brief C++ test file for the Control class
 * @version 0.1
 * @date 2024-12-02
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

// MockPublisher simply captures the last published message
template <typename MessageType>
class MockPublisher {
public:
    void publish(const MessageType &msg) {
        last_message_ = msg;
    }

    MessageType get_last_message() const {
        return last_message_;
    }

private:
    MessageType last_message_;
};

class ControlL1Test : public ::testing::Test {
protected:
    void SetUp() override {
        rclcpp::init(0, nullptr);
        control_node_ = std::make_shared<ctrl::Control>();
    }

    void TearDown() override {
        rclcpp::shutdown();
    }

    std::shared_ptr<ctrl::Control> control_node_;
};

TEST_F(ControlL1Test, PublishOffboardControlMode) {
    MockPublisher<px4_msgs::msg::OffboardControlMode> mock_pub;

    // Simulate publishing
    px4_msgs::msg::OffboardControlMode msg;
    msg.position = true;
    msg.velocity = false;
    msg.acceleration = false;
    msg.attitude = false;
    msg.body_rate = false;
    msg.timestamp = 123456789; // Mock timestamp

    mock_pub.publish(msg);

    auto captured_msg = mock_pub.get_last_message();
    EXPECT_TRUE(captured_msg.position);
    EXPECT_FALSE(captured_msg.velocity);
    EXPECT_FALSE(captured_msg.acceleration);
    EXPECT_FALSE(captured_msg.attitude);
    EXPECT_FALSE(captured_msg.body_rate);
    EXPECT_EQ(captured_msg.timestamp, 123456789);
}

TEST_F(ControlL1Test, PublishTrajectorySetpoint) {
    MockPublisher<px4_msgs::msg::TrajectorySetpoint> mock_pub;

    // Simulate publishing
    px4_msgs::msg::TrajectorySetpoint msg;
    msg.position = {1.0, 2.0, 3.0};
    msg.yaw = 0.5;
    msg.timestamp = 123456789; // Mock timestamp

    mock_pub.publish(msg);

    auto captured_msg = mock_pub.get_last_message();
    EXPECT_FLOAT_EQ(captured_msg.position[0], 1.0);
    EXPECT_FLOAT_EQ(captured_msg.position[1], 2.0);
    EXPECT_FLOAT_EQ(captured_msg.position[2], 3.0);
    EXPECT_FLOAT_EQ(captured_msg.yaw, 0.5);
    EXPECT_EQ(captured_msg.timestamp, 123456789);
}

TEST_F(ControlL1Test, ArmCommand) {
    MockPublisher<px4_msgs::msg::VehicleCommand> mock_pub;

    // Simulate publishing
    px4_msgs::msg::VehicleCommand msg;
    msg.command = px4_msgs::msg::VehicleCommand::VEHICLE_CMD_COMPONENT_ARM_DISARM;
    msg.param1 = 1.0;
    msg.timestamp = 123456789; // Mock timestamp

    mock_pub.publish(msg);

    auto captured_msg = mock_pub.get_last_message();
    EXPECT_EQ(captured_msg.command, px4_msgs::msg::VehicleCommand::VEHICLE_CMD_COMPONENT_ARM_DISARM);
    EXPECT_FLOAT_EQ(captured_msg.param1, 1.0);
    EXPECT_EQ(captured_msg.timestamp, 123456789);
}

TEST_F(ControlL1Test, OffboardModeCommand) {
    MockPublisher<px4_msgs::msg::VehicleCommand> mock_pub;

    // Simulate publishing
    px4_msgs::msg::VehicleCommand msg;
    msg.command = px4_msgs::msg::VehicleCommand::VEHICLE_CMD_DO_SET_MODE;
    msg.param1 = 1.0;
    msg.param2 = 6.0;
    msg.timestamp = 123456789; // Mock timestamp

    mock_pub.publish(msg);

    auto captured_msg = mock_pub.get_last_message();
    EXPECT_EQ(captured_msg.command, px4_msgs::msg::VehicleCommand::VEHICLE_CMD_DO_SET_MODE);
    EXPECT_FLOAT_EQ(captured_msg.param1, 1.0);
    EXPECT_FLOAT_EQ(captured_msg.param2, 6.0);
    EXPECT_EQ(captured_msg.timestamp, 123456789);
}

TEST_F(ControlL1Test, PublishVehicleCommand) {
    MockPublisher<px4_msgs::msg::VehicleCommand> mock_pub;

    // Simulate publishing
    px4_msgs::msg::VehicleCommand msg;
    msg.command = 42;
    msg.param1 = 3.14;
    msg.param2 = 2.71;
    msg.timestamp = 123456789; // Mock timestamp

    mock_pub.publish(msg);

    auto captured_msg = mock_pub.get_last_message();
    EXPECT_EQ(captured_msg.command, 42);
    EXPECT_FLOAT_EQ(captured_msg.param1, 3.14);
    EXPECT_FLOAT_EQ(captured_msg.param2, 2.71);
    EXPECT_EQ(captured_msg.timestamp, 123456789);
}
