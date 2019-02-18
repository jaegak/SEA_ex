#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int32.h"
#include <string>
#include <stdio.h>
#include <string.h>
std::string mssg;

std::string dec_to_hex(int dec)
{
    char hex1[]="0x00";
    char hex2[]="0x00";
    char hex3[]="0x00";
    char hex4[]="0x00";
    int mod = 0;

    if (dec >= 256*256*256)
    {
        int hex1_dec = dec / (256*256*256);
        mod = hex1_dec % 16;
        if (mod <10)
        {
            hex1[3] = 48 + mod;
        }
        else
        {
            hex1[3] = 65 + (mod - 10);
        }
        hex1_dec = hex1_dec / 16;
        mod = hex1_dec % 16;
        if (mod <10)
        {
            hex1[2] = 48 + mod;
        }
        else
        {
            hex1[2] = 65 + (mod - 10);
        }
    }

    if (dec >= 256*256)
    {
        int hex2_dec = dec / (256*256);
        mod = hex2_dec % 16;
        if (mod <10)
        {
            hex2[3] = 48 + mod;
        }
        else
        {
            hex2[3] = 65 + (mod - 10);
        }
        hex2_dec = hex2_dec / 16;
        mod = hex2_dec % 16;
        if (mod <10)
        {
            hex2[2] = 48 + mod;
        }
        else
        {
            hex2[2] = 65 + (mod - 10);
        }
    }

    if (dec >= 256)
    {
        int hex3_dec = dec / 256;
        mod = hex3_dec % 16;
        if (mod <10)
        {
            hex3[3] = 48 + mod;
        }
        else
        {
            hex3[3] = 65 + (mod - 10);
        }
        hex3_dec = hex3_dec / 16;
        mod = hex3_dec % 16;
        if (mod <10)
        {
            hex3[2] = 48 + mod;
        }
        else
        {
            hex3[2] = 65 + (mod - 10);
        }
    }

    int hex4_dec = dec;
    mod = hex4_dec % 16;
    if (mod <10)
    {
        hex4[3] = 48 + mod;
    }
    else
    {
        hex4[3] = 65 + (mod - 10);
    }
    hex4_dec = hex4_dec / 16;
    mod = hex4_dec % 16;
    if (mod <10)
    {
        hex4[2] = 48 + mod;
    }
    else
    {
        hex4[2] = 65 + (mod - 10);
    }
    char hexsum[]="0x00000000";
    hexsum[0]=hex1[0];
    hexsum[1]=hex1[1];
    hexsum[2]=hex4[2];
    hexsum[3]=hex4[3];
    hexsum[4]=hex3[2];
    hexsum[5]=hex3[3];
    hexsum[6]=hex2[2];
    hexsum[7]=hex2[3];
    hexsum[8]=hex1[2];
    hexsum[9]=hex1[3];

    std::string hexx = hexsum;

    return hexx;
}

void chatterCallback(const std_msgs::Int32::ConstPtr& msg)
{
  mssg = dec_to_hex(msg->data);
  //std::cout<<mssg<<std::endl;

  ROS_INFO("I heard: [%s]", mssg.c_str());
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "listener");

  ros::NodeHandle n;

  std_msgs::Int32 nums;
  //std::cin >> nums.data;
  ros::Subscriber sub = n.subscribe("desire_position", 1000, chatterCallback);
  ros::spin();

  return 0;
}