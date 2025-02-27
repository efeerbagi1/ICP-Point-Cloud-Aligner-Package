#include "../include/icp_transformator/icpTransformator.hpp"

// STD
#include <string>

namespace icp_transformator
{

  IcpTransformator::IcpTransformator(ros::NodeHandle &nodeHandle) : nodeHandle_(nodeHandle)
  {
    // velodyne_cloud = boost::make_shared<pcl::PointCloud<pcl::PointXYZ>>();
    // world_cloud = boost::make_shared<pcl::PointCloud<pcl::PointXYZ>>();
    if (!readParameters())
    {
      ROS_ERROR("Could not read parameters.");
      ros::requestShutdown();
    }

    subscriber_velodyne = nodeHandle_.subscribe<sensor_msgs::PointCloud2>(subscriber_velodyne_Topic_, 1, &IcpTransformator::velodyneCloudCallback, this);
    subscriber_world = nodeHandle_.subscribe<sensor_msgs::PointCloud2>(subscriber_world_Topic_, 1, &IcpTransformator::worldCloudCallback, this);

    std::cout << "Subscribing to: " << subscriber_velodyne_Topic_ << " and " << subscriber_world_Topic_ << std::endl;

    publisher_aligned = nodeHandle_.advertise<sensor_msgs::PointCloud2>(publisher_aligned_final, 1);

    ROS_INFO("Successfully launched node.");
  }

  IcpTransformator::~IcpTransformator()
  {
    // Constructor
  }

  bool IcpTransformator::readParameters()
  {
    if (!nodeHandle_.getParam("subscriber_velodyne", subscriber_velodyne_Topic_))
    {
      //std::cout << "velodyneee" << std::endl;

      return false;
    }
    if (!nodeHandle_.getParam("subscriber_world", subscriber_world_Topic_))
    {
      std::cout << "sickkkkkk" << std::endl;

      return false;
    }
    //std::cout << "bbbbbbbbbbbbbbbbbbb" << std::endl;

    return true;
  }

  void IcpTransformator::velodyneCloudCallback(const sensor_msgs::PointCloud2ConstPtr &input_cloud)
  {
    ROS_INFO("callback 1 calisti");
    pcl::fromROSMsg(*input_cloud, *velodyne_cloud); // cloud1'e veri aktar
    ROS_INFO("Cloud Velodyne güncellendi! Nokta sayısı: %lu", velodyne_cloud->points.size());

    alignICP(); // 10hz
  }

  void IcpTransformator::worldCloudCallback(const sensor_msgs::PointCloud2ConstPtr &input_cloud)
  {
    ROS_INFO("callback 2 calisti");
    pcl::fromROSMsg(*input_cloud, *world_cloud); // cloud1'e veri aktar
    ROS_INFO("Cloud World güncellendi! Nokta sayısı: %lu", world_cloud->points.size());
    // 20hz
  }

  void IcpTransformator::alignICP()
  {
    //std::cout << "aaaaaaaaaaaa" << std::endl;
    if (velodyne_cloud->empty() || world_cloud->empty())
    {
      ROS_WARN("PointClouds are empty, skipping ICP alignment.");
      return;
    }

    while (ros::ok())
    {
      ros::spinOnce();;
      pcl::PointCloud<pcl::PointXYZ>::Ptr aligned_cloud(new pcl::PointCloud<pcl::PointXYZ>());
      pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> icp;
      icp.setInputSource(world_cloud);
      icp.setInputTarget(velodyne_cloud);
      icp.align(*aligned_cloud);

      if (icp.hasConverged())
      {
        Eigen::Matrix4f icp_transformation = icp.getFinalTransformation();
        Eigen::Matrix3f rotation_matrix = icp_transformation.block<3, 3>(0, 0);
        Eigen::Vector3f translation_vector = icp_transformation.block<3, 1>(0, 3);

        tf2::Matrix3x3 tf2_rotation(
            rotation_matrix(0, 0), rotation_matrix(0, 1), rotation_matrix(0, 2),
            rotation_matrix(1, 0), rotation_matrix(1, 1), rotation_matrix(1, 2),
            rotation_matrix(2, 0), rotation_matrix(2, 1), rotation_matrix(2, 2));

        tf2::Vector3 tf2_translation(translation_vector(0), translation_vector(1), translation_vector(2));
        tf2::Transform tf2_transform(tf2_rotation, tf2_translation);

        tf2::Transform inverse_tf2_transform = tf2_transform.inverse();
        tf2::Vector3 inv_translation = inverse_tf2_transform.getOrigin();
        tf2::Matrix3x3 inv_rotation(inverse_tf2_transform.getBasis());

        double roll, pitch, yaw;
        inv_rotation.getRPY(roll, pitch, yaw);

        std::cout << "Original Transformation Matrix:\n"
                  << icp_transformation << std::endl;
        std::cout << "Inverse Transform (Restored to Previous State):\n";
        std::cout << "Translation: x=" << inv_translation.x()
                  << ", y=" << inv_translation.y()
                  << ", z=" << inv_translation.z() << "\n";
        std::cout << "Rotation: roll=" << roll << ", pitch=" << pitch << ", yaw=" << yaw << " (radians)\n";

        sensor_msgs::PointCloud2 output;
        pcl::toROSMsg(*aligned_cloud, output);
        output.header.frame_id = "map";
        publisher_aligned.publish(output);
      }
    }
  }

} // namespace
