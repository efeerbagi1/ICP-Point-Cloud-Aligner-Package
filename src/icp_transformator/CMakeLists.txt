cmake_minimum_required(VERSION 2.8.3)
project(icp_transformator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-Wall -Werror)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy std_msgs
  genmsg sensor_msgs
  pcl_ros
  pcl_conversions
  roslint)
find_package(PCL 1.8 REQUIRED)
find_package(Eigen3 REQUIRED)

catkin_package(
  INCLUDE_DIRS
  include
  ${EIGEN3_INCLUDE_DIR}
  LIBRARIES
  icp_transformator_core
  CATKIN_DEPENDS
  roscpp
  sensor_msgs
  DEPENDS
  ## find_package(Eigen3) provides a non standard EIGEN3_INCLUDE_DIR instead of Eigen3_INCLUDE_DIRS.
  ## Therefore, the DEPEND does not work as expected and we need to add the directory to the INCLUDE_DIRS
  # Eigen3

  ## Boost is not part of the DEPENDS since it is only used in source files,
  ## Dependees do not depend on Boost when they depend on this package.
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
)

link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

# add_library(icp_transformator_core
#    src/Algorithm.cpp
# )

# target_compile_features(icp_transformator_core INTERFACE cxx_std_11)

add_executable(icp_transformator_core
  src/icp_transformator_node.cpp
  src/IcpTransformator.cpp
)

target_link_libraries(icp_transformator_core
${catkin_LIBRARIES}
${PCL_LIBRARIES}
)

# target_link_libraries(icp_transformator
# icp_transformator_core
# ${catkin_LIBRARIES}
# ${PCL_LIBRARIES}
# )

add_dependencies(icp_transformator_core
  ${catkin_EXPORTED_TARGETS}
)

target_compile_features(icp_transformator_core INTERFACE cxx_std_11)

install(
  TARGETS icp_transformator_core
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

# Mark cpp header files for installation
install(
  DIRECTORY include/icp_transformator_core/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.hpp"
)

# Mark other files for installation
install(
  DIRECTORY doc
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

roslint_cpp()

