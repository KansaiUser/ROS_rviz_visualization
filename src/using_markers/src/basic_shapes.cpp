/*
* ROS Tutorial http://wiki.ros.org/rviz/Tutorials/Markers%3A%20Basic%20Shapes
*/

#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <sensor_msgs/Image.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

int main( int argc, char** argv )
{
  ros::init(argc, argv, "basic_shapes");
  ros::NodeHandle n;
  ros::Rate r(1);
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);

  ros::Publisher image_pub = n.advertise<sensor_msgs::Image>("visualization_image",1);

  // Set our initial shape type to be a cube
  uint32_t shape = visualization_msgs::Marker::CUBE;

  while (ros::ok())
  {
    visualization_msgs::Marker marker;
    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "/my_frame";
    marker.header.stamp = ros::Time::now();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one
    marker.ns = "basic_shapes";
    marker.id = 0;

    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
    marker.type = shape;

    // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
    marker.action = visualization_msgs::Marker::ADD;

    // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    marker.pose.position.x = 0;
    marker.pose.position.y = 0;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;

    // Set the scale of the marker -- 1x1x1 here means 1m on a side
    marker.scale.x = 1.0;
    marker.scale.y = 1.0;
    marker.scale.z = 1.0;

    // Set the color -- be sure to set alpha to something non-zero!
    marker.color.r = 0.0f;
    marker.color.g = 1.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;

    marker.text = "Hello Robot World!";

    marker.lifetime = ros::Duration();

    sensor_msgs::Image image;

//
 /*   ros::Time timeNow = ros::Time::now();
    std::string frame = "camera";

    image.header.stamp = timeNow;
    image.header.frame_id = frame;

    // Fill the left image message
    sensor_msgs::fillImage( image,
                            sensor_msgs::image_encodings::MONO8,
                            240, // height
                            320, // width
                            320, // stepSize
                            pFrameData->leftData);


    sensor_msgs::clearImage(image);
   */ 
//
    cv_bridge::CvImage cv_image;
    cv_image.image = cv::imread("/root/test_dir/Test.png",CV_LOAD_IMAGE_COLOR);
    cv_image.encoding = "bgr8";
    //sensor_msgs::Image ros_image;
    cv_image.toImageMsg(image);
//
    // Publish the marker
    while (marker_pub.getNumSubscribers() < 1)
    {
      if (!ros::ok())
      {
        return 0;
      }
      ROS_WARN_ONCE("Please create a subscriber to the marker");
      sleep(1);
    }
    marker_pub.publish(marker);

    image_pub.publish(image);

    //ROS_INFO("[Marker] The text is [%s]\n", marker.text.c_str());// msg->data.c_str());    

    // Cycle between different shapes
    switch (shape)
    {
    case visualization_msgs::Marker::CUBE:
      shape = visualization_msgs::Marker::SPHERE;
      break;
    case visualization_msgs::Marker::SPHERE:
      shape = visualization_msgs::Marker::ARROW;
      break;
    case visualization_msgs::Marker::ARROW:
      shape = visualization_msgs::Marker::CYLINDER;
      break;
    case visualization_msgs::Marker::CYLINDER:
      shape = visualization_msgs::Marker::TEXT_VIEW_FACING;
      break;
    case visualization_msgs::Marker::TEXT_VIEW_FACING:
      shape = visualization_msgs::Marker::CUBE;
      break;  
    }

    r.sleep();
  }
}