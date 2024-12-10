/**
 * @file Control.cpp
 * @author Apoorv Thapliyal
 * @brief C++ Source file for the Control class
 * @version 0.1
 * @date 2024-12-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Control.hpp"

ctrl::Control::Control() : Node("Control") {
  std::cout << "Control node initialized" << std::endl;

  // Load the YAML file
  YAML::Node config = YAML::LoadFile(
      "../../install/px4_swarm_controller/share/px4_swarm_controller/config/"
      "config.yaml");
  // YAML::Node config =
  // YAML::LoadFile("src/px4_swarm_controller/config/config.yaml");

  // Get the number of drones in the swarm
  num_drones_ = config["num_drones"].as<int>();
  std::cout << "Number of drones in the swarm: " << num_drones_ << std::endl;

  // Resize the setpoints vector
  setpoints_.resize(num_drones_);

  // Resize the local positions vector
  local_positions_.resize(num_drones_);

  // Resize the vehicle local position subscribers vector
  vehicle_local_position_subscribers_.resize(num_drones_);

  // Resize the reached setpoint vector
  reached_setpoint_.resize(num_drones_);

  // Parse setpoints
  YAML::Node sp_node = config["setpoints"];

  // Iterate over each drone
  for (auto it = sp_node.begin(); it != sp_node.end(); it++) {
    // Get the drone ID, subtract 1 for 0 based indexing
    int drone_id = std::stoi(it->first.as<std::string>()) - 1;

    // Get the setpoints for the drone
    YAML::Node sp_list = it->second;

    for (size_t i = 0; i < sp_list.size(); ++i) {
      std::array<float, 4> sp;
      for (size_t j = 0; j < 4; ++j) {
        sp[j] = sp_list[i][j].as<float>();
      }
      setpoints_[drone_id].push_back(sp);
    }
  }

  // Get the number of setpoints for each drone
  num_setpoints_ = setpoints_[0].size();

  // std::cout << "Number of setpoints for each drone: " << num_setpoints_ <<
  // std::endl;

  // Initialize the offboard setpoint counter
  offboard_setpoint_counter_ = 0;

  // Initialise the setpoint counter
  setpoint_counter_ = 0;

  // Initalize the publishers and subscribers for every drone in the swarm
  for (int i = 1; i <= num_drones_; i++) {
    // String of the offboard control mode topic
    std::string offboard_control_mode_topic =
        "/px4_" + std::to_string(i) + "/fmu/in/offboard_control_mode";

    // String of the trajectory setpoint topic
    std::string trajectory_setpoint_topic =
        "/px4_" + std::to_string(i) + "/fmu/in/trajectory_setpoint";

    // String of the vehicle command topic
    std::string vehicle_command_topic =
        "/px4_" + std::to_string(i) + "/fmu/in/vehicle_command";

    // String of the local position topic
    std::string local_position_topic =
        "/px4_" + std::to_string(i) + "/fmu/out/vehicle_local_position";

    // Publisher for the offboard control mode
    offboard_control_mode_publishers_.push_back(
        this->create_publisher<OffboardControlMode>(offboard_control_mode_topic,
                                                    10));

    // Publisher for the trajectory setpoint
    trajectory_setpoint_publishers_.push_back(
        this->create_publisher<TrajectorySetpoint>(trajectory_setpoint_topic,
                                                   10));

    // Publisher for the vehicle command
    vehicle_command_publishers_.push_back(
        this->create_publisher<VehicleCommand>(vehicle_command_topic, 10));

    // Subscriber for the local position
    auto sub = this->create_subscription<VehicleLocalPosition>(
        local_position_topic, rclcpp::QoS(10).best_effort(),
        [this, i](const VehicleLocalPosition::SharedPtr msg) -> void {
          this->vehicle_local_position_callback(i - 1, msg);
        });

    // Store the subscriber in the subscribers vector
    vehicle_local_position_subscribers_[i - 1] = sub;
  }

  // Set sent offboard_arm_sent to false
  offboard_arm_sent_ = false;

  // Define a timer callback for controlling the drones
  auto timer_callback = [this]() -> void {
    // After enough setpoints and offboard control mode messages have been sent,
    // arm the drones and switch to offboard mode
    if (offboard_setpoint_counter_ == 50) {
      // Switch to offboard mode and arm the drones
      for (int i = 0; i < num_drones_; i++) {
        // Send command multiple times to ensure that the command is received
        int counter = 0;
        while (counter < 5) {
          arm(vehicle_command_publishers_[i]);
          offboard_mode(vehicle_command_publishers_[i]);
          counter++;
        }
      }

      // Set offboard_arm_sent to true
      offboard_arm_sent_ = true;
    }

    // The offboard control mode needs to be paired with a trajectory setpoint
    for (int i = 0; i < num_drones_; i++) {
      publish_offboard_control_mode(offboard_control_mode_publishers_[i]);

      // Till arm_offboard_sent is not set, publish default setpoints
      if (!offboard_arm_sent_)
        // Publish setpoint as a 5m altitude hold
        publish_trajectory_setpoint(trajectory_setpoint_publishers_[i], 0.0,
                                    0.0, -5.0, 0.0);

      // After arming and switching to offboard mode, publish the setpoints read
      // from the yaml file
      publish_default_setpoints();
    }

    // Stop the counter after reaching certain threshold
    if (offboard_setpoint_counter_ < 51) {
      offboard_setpoint_counter_++;
    }
  };

  // Create a timer with a 100ms period
  timer_ =
      this->create_wall_timer(std::chrono::milliseconds(100), timer_callback);
}

void ctrl::Control::publish_offboard_control_mode(
    rclcpp::Publisher<OffboardControlMode>::SharedPtr pub_) {
  OffboardControlMode msg;
  msg.position = true;
  msg.velocity = false;
  msg.acceleration = false;
  msg.attitude = false;
  msg.body_rate = false;
  msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
  pub_->publish(msg);
}

void ctrl::Control::publish_trajectory_setpoint(
    rclcpp::Publisher<TrajectorySetpoint>::SharedPtr pub_, float x, float y,
    float z, float yaw) {
  TrajectorySetpoint msg;
  msg.position = {x, y, z};
  msg.yaw = yaw;
  msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
  pub_->publish(msg);
}

void ctrl::Control::arm(rclcpp::Publisher<VehicleCommand>::SharedPtr pub_) {
  publish_vehicle_command(
      pub_, VehicleCommand::VEHICLE_CMD_COMPONENT_ARM_DISARM, 1.0);

  RCLCPP_INFO(this->get_logger(), "Arm command sent");
}

void ctrl::Control::offboard_mode(
    rclcpp::Publisher<VehicleCommand>::SharedPtr pub_) {
  publish_vehicle_command(pub_, VehicleCommand::VEHICLE_CMD_DO_SET_MODE, 1.0,
                          6.0);

  RCLCPP_INFO(this->get_logger(), "Offboard mode command sent");
}

void ctrl::Control::publish_vehicle_command(
    rclcpp::Publisher<VehicleCommand>::SharedPtr pub_, uint16_t command,
    float param1, float param2) {
  VehicleCommand msg;
  msg.param1 = param1;
  msg.param2 = param2;
  msg.command = command;
  msg.target_system = 0;
  msg.target_component = 1;
  msg.source_system = 1;
  msg.source_component = 1;
  msg.from_external = true;
  msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
  pub_->publish(msg);
}

void ctrl::Control::publish_default_setpoints() {
  // Iterate over the drones
  for (int i = 0; i < num_drones_; i++) {
    // Get the current setpoint to be published
    std::array<float, 4> setpoint = setpoints_[i][setpoint_counter_];

    // Publish the setpoint
    publish_trajectory_setpoint(trajectory_setpoint_publishers_[i], setpoint[0],
                                setpoint[1], setpoint[2], setpoint[3]);

    // Calculate the distance of the drone from its setpoint
    float distance =
        std::sqrt(std::pow(local_positions_[i][0] - setpoint[0], 2) +
                  std::pow(local_positions_[i][1] - setpoint[1], 2) +
                  std::pow(local_positions_[i][2] - setpoint[2], 2));

    // If the drone is close to its setpoint and has not reached its setpoint
    // before, set the reached_setpoint_ flag to true
    if (distance < 0.3 and !reached_setpoint_[i]) {
      reached_setpoint_[i] = true;
    }

    // If all the drones have reached their setpoints, increment the
    // setpoint_counter_, but stop it after reaching the last setpoint
    if (std::all_of(reached_setpoint_.begin(), reached_setpoint_.end(),
                    [](bool v) { return v; })) {
      setpoint_counter_++;
      if (setpoint_counter_ == num_setpoints_) {
        setpoint_counter_ = num_setpoints_ - 1;
      }

      std::cout << "Reached setpoint " << setpoint_counter_ << std::endl;

      // Reset the reached_setpoint_ flag
      std::fill(reached_setpoint_.begin(), reached_setpoint_.end(), false);
    }

    // // If the drone has not reached its setpoint
    // if(!reached_setpoint_[i]){
    //     // Print current position and setpoint
    //     // std::cout << "Drone " << i+1 << " current position: x=" <<
    //     local_positions_[i][0] << ", y=" << local_positions_[i][1] << ", z="
    //     << local_positions_[i][2] << std::endl;
    //     // std::cout << "Drone " << i+1 << " setpoint: x=" << setpoint[0] <<
    //     ", y=" << setpoint[1] << ", z=" << setpoint[2] << std::endl;

    //     // Print the distance from the setpoint
    //     std::cout << "Drone " << i+1 << " distance from setpoint: " <<
    //     distance << std::endl;
    // }
  }
}

void ctrl::Control::vehicle_local_position_callback(
    int drone_id, const VehicleLocalPosition::SharedPtr msg) {
  local_positions_[drone_id] = {msg->x, msg->y, msg->z};
  // std::cout << "Drone " << drone_id << " local position: x=" << msg->x << ",
  // y=" << msg->y << ", z=" << msg->z << std::endl;
}