#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include "std_msgs/String.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include <net/if.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#define _USE_MATH_DEFINES
#include <math.h>


#define CANID_DELIM '#'
#define DATA_SEPERATOR '.'

std::string desired_hex;
std::string magnetic_hex;
int s;

using namespace std;

string Control_Endless = "3F00";
string Control_Enable = "0F00";
    string rtr;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct canfd_frame frame_fd;
    int required_mtu;
    struct ifreq ifr;
    string data;

    struct canfd_frame frame_get;

    const char *ifname = "slcan0";

    struct iovec iov;
    struct msghdr canmsg;
    char ctrlmsg[CMSG_SPACE(sizeof(struct timeval) + 3*sizeof(struct timespec) + sizeof(__u32))];



unsigned short COB_ID[4] = {0x201, 0x301, 0x401, 0x501};

unsigned char asc2nibble(char c) {

    if ((c >= '0') && (c <= '9'))
        return c - '0';

    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;

    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;

    return 16; /* error */
}

int hexstring2data(char *arg, unsigned char *data, int maxdlen) {

    int len = strlen(arg);
    int i;
    unsigned char tmp;

    if (!len || len%2 || len > maxdlen*2)
        return 1;

    memset(data, 0, maxdlen);

    for (i=0; i < len/2; i++) {

        tmp = asc2nibble(*(arg+(2*i)));
        if (tmp > 0x0F)
            return 1;

        data[i] = (tmp << 4);

        tmp = asc2nibble(*(arg+(2*i)+1));
        if (tmp > 0x0F)
            return 1;

        data[i] |= tmp;
    }

    return 0;
}

int parse_canframe(char *cs, struct canfd_frame *cf) {
    /* documentation see lib.h */

    int i, idx, dlen, len;
    int maxdlen = CAN_MAX_DLEN;
    int ret = CAN_MTU;
    unsigned char tmp;

    len = strlen(cs);
    //printf("'%s' len %d\n", cs, len);

    memset(cf, 0, sizeof(*cf)); /* init CAN FD frame, e.g. LEN = 0 */

    if (len < 4)
        return 0;

    if (cs[3] == CANID_DELIM) { /* 3 digits */

        idx = 4;
        for (i=0; i<3; i++){
            if ((tmp = asc2nibble(cs[i])) > 0x0F)
                return 0;
            cf->can_id |= (tmp << (2-i)*4);
        }

    } else if (cs[8] == CANID_DELIM) { /* 8 digits */

        idx = 9;
        for (i=0; i<8; i++){
            if ((tmp = asc2nibble(cs[i])) > 0x0F)
                return 0;
            cf->can_id |= (tmp << (7-i)*4);
        }
        if (!(cf->can_id & CAN_ERR_FLAG)) /* 8 digits but no errorframe?  */
            cf->can_id |= CAN_EFF_FLAG;   /* then it is an extended frame */

    } else
        return 0;

    if((cs[idx] == 'R') || (cs[idx] == 'r')){ /* RTR frame */
        cf->can_id |= CAN_RTR_FLAG;

        /* check for optional DLC value for CAN 2.0B frames */
        if(cs[++idx] && (tmp = asc2nibble(cs[idx])) <= CAN_MAX_DLC)
            cf->len = tmp;

        return ret;
    }

    if (cs[idx] == CANID_DELIM) { /* CAN FD frame escape char '##' */

        maxdlen = CANFD_MAX_DLEN;
        ret = CANFD_MTU;

        /* CAN FD frame <canid>##<flags><data>* */
        if ((tmp = asc2nibble(cs[idx+1])) > 0x0F)
            return 0;

        cf->flags = tmp;
        idx += 2;
    }

    for (i=0, dlen=0; i < maxdlen; i++){

        if(cs[idx] == DATA_SEPERATOR) /* skip (optional) separator */
            idx++;

        if(idx >= len) /* end of string => end of data */
            break;

        if ((tmp = asc2nibble(cs[idx++])) > 0x0F)
            return 0;
        cf->data[i] = (tmp << 4);
        if ((tmp = asc2nibble(cs[idx++])) > 0x0F)
            return 0;
        cf->data[i] |= tmp;
        dlen++;
    }
    cf->len = dlen;

    return ret;
}

