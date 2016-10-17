#include <iostream>
#include <omd/opto.h>
#include <unistd.h>

#include "ros/ros.h"
#include "geometry_msgs/WrenchStamped.h"
#include "std_msgs/String.h"
#include <sstream>


void MySleep(unsigned long p_uMillisecs)
{
	usleep(p_uMillisecs * 1000);
}

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "talker");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle nw;
  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
	// Change this to advertise the data type as wrench stamped
	geometry_msgs::WrenchStamped mywrench;  
	mywrench.header.frame_id="0";
	mywrench.wrench.torque.x=0;
	mywrench.wrench.torque.y=0;
	mywrench.wrench.torque.z=0;

	ros::Publisher wrench_pub=nw.advertise<geometry_msgs::WrenchStamped>("WrenchData",3333);

	ros::Rate loop_rate(333);

	OptoDAQ daq;
	OptoPorts ports;

	MySleep(1000); // We wait some ms to be sure about OptoPorts enumerated PortList
	
	OPort* portlist=ports.listPorts(true);
if (ports.getLastSize()>0)
    {
    	daq.open(portlist[0]);
}
OptoPackage pack3D;

  while (ros::ok())
  {
   
            int size=daq.read(pack3D,false);	// Reading Sensor #0 (up to 16 Sensors)
	mywrench.header.stamp=ros::Time::now();
	
	mywrench.wrench.force.x=pack3D.x*1000/401.7;
	mywrench.wrench.force.y=pack3D.y*1000/401.7;
	mywrench.wrench.force.z=pack3D.z*1000/401.7;

wrench_pub.publish(mywrench);
    ros::spinOnce();
    loop_rate.sleep();
  }
daq.close();
  return 0;
}
