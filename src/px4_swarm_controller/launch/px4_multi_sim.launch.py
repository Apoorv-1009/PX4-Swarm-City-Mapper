import yaml
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def parse_swarm_config(config_file):

    swarm_config = config_file["initial_positions"]
    # Extract all initial poses
    initial_poses_string = "\""
    initial_poses_dict = dict()
    for idx, item in enumerate(swarm_config.values()):
        initial_pose = item["initial_pose"]
        initial_poses_dict["px4_" + str(idx + 1)] = initial_pose
        initial_poses_string += str(initial_pose["x"]) + "," + str(initial_pose["y"]) + "|"
    initial_poses_string = initial_poses_string[:-1] + "\""

    return len(swarm_config), initial_poses_string, initial_poses_dict


def generate_launch_description():
    ld = LaunchDescription()
    # Extract information from configuration files

    # Swarm information
    with open('src/px4_swarm_controller/config/config.yaml', 'r') as swarm_file:
        swarm_config_data = yaml.safe_load(swarm_file)
    nb_drones, initial_poses, initial_poses_dict = parse_swarm_config(swarm_config_data)
    swarm_file.close()

    print(nb_drones)

    ld.add_action(
        Node(
            package='px4_swarm_controller',
            executable='simulation_node.py',
            name='simulation_node',
            parameters=[{'nb_vehicles': nb_drones,'initial_pose': initial_poses}]
        )
    )
    return ld
