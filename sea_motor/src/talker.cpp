#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int32.h"
#include <termios.h>
#include <stdio.h>

#include <sstream>

std_msgs::Int32 nums;


int main(int argc, char **argv)
{
    ros::init(argc, argv, "talker");

    ros::NodeHandle n;

    ros::Publisher desire_position_pub = n.advertise<std_msgs::Int32>("desire_position", 1000);

    ros::Rate loop_rate(10);

    int count = 0;
    while (ros::ok())
    {
        std_msgs::String msg;
        std_msgs::Int32 msg2;

        //std::stringstream ss;
        std::cin >> nums.data;
       // msg.data = ss.str();

        msg2.data = nums.data;

        ROS_INFO("%d", msg2.data);

        desire_position_pub.publish(msg2);

        ros::spinOnce();

        loop_rate.sleep();
        ++count;

    }

    return 0;
}
