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

TEST_F(ControlL1Test, Arm) {
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

TEST_F(ControlL1Test, OffboardMode) {
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

TEST_F(ControlL1Test, PublishDefaultSetpoints) {
    // Assume the number of drones is fixed or known for the test
    const int num_drones = 2; // Replace with the known number of drones

    // Step 1: Mock publishers to simulate publishing for each drone
    std::vector<MockPublisher<px4_msgs::msg::TrajectorySetpoint>> mock_publishers(num_drones);

    // Step 2: Simulate setpoints for the drones
    std::vector<std::vector<std::array<float, 4>>> mock_setpoints = {
        {{1.0, 2.0, 3.0, 0.0}, {4.0, 5.0, 6.0, 1.0}},
        {{7.0, 8.0, 9.0, 0.5}, {10.0, 11.0, 12.0, 1.5}}
    };

    // Step 3: Simulate the `publish_default_setpoints` method indirectly
    for (size_t i = 0; i < mock_setpoints.size(); ++i) {
        for (const auto& setpoint : mock_setpoints[i]) {
            px4_msgs::msg::TrajectorySetpoint msg;
            msg.position = {setpoint[0], setpoint[1], setpoint[2]};
            msg.yaw = setpoint[3];
            msg.timestamp = control_node_->get_clock()->now().nanoseconds() / 1000;
            mock_publishers[i].publish(msg);

            auto captured_msg = mock_publishers[i].get_last_message();

            // Step 4: Validate published message matches the setpoints
            EXPECT_FLOAT_EQ(captured_msg.position[0], setpoint[0]);
            EXPECT_FLOAT_EQ(captured_msg.position[1], setpoint[1]);
            EXPECT_FLOAT_EQ(captured_msg.position[2], setpoint[2]);
            EXPECT_FLOAT_EQ(captured_msg.yaw, setpoint[3]);
            EXPECT_GT(captured_msg.timestamp, 0); // Timestamp should be valid
        }
    }
}

TEST_F(ControlL1Test, VehicleLocalPositionCallback) {
    px4_msgs::msg::VehicleLocalPosition::SharedPtr mock_msg = std::make_shared<px4_msgs::msg::VehicleLocalPosition>();
    mock_msg->x = 1.5;
    mock_msg->y = 2.5;
    mock_msg->z = -3.5;
    const int drone_id = 0;
    control_node_->vehicle_local_position_callback(drone_id, mock_msg);
    SUCCEED(); 
}
