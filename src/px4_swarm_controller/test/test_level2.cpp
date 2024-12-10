/**
 * @file test_level2.cpp
 * @author Mohammed Munawwar
 * @brief C++ file for testing the Control package
 * @version 0.1
 * @date 2024-12-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <catch_ros2/catch_ros2.hpp>
#include <rclcpp/rclcpp.hpp>
#include "px4_msgs/msg/offboard_control_mode.hpp"
#include "px4_msgs/msg/trajectory_setpoint.hpp"
#include "px4_msgs/msg/vehicle_command.hpp"
#include "px4_msgs/msg/vehicle_local_position.hpp"
#include "Control.hpp"

using namespace std::chrono_literals;

////////////////////////////////////////////////
// Test Fixture for Control Node
////////////////////////////////////////////////
class ControlL2Test {
public:
    ControlL2Test() {
        // Create test node
        Node = rclcpp::Node::make_shared("Integration_Test_Control");
        // // Logger

        // // Initialize publishers and subscribers
        // trajectory_setpoint_publisher_ = test_node_->create_publisher<px4_msgs::msg::TrajectorySetpoint>(
        //     "/px4_1/fmu/in/trajectory_setpoint", 10);
        // trajectory_setpoint_subscriber_ = test_node_->create_subscription<px4_msgs::msg::TrajectorySetpoint>(
        //     "/px4_1/fmu/in/trajectory_setpoint", 10,
        //     [this](const px4_msgs::msg::TrajectorySetpoint::SharedPtr msg) {
        //         received_trajectory_setpoint_ = msg;
        //         trajectory_setpoint_received_ = true;
        //     });

        // trajectory_setpoint_received_ = false;

        // // Launch the Control node
        // control_node_ = std::make_shared<ctrl::Control>();
        // executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
        // executor_->add_node(control_node_);
    }

    ~ControlL2Test() {
        // executor_->remove_node(control_node_);
    }

    // void spin_and_wait_for_setpoint() {
    //     auto start_time = std::chrono::steady_clock::now();
    //     while (!trajectory_setpoint_received_ && 
    //            std::chrono::steady_clock::now() - start_time < 5s) {
    //         executor_->spin_some();
    //         rclcpp::sleep_for(50ms);
    //     }
    // }

    // rclcpp::Node::SharedPtr test_node_;
    // std::shared_ptr<rclcpp::executors::SingleThreadedExecutor> executor_;

    // std::shared_ptr<ctrl::Control> control_node_;
    // rclcpp::Publisher<px4_msgs::msg::TrajectorySetpoint>::SharedPtr trajectory_setpoint_publisher_;
    // rclcpp::Subscription<px4_msgs::msg::TrajectorySetpoint>::SharedPtr trajectory_setpoint_subscriber_;

    // px4_msgs::msg::TrajectorySetpoint::SharedPtr received_trajectory_setpoint_;
    // bool trajectory_setpoint_received_;

    protected:
     rclcpp::Node::SharedPtr Node;
};

////////////////////////////////////////////////
// Test Case: Offboard Control Node
////////////////////////////////////////////////

TEST_CASE_METHOD(ControlL2Test, "test offboard control mode", "[IntegrationTest][OffboardControlNode]") {

    auto offboard_control_mode_topic = "/px4_1/fmu/in/offboard_control_mode";
    //
    auto executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();

    bool received_message = false;

    // Subscriber for the offboard control mode
    auto offboard_sub = Node->create_subscription<OffboardControlMode>(
        offboard_control_mode_topic, 
        10, 
        [&received_message](const OffboardControlMode::SharedPtr /*msg*/) -> void {
            received_message = true;
        });

    executor->add_node(Node);

    // Run the executor for a short period to allow message processing
   auto start_time = std::chrono::steady_clock::now();
   while (!received_message && (std::chrono::steady_clock::now()
      - start_time) < std::chrono::seconds(5)) {
    executor->spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(100)
     );
    }

    executor->cancel();

    REQUIRE(received_message);
}

TEST_CASE_METHOD(ControlL2Test, "test trajectory setpoint", "[IntegrationTest][TrajectorySetpoint]") {

    auto trajectory_setpoint_topic = "/px4_1/fmu/in/trajectory_setpoint";
    //
    auto executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();

    bool received_setpoints = false;

    // Subscriber for the offboard control mode
    auto offboard_sub = Node->create_subscription<TrajectorySetpoint>(
        trajectory_setpoint_topic, 
        10, 
        [&received_setpoints](const TrajectorySetpoint::SharedPtr /*msg*/) -> void {
            received_setpoints = true;
        });

    executor->add_node(Node);

    // Run the executor for a short period to allow message processing
   auto start_time = std::chrono::steady_clock::now();
   while (!received_setpoints && (std::chrono::steady_clock::now()
      - start_time) < std::chrono::seconds(5)) {
    executor->spin_some();
    std::this_thread::sleep_for(std::chrono::milliseconds(100)
     );
    }

    executor->cancel();

    REQUIRE(received_setpoints);
}


