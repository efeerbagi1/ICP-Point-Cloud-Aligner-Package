#include "../include/icp_transformator/icpTransformator.hpp"

int main(int argc, char** argv)
{
  ros::init(argc, argv, "IcpTransformator");
  ros::NodeHandle nodeHandle("~");

  icp_transformator::IcpTransformator icpTransformator(nodeHandle);

  ros::spin();
  return 0;
}