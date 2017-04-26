#include <ros/ros.h>
#include <iostream>
#include <sensor_msgs/LaserScan.h>
#include "geometry_msgs/Twist.h"


// 2 0.05 0.4
// 3 0.03 0.5

#define THRESHOLD 1.5
#define Kp 0.01
#define V 0.3
#define delta_t 1/30

struct gapStruct {
   float  gapStart;
   float  gapSize;
};

float max_gapFinish,max_gapStart;
float openGap=0;
int size=0;
int dataNum=0;
float max_gapSize=120;
float average_gap;
float W;
float R,delta_x,alfa,sub_alfa=0;

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
   
   size=(scan->angle_max - scan->angle_min) / scan->angle_increment;
   float ranges[size-1]; 
   //collect data
   for(dataNum=1;dataNum<size;dataNum++)
   	 ranges[dataNum]=scan->ranges[dataNum];
   
    
    //simulation obstacle
   ranges[0]=0.9;
   ranges[size-1]=0.9;

   for(dataNum=0;dataNum<size;dataNum++){
   	 
   	 if( (ranges[dataNum]<THRESHOLD)&&ranges[dataNum]>0.5){
   	 	 ranges[dataNum]=1;
   	  	}

   	 else
   	 	ranges[dataNum]=0;
   	}

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
			max_gapFinish=max_gapStart+gap.gapSize*(scan->angle_increment)*180/3.14;
			average_gap = (max_gapStart+max_gapFinish)/2;
     	}
        //printf("\n\n max_gapStart = %f || max_gapFinish = %f || max_gapSize = %f || average_gap = %f ||\n\n",max_gapStart,max_gapFinish,max_gapSize,average_gap);
     	gap.gapStart=0;
     	gap.gapSize=0;
     	openGap=0;
     }
   }
   	//for(dataNum=0;dataNum<size;dataNum++){
	// 	printf("%.1f  ",ranges[dataNum]);
	//}
   max_gapSize = 120;
   W=Kp*-1*(90 - average_gap) ;

   printf("hedef = %f",average_gap);
   //R=(V*delta_t)/W;rostop
   //alfa = (V*delta_t)/R*(180/3.14);
   //sub_alfa=alfa+sub_alfa;
   
   //delta_x=R*(sin()-sin())
   printf("W = %f \n",W);
 }




int main(int argc, char **argv)
{
   ros::init(argc, argv, "gryyy"); 

   ros::NodeHandle nh;

   ros::Publisher chatter_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
   ros::Subscriber scanSub=nh.subscribe<sensor_msgs::LaserScan>("/scan",1000,processLaserScan);

   ros::Rate loop_rate(10);

   while(ros::ok()){
   	 geometry_msgs::Twist velMsg;
     velMsg.linear.x  = V;
     velMsg.angular.z = (W);
     chatter_pub.publish(velMsg);
     ros::spinOnce();
     loop_rate.sleep();

   }

return 0;
}



/*


void processLaserScan(const sensor_msgs::LaserScan::ConstPtr& scan){

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
   
   struct gapStruct gap;
   
   size=(scan->angle_max - scan->angle_min) / scan->angle_increment;
   float ranges[size-1]; 
   //collect data
   for(dataNum=1;dataNum<size;dataNum++){
   ranges[dataNum]=scan->ranges[dataNum];
   }
    
    //simulation obstacle
   ranges[0]=0.9;
   ranges[size-1]=0.9;

   for(dataNum=0;dataNum<size;dataNum++){
   	 
   	 if( (ranges[dataNum]<THRESHOLD)&&ranges[dataNum]>0.5){
   	 	ranges[dataNum]=1;
   	 }
   	 else{
   	 	ranges[dataNum]=0;}    }

   for(dataNum=0;dataNum<size;dataNum++){
     if((ranges[dataNum]==0) && openGap==0){
     	openGap=1;
     	gap.gapStart=dataNum;
     }
     if(openGap){
     	gap.gapSize++;
     }
     if((ranges[dataNum]==1) && openGap ==1){
     	if (gap.gapSize > max_gapSize)
     	{	
			max_gapStart=(gap.gapStart*(scan->angle_increment))*180/3.14;
			max_gapSize=gap.gapSize;
			max_gapFinish=max_gapStart+gap.gapSize*(scan->angle_increment)*180/3.14;
			average_gap = (max_gapStart+max_gapFinish)/2;
     	}
        printf("\n\n max_gapStart = %f || max_gapFinish = %f || max_gapSize = %f || average_gap = %f ||\n\n",max_gapStart,max_gapFinish,max_gapSize,average_gap);
     	gap.gapStart=0;
     	gap.gapSize=0;
     	openGap=0;
     }

   }

   	for(dataNum=0;dataNum<size;dataNum++){
	 	printf("%.1f  ",ranges[dataNum]);
	 }
   max_gapSize = 120;
   //printf("GAP BASLAMA BİTME SİZE %f %f %d\n",max_gapStart,max_gapFinish,max_gap);
   W=Kp*(90 - average_gap) ;
 }

int main(int argc, char **argv)
{
   ros::init(argc, argv, "gryyy"); 

   ros::NodeHandle nh;

   ros::Publisher chatter_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
   ros::Subscriber scanSub=nh.subscribe<sensor_msgs::LaserScan>("/scan",1000,processLaserScan);
   //ros::Rate loop_rate(1);
   ros::Rate loop_rate(10);
   
   while(ros::ok()){
   	 geometry_msgs::Twist velMsg;
     //chatter_pub.publish(velMsg);
     velMsg.linear.x  = V;
     velMsg.angular.z = (W);
     chatter_pub.publish(velMsg);
     ros::spinOnce();
     loop_rate.sleep();
   }

return 0;
}

*/
