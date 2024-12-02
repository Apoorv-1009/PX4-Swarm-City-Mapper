/**
 * @file Control.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

 #include "Control.hpp"
#include <iostream>

ctrl::Control::Control() : Node("Control"){

    // Initialize the offboard control mode publisher
    offboard_control_mode_publisher_ = this->create_publisher<OffboardControlMode>("/fmu/in/offboard_control_mode", 10);

    // Initialize the trajectory setpoint publisher
    trajectory_setpoint_publisher_ = this->create_publisher<TrajectorySetpoint>("/fmu/in/trajectory_setpoint", 10);

    // Initialize the vehicle command publisher
    vehicle_command_publisher_ = this->create_publisher<VehicleCommand>("/fmu/in/vehicle_command", 10);

    // Initialize the setpoint counter
    offboard_setpoint_counter_ = 0;

    std::cout << "Control node initialized" << std::endl;

    // Define a timer callback
    auto timer_callback = [this]() -> void{

        if(offboard_setpoint_counter_ == 10) {
            // Change to offboard mode after 10 setpoints
            offboard_mode(vehicle_command_publisher_);

            // Arm the drone
            arm(vehicle_command_publisher_);
        }

        std::cout << "Publishing setpoint " << offboard_setpoint_counter_ << std::endl;

        // The offboard control mode needs to be paired with a trajectory setpoint
        publish_offboard_control_mode(offboard_control_mode_publisher_);
        publish_trajectory_setpoint(trajectory_setpoint_publisher_, 0.0, 0.0, -5.0, 0.0);

        // Stop the counter after reaching 11
        if(offboard_setpoint_counter_ < 11) {
            offboard_setpoint_counter_++;
        }
    };

    // Create a timer with a 100ms period
    timer_ = this->create_wall_timer(std::chrono::milliseconds(100), timer_callback);
}

void ctrl::Control::publish_offboard_control_mode(rclcpp::Publisher<OffboardControlMode>::SharedPtr pub_){
    OffboardControlMode msg;
    msg.position = true;
    msg.velocity = false;
    msg.acceleration = false;
    msg.attitude = false;
    msg.body_rate = false;
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
    pub_->publish(msg);
}

void ctrl::Control::publish_trajectory_setpoint(rclcpp::Publisher<TrajectorySetpoint>::SharedPtr pub_, float x, float y, float z, float yaw){
    TrajectorySetpoint msg;
    msg.position = {x, y, z};
    msg.yaw = yaw;
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
    pub_->publish(msg);
}

void ctrl::Control::arm(rclcpp::Publisher<VehicleCommand>::SharedPtr pub_){
    publish_vehicle_command(pub_, VehicleCommand::VEHICLE_CMD_COMPONENT_ARM_DISARM, 1.0);

    RCLCPP_INFO(this->get_logger(), "Arm command sent");
}

void ctrl::Control::offboard_mode(rclcpp::Publisher<VehicleCommand>::SharedPtr pub_){
    publish_vehicle_command(pub_, VehicleCommand::VEHICLE_CMD_DO_SET_MODE, 1.0, 6.0);

    RCLCPP_INFO(this->get_logger(), "Offboard mode command sent");
}

void ctrl::Control::publish_vehicle_command(rclcpp::Publisher<VehicleCommand>::SharedPtr pub_, uint16_t command, float param1, float param2){
    VehicleCommand msg;
    msg.param1 = param1;
    msg.param2 = param2;
    msg.command = command;
    msg.target_system = 1;
    msg.target_component = 1;
    msg.source_system = 1;
    msg.source_component = 1;
    msg.from_external = true;
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
    pub_->publish(msg);
}

