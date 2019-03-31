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

#include <robot_connector/msg_type.h>
#include <robot_connector/utils.h>

#include <industrial_msgs/RobotStatus.h>
#include <rwc_msgs/RobotStatusSrv.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <set>
#include <deque>
#include <utility>
#include <boost/asio.hpp>
#include <rtp_simulator/SimpleRobotInterface.h>

using namespace boost::asio;
namespace pt = boost::property_tree;

Buffer<RobotCommand> commandQueue;

class RobotService
{
// service type
public:
  enum ServiceType
  {
    TRAJECTORY_STREAMING, ROBOT_STATE,
  };

public:

  RobotService(ServiceType service_type)
  : service_type_(service_type)
  {}

  void StartHandling(std::shared_ptr<ip::tcp::socket> sock)
  {
    std::thread th([this, sock] { HandleClient(sock); });
    th.detach();
  }

// methods
private:

  struct __attribute__((__packed__)) PacketHeader
  {
    int32_t msg_len;
    int32_t msg_type;   // identifies type of message (standard and robot specific values)
    int32_t comm_type;
    int32_t reply_code; // only valid in service replies
  };

  template <typename T>
  PacketHeader makeHeader(rwc::MsgType msg_type)
  {
    namespace sm = industrial::simple_message;
    auto reply_header = PacketHeader { msg_len : sizeof(T)
                                               + sizeof(PacketHeader)
                                               - sizeof(int32_t),
                                       msg_type : toUType(msg_type),
                                       comm_type : sm::CommType::SERVICE_REPLY, // i.e. 3
                                       reply_code : sm::ReplyType::SUCCESS // i.e. 1
                                     };
    return reply_header;
  }

  struct __attribute__((__packed__)) TaskMessage
  {
    int32_t task_id;
    int32_t error_handler;
    int32_t count;
  };

  constexpr static uint32_t MAX_JOINTS = 10;
  struct __attribute__((__packed__)) JointResponceMessage // 44 Bytes
  {
    int32_t sequence_id;
    float joints[MAX_JOINTS];

    JointResponceMessage(int32_t id)
      : sequence_id(id), joints{0.f,}
    {}
  };

  struct __attribute__((__packed__)) JointTrajPtMessage
    : public JointResponceMessage // 56 Bytes
  {
    float velocity;
    float duration;

    JointTrajPtMessage(int32_t id = 0)
      : JointResponceMessage(id)
    {}
  };

  struct __attribute__((__packed__)) TaskResult
  {
    int32_t task_id;
    int32_t sequence_id;
    int32_t error_code;
  };

  PacketHeader readHeader(std::istream &is)
  {
    PacketHeader header;
    is.read((char*)&header, sizeof(PacketHeader));
    return header;
  }

  void readBody(int32_t msg_type, std::istream &is)
  {
    if (msg_type == toUType(rwc::MsgType::TASK))
    {
      TaskMessage task_message;
      is.read((char*)&task_message, sizeof(TaskMessage));
      std::cout << "task_id: " << task_message.task_id
                << " error_handler: " << task_message.error_handler
                << " count: " << task_message.count
                << std::endl;

      current_task_id_.store(task_message.task_id);
      commandQueue.add(RobotCommand { task_id : task_message.task_id,
                                      sequence_id : -1,
                                      command_type : 0 } );
    }
    else if (msg_type == toUType(rwc::MsgType::JOINT_TRAJ_PT))
    {
      JointTrajPtMessage joint_traj_pt_msg;
      industrial::joint_data::JointData joint_data;
      is.read((char*)&joint_traj_pt_msg, sizeof(JointTrajPtMessage));
      std::cout << "joint_traj_pt_msg: sequence_id: " << joint_traj_pt_msg.sequence_id
                << " velocity: " << joint_traj_pt_msg.velocity << "\n joints : [ ";
      for (size_t i = 0; i < MAX_JOINTS; ++i)
      {
        std::cout << joint_traj_pt_msg.joints[i] << " ";
      }
      std::cout << "]\n";

      commandQueue.add(RobotCommand { task_id : current_task_id_.load(),
                                      sequence_id : joint_traj_pt_msg.sequence_id,
                                      command_type : 1 } );

    }
  }

  void readPacket(std::shared_ptr<ip::tcp::socket> sock)
  {
    boost::asio::streambuf b;

    // reserve 512 bytes in output sequence
    auto bufs = b.prepare(512);
    size_t n = sock->receive(bufs);
    std::cout << "read packed n: " << n << " bytes\n";

    // received data is "committed" from output sequence to input sequence
    b.commit(n);

    std::istream is(&b);

    PacketHeader header = readHeader(is);
    std::cout
      << "msg_len: " << header.msg_len
      << " msg_type: " << rwc::MsgName(header.msg_type) << "(" << header.msg_type << ")"
      << " comm_type: " << header.comm_type
      << " reply_code: " << header.reply_code << std::endl;

    readBody(header.msg_type, is);
  }

