/*************************************************************************************************************************************
This code was written by Giray UZUN who is graduated from Istanbul Technical University Control Engineering at 2016.                **
Follow the Gap Algorithm methos(FGM) is using for obstacle avoidance without goal point. This is not exactly like FGM but mostly similar    **
Everybody can use this program, it is open-source. We used that code on Intelligent Ground Vehicle Competition 2016.                **
You can reach the videos about this program result, link ;                                                                          **
        https://www.youtube.com/watch?v=9nzwZKTOCAk&list=PLztatg7ScoJJ8BQdfusIX4T78J3gQuX-c&index=3                                 **
                                                                                                                                    **
                                                                                                                                    **
If you want to ask something, please contact with me                                                                                **
facebook = https://www.facebook.com/giray.uzun.7                                                                                    **
e-mail   = girayuzun@gmail.com                                                                                                      **
                                                                                                                                    **
                                                                                                                                    **
*************************************************************************************************************************************/
 
#include <ros/ros.h>
#include <iostream>
#include <sensor_msgs/LaserScan.h>
#include "geometry_msgs/Twist.h"
 
#define THRESHOLD 1
#define Kp 0.01
#define Ki 0.0001                                                                                                                                   
#define V 0.1
#define delta_t 1/30
#define katsayi 0.50

struct gapStruct {
   float  gapStart;
   float  gapSize;
};
 
float max_gapFinish,max_gapStart;
float openGap=0;
int size=0;
int dataNum=0;
float max_gapSize=140;
float average_gap;
float W;
float R,delta_x,alfa,sub_alfa=0;
float integral;
float average_gap_;

 
void processLaserScan(const sensor_msgs::LaserScan::ConstPtr& scan){
   /*
   scan->angle_min;
   scan->angle_max;
   scan->angle_increment;
   size=(angle_max-angle_min)/angle_increment;
   float ranges[size],intensities[size]; 
   scan->time_increment;
   scan->scan_time;
   scan->range_min;
   scan->range_max;
   scan->ranges[];
   */
   ros::Rate loop_rate(10);
   struct gapStruct gap;
   //size of LIDAR data
   size=(scan->angle_max - scan->angle_min) / scan->angle_increment;
   float ranges[size-1]; 
   //collect data
   for(dataNum=1;dataNum<size;dataNum++)
     ranges[dataNum]=scan->ranges[dataNum];
    
     
   //simulation obstacle for going straight without obstacle
   ranges[0]=0.9;
   ranges[size-1]=0.9;
 
   // data is converted '1' and '0' map. '1' means that there is a obstacle, '0' means that there is a gap
   for(dataNum=0;dataNum<size;dataNum++){
     if( (ranges[dataNum]<THRESHOLD)&&ranges[dataNum]>0.5){
         ranges[dataNum]=1;
        }
     else
        ranges[dataNum]=0;
    }
 
    //detect gap and gap size. And also record the biggest gap
   for(dataNum=0;dataNum<size;dataNum++){
     if((ranges[dataNum]==0) && openGap==0){
         openGap=1;
         gap.gapStart=dataNum;
        }
     if(openGap){
         gap.gapSize++;
        }
 
     if((ranges[dataNum]==1) && openGap ==1){
        if (gap.gapSize > max_gapSize){  
            max_gapStart=(gap.gapStart*(scan->angle_increment))*180/3.14;
            max_gapSize=gap.gapSize;
            printf("max_gap %f   ",max_gapSize );
            max_gapFinish=max_gapStart+gap.gapSize*(scan->angle_increment)*180/3.14;
            average_gap = (max_gapStart+max_gapFinish)/2;
        }
        gap.gapStart=0;
        gap.gapSize=0;
        openGap=0;
     }
   }
   average_gap = katsayi * average_gap + average_gap_*(1-katsayi);
 
   max_gapSize = 70;           //minumum gap size to be max. gap size should be greater than 120 for being the biggest gap size
   W=Kp*(-90+average_gap);    //control signal = Kp * error.  Aim is that hold the gap in the middle. 
   integral = integral + (-90 + average_gap);
   if(integral>0.3)
   {
    integral = 0.3;
   }
   W = integral*Ki + W;
   printf("W = %f ~~ average_gap = %f\n",W,average_gap);       //check the angular velocity
   average_gap_ = average_gap;
 }
 
int main(int argc, char **argv)
{
   ros::init(argc, argv, "gryyy"); 
 
   ros::NodeHandle nh;
 
   ros::Publisher chatter_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
   ros::Subscriber scanSub=nh.subscribe<sensor_msgs::LaserScan>("/scan_img",1000,processLaserScan);
 
   ros::Rate loop_rate(10);
 
   while(ros::ok()){
     geometry_msgs::Twist velMsg;
     velMsg.linear.x  = V;          // linear velocity
     velMsg.angular.z = (W);        // angular velocity
     chatter_pub.publish(velMsg);   // publishing data
     ros::spinOnce();               
     loop_rate.sleep();                 
   }
 
return 0;
}
