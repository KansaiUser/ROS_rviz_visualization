/*
*  Trying to do rviz to show images with options determined by a menu
*/
#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <sensor_msgs/Image.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>


#include <interactive_markers/interactive_marker_server.h>
#include <interactive_markers/menu_handler.h>

#include <tf/transform_broadcaster.h>
#include <tf/tf.h>

#include <math.h>

#include <vector>

using namespace visualization_msgs;
using namespace interactive_markers;

boost::shared_ptr<InteractiveMarkerServer> server;
//float marker_pos = 0;
int found_elements = 2;  //Here we simulate we have "found" two elements

std::vector<bool> doShow;

MenuHandler menu_handler;
MenuHandler::EntryHandle h_mode_last;

//visualization_msgs::Marker marker;
sensor_msgs::Image image;


void modeCb( const visualization_msgs::InteractiveMarkerFeedbackConstPtr &feedback )
{
  //menu_handler.setCheckState( h_mode_last, MenuHandler::UNCHECKED );
  //h_mode_last = feedback->menu_entry_id;  // here we got the number of the element to show (-1)
  //menu_handler.setCheckState( h_mode_last, MenuHandler::CHECKED );

  MenuHandler::EntryHandle handle = feedback->menu_entry_id; // here we got the number of the element to show (-1)
  MenuHandler::CheckState state;
  menu_handler.getCheckState( handle, state );

  ROS_INFO("Switching to menu entry #%d", handle);

   if ( state == MenuHandler::CHECKED )
   {
     ROS_INFO(" Hiding");
     doShow[handle] = false;
     menu_handler.setCheckState( handle, MenuHandler::UNCHECKED );
   }
   else
   {
     ROS_INFO(" Showing");
     doShow[handle] = true;
     menu_handler.setCheckState( handle, MenuHandler::CHECKED );
   }
   

  menu_handler.reApply( *server );
  server->applyChanges();

}

void makeImage()
{
    cv_bridge::CvImage cv_image;
    cv_image.image = cv::imread("/root/test_dir/two_pedestrian.jpeg",CV_LOAD_IMAGE_COLOR);
    cv_image.encoding = "bgr8";
    //sensor_msgs::Image ros_image;
    cv_image.toImageMsg(image);
}

void makeMenuMarker( std::string name )
{
  InteractiveMarker menu_marker;
  menu_marker.header.frame_id = "base_link";
  menu_marker.pose.position.y = 0.0; //-3.0 * marker_pos++;;
  menu_marker.scale = 1;
  menu_marker.name = name;

  InteractiveMarkerControl control;

  control.interaction_mode = InteractiveMarkerControl::BUTTON;
  control.always_visible = true;

  Marker marker;

  marker.type = Marker::CUBE;
  marker.scale.x = 0.45;
  marker.scale.y = 0.45;
  marker.scale.z = 0.45;
  marker.color.r = 0.5;
  marker.color.g = 0.5;
  marker.color.b = 0.5;
  marker.color.a = 1.0;

  control.markers.push_back( marker );
  menu_marker.controls.push_back(control);

  server->insert( menu_marker );

}

void initMenu()
{
    MenuHandler::EntryHandle sub_menu_handle = menu_handler.insert( "Show" );
    for ( int i=0; i<found_elements; i++ )
    {
      std::ostringstream s;
      s << "Element " << i;
      h_mode_last = menu_handler.insert( sub_menu_handle, s.str(), &modeCb );
      menu_handler.setCheckState( h_mode_last, MenuHandler::CHECKED );  //The default option is checked (show everything)
    }

}

int main(int argc, char** argv)
{

  ros::init(argc, argv, "image_menu");

  server.reset( new InteractiveMarkerServer("image_menu","",false) );  //take ownership of the marker

  ros::NodeHandle n;

  ros::Publisher image_pub = n.advertise<sensor_msgs::Image>("visualization_image",1);

  for(int i=0;i<found_elements;i++){
      doShow.push_back(true);
  }


  initMenu();  //First we initiate the shape of the menu on the menu_handler

  makeMenuMarker("Image" );

  makeImage();


  // we apply the menu hander on the server 
  menu_handler.apply( *server, "Image" );

  server->applyChanges();

    while (image_pub.getNumSubscribers() < 1)
    {
      if (!ros::ok())
      {
        return 0;
      }
      ROS_WARN_ONCE("Please create a subscriber to the image");
      sleep(1);
    }
   
    image_pub.publish(image);

  ros::spin();

  server.reset();  //deletes managed object




}