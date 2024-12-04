#!/bin/bash
#
# This script should be invoked by "ros2 run" and after the unit test
# has been run.
#
set -ue -o pipefail             # stop at the first error

PROG_DIR=$(dirname $(readlink -f "$0")) # where is the program located
EXEC_DIR=$(pwd -P)                      # where are we executing from
PROG_NAME=$(basename "$0")              # the program name without the path
echo
echo "PROG_DIR = $PROG_DIR"
echo "EXEC_DIR = $EXEC_DIR"
echo "PROG_NAME = $PROG_NAME"
echo "PARAMS = $@"
echo

# 1.) Get the ros package name
ROS_PACKAGE_NAME=$(basename $PROG_DIR)
echo "ROS_PACKAGE_NAME = $ROS_PACKAGE_NAME"

# 2.) Generate report info
BUILD_DIR=$EXEC_DIR/build/$ROS_PACKAGE_NAME/
echo "BUILD_DIR = $BUILD_DIR"
rm -f $BUILD_DIR/coverage.info
lcov --capture --directory $BUILD_DIR --output-file $BUILD_DIR/coverage.info

# 3.) Exclude some files from the reoport
rm -f $BUILD_DIR/coverage_cleaned.info
lcov --remove $BUILD_DIR/coverage.info \
     '/opt/*' \
     '/usr/*' \
     '*rclcpp/*' \
     '*libstatistics_collector/*' \
     '*rosidl_runtime*' \
     '*rcl_interfaces*' \
     '*rmw/rmw/*' \
     '*tracetools/*' \
     '*_msgs/*' \
     '*/gtest*' \
     'gtest/*' \
     --output-file $BUILD_DIR/coverage_cleaned.info; \
mv $BUILD_DIR/coverage_cleaned.info $BUILD_DIR/test_coverage.info


# 4.) Finally generate the coverage report
rm -rf $BUILD_DIR/ROS_PACKAGE_NAME/test_coverage/
genhtml --output-directory \
        $BUILD_DIR/test_coverage \
        $BUILD_DIR/test_coverage.info 
echo "Code Coverage generated:"
echo "     $BUILD_DIR/test_coverage.info"
echo "     $BUILD_DIR/test_coverage/index.html"
echo
echo "To view the report, do:"
echo "   open $BUILD_DIR/test_coverage/index.html"

