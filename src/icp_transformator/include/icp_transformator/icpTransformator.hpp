#pragma once

#ifndef ICPTRANSFORMATOR_HPP
#define ICPTRANSFORMATOR_HPP

#include <ros/ros.h>
#include "std_msgs/String.h"
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <boost/foreach.hpp>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/io/pcd_io.h>
#include <pcl/registration/icp.h>
#include <iostream>
#include <Eigen/Dense>
#include <tf2/LinearMath/Transform.h>
#include <tf2/LinearMath/Quaternion.h>

namespace icp_transformator
{

    class IcpTransformator
    {

    public:
        IcpTransformator(ros::NodeHandle &nodeHandle);

        ~IcpTransformator();

    private:
        void velodyneCloudCallback(const sensor_msgs::PointCloud2ConstPtr &input_cloud);
        void worldCloudCallback(const sensor_msgs::PointCloud2ConstPtr &input_cloud);

        bool readParameters();

        void alignICP();

        ros::NodeHandle &nodeHandle_;

        ros::Subscriber subscriber_velodyne;
        std::string subscriber_velodyne_Topic_;

        ros::Subscriber subscriber_world;
        std::string subscriber_world_Topic_;

        ros::Publisher publisher_aligned;
        std::string publisher_aligned_final;

        pcl::PointCloud<pcl::PointXYZ>::Ptr velodyne_cloud = boost::make_shared<pcl::PointCloud<pcl::PointXYZ>>();
        pcl::PointCloud<pcl::PointXYZ>::Ptr world_cloud = boost::make_shared<pcl::PointCloud<pcl::PointXYZ>>();
    };
}

#endif