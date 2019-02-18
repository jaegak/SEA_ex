cmake_minimum_required(VERSION 2.8.3)
project(sea_motor)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  genmsg
)

add_message_files(DIRECTORY msg FILES Num.msg)
add_service_files(DIRECTORY srv FILES AddTwoInts.srv)

generate_messages(DEPENDENCIES std_msgs)

catkin_package()

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(talker src/talker.cpp)
target_link_libraries(talker ${catkin_LIBRARIES})
add_dependencies(talker sea_motor_generate_messages_cpp)

add_executable(sensor_talker src/sensor_talker.cpp)
target_link_libraries(sensor_talker ${catkin_LIBRARIES})
add_dependencies(sensor_talker sea_motor_generate_messages_cpp)

add_executable(listener src/listener.cpp)
target_link_libraries(listener ${catkin_LIBRARIES})
add_dependencies(listener sea_motor_generate_messages_cpp)

add_executable(P_listen src/P_listen.cpp)
target_link_libraries(P_listen ${catkin_LIBRARIES})
add_dependencies(P_listen sea_motor_generate_messages_cpp)

add_executable(listener2 src/listener2.cpp)
target_link_libraries(listener2 ${catkin_LIBRARIES})
add_dependencies(listener2 sea_motor_generate_messages_cpp)
