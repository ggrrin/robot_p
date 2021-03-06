#include <controller_manager/controller_manager.h>
#include <robot_p/robothw.h>
#include <ros/ros.h>

#include <chrono>
#include <thread>

void controlThread(ros::NodeHandle* nh, ros::Rate rate, robotp::RobotHW* robot,
                   controller_manager::ControllerManager* cm)
{
  auto last_time = std::chrono::steady_clock::now();

  while (nh->ok()) {
    // Calculate monotonic time elapsed
    auto this_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_duration = this_time - last_time;

    ros::Duration elapsed(elapsed_duration.count());
    last_time = this_time;

    // read odom from encoder
    robot->read(ros::Time::now(), elapsed);
    cm->update(ros::Time::now(), elapsed);
    // execute movement
    robot->write();
    rate.sleep();
  }
}

int main(int argc, char* argv[])
{
  // Initialize ROS node.
  ros::init(argc, argv, "robot_p_control");
  ros::NodeHandle controller_nh;
  ros::NodeHandle nh_private("~");

  int rate = 0;
  nh_private.param<int>("controller_rate", rate, 1);

  std::unique_ptr<robotp::RobotHW> robot(new robotp::RobotHW(nh_private));

  controller_manager::ControllerManager cm(robot.get(), controller_nh);

  std::thread control_loop(controlThread, &controller_nh, ros::Rate(rate),
                           robot.get(), &cm);

  // Foreground ROS spinner for ROS callbacks
  while (controller_nh.ok()) {
    ros::spinOnce();
  }
  control_loop.join();

  return 0;
}
