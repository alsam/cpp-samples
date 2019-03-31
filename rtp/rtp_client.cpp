// The MIT License (MIT)
//
// Copyright (c) 2019 Alexander Samoilov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE

#include <ros/ros.h>

#include <sstream>

#include <rwc_msgs/RobotCommand.h>
#include <rwc_msgs/RobotExecutorSrv.h>
#include <rwc_msgs/RobotStatusSrv.h>

#include <robot_connector/utils.h>
#include <rtp_simulator/SimpleRobotInterface.h>

using namespace std;
namespace pt = boost::property_tree;

map<string, rwc::RobotConfig> configs;

SimpleRobotInterface::Responce SimpleRobotInterface::executeTask(rwc_msgs::RobotTask&& task)
{
  auto status = this->start(task);
  status = this->execute(task);
}

template <typename T>
SimpleRobotInterface::Responce SimpleRobotInterface::start(T&& task)
{
  std::cout << "start\n";
}

template <typename T>
SimpleRobotInterface::Responce SimpleRobotInterface::execute(T&& task)
{
  std::cout << "execute\n";
}

rwc_msgs::RobotCommand toRobotCommand(const rwc::Operation operation, const rwc::RobotConfig robotConfig)
{
  rwc_msgs::RobotCommand cmd;

  cmd.robot_name = operation.arm;
  const auto& vals = operation.values;

  if (operation.type == "joints")
  {
    cmd.operation = rwc_msgs::RobotCommand::JOINTS;

    cmd.joint_trajectory.joint_names = robotConfig.joints;

    cmd.joint_trajectory.points.resize(1);
    auto &traj = cmd.joint_trajectory.points[0];
    // cannot apply `operator=` directly due to `std::vector<float>` vs. `std::vector<double>` types mismatch
    std::copy(vals.begin(), vals.end(), std::inserter(traj.positions, traj.positions.end()));
    traj.velocities = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };
  }
  else if (operation.type == "linear")
  {
    cmd.operation = rwc_msgs::RobotCommand::LINEAR;
  }
  else if (operation.type == "weld")
  {
    cmd.operation = rwc_msgs::RobotCommand::WELD_LINEAR;
  }
  else if (operation.type == "attach")
  {
    cmd.operation = rwc_msgs::RobotCommand::ATTACH;
  }
  else if (operation.type == "detach")
  {
    cmd.operation = rwc_msgs::RobotCommand::DETACH;
  }
  else {
    stringstream str;
    str << "unknown operation type " << operation.type;
    throw std::runtime_error(str.str());
  }

  switch (cmd.operation)
  {
    case rwc_msgs::RobotCommand::LINEAR: case rwc_msgs::RobotCommand::WELD_LINEAR:
      {
        rwc_msgs::EulerPose eu_pose;
        eu_pose.x = vals[0];
        eu_pose.y = vals[1];
        eu_pose.z = vals[2];
        eu_pose.w = vals[3];
        eu_pose.p = vals[4];
        eu_pose.r = vals[5];
        cmd.linear_poses.emplace_back(eu_pose);
      }
    default:
      break;
  }

  return cmd;
}

int main(int argc, char **argv)
{
  // initialize node
  ros::init(argc, argv, "rtp_sim_client");

  ros::NodeHandle nh;

  string configPath;

  if (ros::param::has("config"))
  {
    ros::param::get("config", configPath);
  }
  else
  {
    ROS_INFO("No robots config, skip connection");
    return 0;
  }

  pt::ptree pt;
  pt::read_json(configPath, pt);

  vector<rwc::RobotConfig> cRobots = rwc::parseRobotsConfig(pt.get_child("robots"));
  ROS_INFO("test %ld robots", cRobots.size());
  for (rwc::RobotConfig c : cRobots)
  {
    configs[c.name] = c;
  }

  vector<rwc_msgs::RobotErrorHandler> error_handlers;
  vector<rwc_msgs::RobotTask> tasks;

  for (const auto &cmd_entry : pt.get_child("commands"))
  {
    rwc::Operation operation;
    operation.arm = cmd_entry.second.get<string>("arm");
    operation.type = cmd_entry.second.get<string>("type");
    for (const auto &value : cmd_entry.second.get_child("values"))
      operation.values.push_back(value.second.get_value<float>());

    std::vector<rwc::Operation> operations = rwc::parseRobotsOperations(cmd_entry.second.get_child("operations"));

    if (operation.type == "task")
    {
      rwc_msgs::RobotTask robotTask;
      robotTask.task_id = (int)operation.values[0];
      robotTask.error_id = (int)operation.values[1];
      robotTask.robot_name = operation.arm;
      for (const rwc::Operation &o : operations)
      {
        rwc_msgs::RobotCommand cmd = toRobotCommand(o, configs[o.arm]);
        robotTask.commands.push_back(cmd);
      }
      tasks.push_back(robotTask);
    }
    else if (operation.type == "error_handler")
    {
      rwc_msgs::RobotErrorHandler error_handler;
      error_handler.error_id = (int)operation.values[0];
      error_handler.robot_name = operation.arm;
      for (const rwc::Operation &o : operations)
      {
        rwc_msgs::RobotCommand cmd = toRobotCommand(o, configs[o.arm]);
        error_handler.commands.push_back(cmd);
      }
      error_handlers.push_back(error_handler);
    }
  }

  try
  {
    SimpleRobotInterface robot_interface(std::move(error_handlers));
    for (auto &task : tasks)
    {
      robot_interface.executeTask(std::move(task));
    }
  } catch (const exception &e)
  {
    ROS_ERROR("failed execute test :: %s", e.what());
    return 1;
  }

  return 0;
}