void LogInfo(string message)
{
    cout << message << endl;
}


template <typename T>
std::string dec_to_hex(T dec, int NbofByte){
    std::stringstream stream_HL;
    string s, s_LH;
    stream_HL << std::setfill ('0') << std::setw(sizeof(T)*2) <<std::hex << dec;
    s = stream_HL.str();
    for (int i=0; i<NbofByte; i++){
        s_LH.append(s.substr(2*(NbofByte-1-i),2));
    }
    return s_LH;
}

int hexarray_to_int(unsigned char *buffer){
//    int length = sizeof(buffer)/ sizeof(*buffer);
    int length = 2;
    int hextoint = 0;
    for (int i=0; i<length; i++)
    {
        hextoint += (buffer[i] << 8*i);
    }

    return hextoint;
}

int hexarray_to_int4(unsigned char *buffer){
//    int length = sizeof(buffer)/ sizeof(*buffer);
    int length = 4;
    int hextoint = 0;
    for (int i=0; i<length; i++)
    {
        hextoint += (buffer[i] << 8*i);
    }

    return hextoint;
}

std::string stringappend(string a, string b)
{
    string s;
    s.append(a);
    s.append(b);
    return s;
}

void commandCallback(const std_msgs::Int32::ConstPtr& msg)
{
    ros::NodeHandle n;
     desired_hex= dec_to_hex(msg->data, 4);
     ROS_INFO("I heard: [%s]", desired_hex.c_str());
// Motor move
    char hexsum[10];
    strcpy(hexsum,desired_hex.c_str());
    char hex1[]="0x00";
    char hex2[]="0x00";
    char hex3[]="0x00";
    char hex4[]="0x00";

    std::stringstream str1;
    std::stringstream str2;
    std::stringstream str3;
    std::stringstream str4;

    hex1[2]=hexsum[0];
    hex1[3]=hexsum[1];
    hex2[2]=hexsum[2];
    hex2[3]=hexsum[3];
    hex3[2]=hexsum[4];
    hex3[3]=hexsum[5];
    hex4[2]=hexsum[6];
    hex4[3]=hexsum[7];

    str1 << hex1;
    str2 << hex2;
    str3 << hex3;
    str4 << hex4;
    int value1;
    int value2;
    int value3;
    int value4;
    str1 >> std::hex >> value1;
    str2 >> std::hex >> value2;
    str3 >> std::hex >> value3;
    str4 >> std::hex >> value4;

    LogInfo("motor move");
    frame.can_id  = 0x501;
    frame.can_dlc = 6;
    frame.data[0] = 0x0F;
    frame.data[1] = 0x00;
    frame.data[2] = value1;
    frame.data[3] = value2;
    frame.data[4] = value3;
    frame.data[5] = value4;
    write(s, &frame, sizeof(struct can_frame));
    ros::Duration(1).sleep();

}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "motor_listen");
    ros::NodeHandle n;
//    ros::Publisher encoder_pub = n.advertise<std_msgs::Int32>("encoder", 1000);
//    ros::Publisher magnetic_pub = n.advertise<std_msgs::Int32>("magnetic", 1000);



    if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Error while opening socket");
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

//    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error in socket bind");
        return -2;
    }


    iov.iov_base = &frame_get;
    canmsg.msg_name = &addr;
    canmsg.msg_iov = &iov;
    canmsg.msg_iovlen = 1;


    iov.iov_len = sizeof(frame_get);
    canmsg.msg_control = &ctrlmsg;
    canmsg.msg_namelen = sizeof(addr);
    canmsg.msg_controllen = sizeof(ctrlmsg);
    canmsg.msg_flags = 0;

    ros::Subscriber sub = n.subscribe("desire_velocity", 1000, commandCallback);

    ros::spin();

    return 0;
}