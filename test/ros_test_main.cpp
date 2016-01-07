#define DEBUG
#undef NDEBUG

#include <chrono>
#include <memory>

#include <MatlabInterfaceTests.hpp>
#include <MatFileTest.hpp>

#include <ros/ros.h>

#include <std_msgs/String.h>

#include <boost/thread.hpp>

void dummyCallback(const std_msgs::StringConstPtr& str)
{
	std::cout << "Hello!" << std::endl;
	return;
}

void dummyFunction()
{
	return;
}

/// Run all the tests that were declared with TEST()
int main(int argc, char **argv){

	ros::init(argc, argv, "matlab_cpp_interface_test");
	ros::NodeHandle nh;

	boost::thread t(dummyFunction);

	ros::Publisher pub = nh.advertise<std_msgs::String>("dummy", 5);
	ros::Subscriber sub = nh.subscribe("dummy", 1, dummyCallback);

	std_msgs::String string;
	string.data = "test";
	pub.publish(string);

	std::shared_ptr<int> a(new int);
	*a = 1;
	ros::Duration(*a).sleep();

	ros::spin();

	std::cout<<"Starting matlab engine test"<<std::endl;
	testInit();
	testCommand();
	testPut();
	testPutEigen();
	testGet();
	testGetEigen();
	testMixedPut();
	testGui();
	std::cout<<"Completed matlab engine test"<<std::endl;

	std::cout<<"Starting mat-file test"<<std::endl;
	testOpenClose();
	testWriteRead();
	testWriteEigen();
	testWriteScalarVectors();
	std::cout<<"Completed mat-file test"<<std::endl;
}