  /// the main workforce for communicating with robot-connector
  void HandleClient(std::shared_ptr<ip::tcp::socket> sock)
  {
    try
    {
      if (service_type_ == TRAJECTORY_STREAMING)
      {
        for (;;)
        {
          readPacket(sock);

          auto reply_header = makeHeader<JointResponceMessage>(rwc::MsgType::JOINT_RESPONCE);
          boost::asio::write(*sock.get(), boost::asio::buffer((void*)&reply_header, sizeof(reply_header)));

          JointResponceMessage reply(1);
          boost::asio::write(*sock.get(), boost::asio::buffer((void*)&reply, sizeof(reply)));
        }
      }
      else if (service_type_ == ROBOT_STATE)
      {
        using namespace std::chrono_literals;
        for (;;)
        {
          constexpr size_t status_freq = 10;
          for (size_t i = 0; i < status_freq; ++i)
          {
            industrial_msgs::RobotStatus status;
            auto reply_header = makeHeader<decltype(status)>(rwc::MsgType::STATUS);
            boost::asio::write(*sock.get(), boost::asio::buffer((void*)&reply_header, sizeof(reply_header)));
            boost::asio::write(*sock.get(), boost::asio::buffer((void*)&status, sizeof(status)));
          }

          if (!commandQueue.empty())
          {
            RobotCommand cmd = commandQueue.remove();
            if (cmd.command_type != 0) // TODO for task command, enum instead of constant
            {
              auto task_result = TaskResult { task_id : cmd.task_id, sequence_id : /* cmd.sequence_id */ 0, error_code : 0 };
              //auto task_result = TaskResult { task_id : current_task_id_, sequence_id : /* cmd.sequence_id */ 0, error_code : 0 };
              auto task_reply_header = makeHeader<decltype(task_result)>(rwc::MsgType::TASK_RESULT);
              boost::asio::write(*sock.get(), boost::asio::buffer((void*)&task_reply_header, sizeof(task_reply_header)));
              boost::asio::write(*sock.get(), boost::asio::buffer((void*)&task_result, sizeof(task_result)));
            }
          }
          //std::this_thread::sleep_for(200ms);
        }
      }
    }
    catch (boost::system::system_error &e)
    {
      std::cout << "Error occured! Error code = "
                << e.code() << ". Message: "
                << e.what();
    }

    onFinish();
  }

  /// cleanup.
  void onFinish() {
    delete this;
  }

// data members
private:

  std::shared_ptr<ip::tcp::socket> sock_;

  ServiceType service_type_;

  std::atomic<int32_t> current_task_id_;
};

class RobotAcceptor {
public:

  RobotAcceptor(io_service &ios, uint16_t port_num)
    : ios_(ios),
      acceptor_(ios_,
                ip::tcp::endpoint(ip::address_v4::any(),
                                  port_num))
  {
    acceptor_.listen();
  }

  void Accept(RobotService::ServiceType service_type)
  {
    auto sock = std::make_shared<ip::tcp::socket>(ios_);
    acceptor_.accept(*sock.get());
    (new RobotService(service_type))->StartHandling(sock);
  }

// methods
private:

// data members
private:

  io_service &ios_;
  ip::tcp::acceptor acceptor_;
};

class RobotSimulatorServer
{
public:

  RobotSimulatorServer()
    : stop_(false) {}

  void Start(uint16_t port_num, RobotService::ServiceType service_type)
  {
    thread_.reset(new std::thread(
          [this, port_num, service_type]
          {
            this->Run(port_num, service_type);
          })
        );
  }

  void Stop()
  {
    stop_.store(true);
    thread_->join();
  }

// methods
private:
  void Run(uint16_t port_num, RobotService::ServiceType service_type)
  {
    RobotAcceptor acc(ios_, port_num);

    while (!stop_.load())
    {
      acc.Accept(service_type);
    }
  }

// data members
private:

  std::unique_ptr<std::thread> thread_;
  std::atomic<bool> stop_;
  io_service ios_;

};

int main(int argc, char** argv)
{

  // initialize node
  ros::init(argc, argv, "rtp_simulator");

  std::string configPath;

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

  try
  {
    std::vector<rwc::RobotConfig> cRobots = rwc::parseRobotsConfig(pt.get_child("robots"));
    ROS_INFO("succesfully parsed %zu robots", cRobots.size());

    for (rwc::RobotConfig &conf: cRobots)
    {
      if (!conf.connect)
      {
        ROS_INFO("skip %s::%s robot", conf.name.c_str(), conf.ip.c_str());
        continue;
      }

      std::cout << "establishing TCP connection to ip: " << conf.ip
                << " state port: " << conf.state_port
                << " joints ports: " << conf.joints_port << std::endl;

      // creating two servers per robot: joints and state
      try
      {
        RobotSimulatorServer* srv = new RobotSimulatorServer[2];
        ROS_INFO("starting joints server");
        srv[0].Start(conf.joints_port, RobotService::TRAJECTORY_STREAMING);
        ROS_INFO("starting state server");
        srv[1].Start(conf.state_port, RobotService::ROBOT_STATE);

      }
      catch (boost::system::system_error &e)
      {
        std::cout << "Error occured! Error code = "
                  << e.code() << ". Message: "
                  << e.what();
      }
    }

    ros::waitForShutdown();

  }
  catch (const std::exception &e)
  {
    ROS_INFO("err parse config : %s", e.what());
  }


  return 0;
}

