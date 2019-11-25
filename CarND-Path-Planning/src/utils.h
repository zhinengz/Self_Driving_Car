#ifndef UTILS_H
#define  UTILS_H

#include <math.h>
#include <string>
#include <vector>
#include <map>
#include <array>
#include "spline.h"
#include "helpers.h"
using std::string;
using std::vector;
using std::map;

static const int max_acceleration =10;
static const int comfort_buffer = 30;
static const double speed_limit = 49.5;
static const int total_lane = 3;

int get_lane(double d)
{
  int lane = -1;
  if ( d > 0 && d < 4 ) {
    lane = 0;
  } else if ( d > 4 && d < 8 ) {
    lane = 1;
  } else if ( d > 8 && d < 12 ) {
    lane = 2;
  }
  return lane;
}

void available_lanes(int lane,double& s_of_car,
              double& car_s, int prev_size,
	            const vector<vector<double>>& sensor_fusion,
							std::array<bool,total_lane>& any_car_ahead,
						  std::array<bool,total_lane>& any_car_behind,
						  std::array<bool,total_lane>& any_lanes_available,
						  std::array<double,total_lane>& distance_ahead){
	int other_car_lane ;
	double vx ;
	double vy ;
	double check_speed ;
	double check_car_s ;
  //std::array<int,total_lane> id_car_ahead;
  //id_car_ahead.fill(-100);

  for (int i = 0; i<sensor_fusion.size(); i++)
  {
    other_car_lane = get_lane(sensor_fusion[i][6]);
  	vx = sensor_fusion[i][3];
  	vy = sensor_fusion[i][4];
  	check_speed = sqrt(vx*vx + vy*vy);
  	check_car_s = sensor_fusion[i][5];

	  check_car_s += ((double)prev_size*0.02*check_speed);
    //record the distance to the car in the front
	  if(check_car_s>car_s && (check_car_s-car_s)<distance_ahead[other_car_lane]){
		  distance_ahead[other_car_lane]=(check_car_s-car_s);
    //  id_car_ahead[other_car_lane]=sensor_fusion[i][0];

    }
    //handle the case the car is less than previous s,
    // like suddenly a car cut in from adjacent lane
    if(other_car_lane==lane){
       distance_ahead[other_car_lane]=std::min(distance_ahead[other_car_lane],
          (sensor_fusion[i][5]-s_of_car));
    }
      //record  if there is any car in front from any lanes
      //sometime the car from other lane could cut in into previous trajactory
	  any_car_ahead[other_car_lane] |=
										 (check_car_s > car_s && check_car_s - car_s < 30)|
       (sensor_fusion[i][5]>s_of_car&& (sensor_fusion[i][5]-s_of_car)<30);
    //record  if there is any car behind from any lanes
	  any_car_behind[other_car_lane] |=
  	  (sensor_fusion[i][5] < s_of_car && sensor_fusion[i][5]  + 30 > s_of_car)|
									 	(check_car_s < car_s && check_car_s + 30 > car_s);
    //record the lanes without car both in front and behind
	  any_lanes_available[other_car_lane] = !any_car_ahead[other_car_lane]
	         && !any_car_behind[other_car_lane];
  }


}


#endif // UTILS_H
