#include <uWS/uWS.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include "Eigen-3.3/Eigen/Core"
#include "Eigen-3.3/Eigen/QR"
#include "helpers.h"
#include "json.hpp"
#include "spline.h"
#include "utils.h"

// for convenience
using nlohmann::json;
using std::string;
using std::vector;

int main() {
	uWS::Hub h;

	// Load up map values for waypoint's x,y,s and d normalized normal vectors
	vector<double> map_waypoints_x;
	vector<double> map_waypoints_y;
	vector<double> map_waypoints_s;
	vector<double> map_waypoints_dx;
	vector<double> map_waypoints_dy;

	// Waypoint map to read from
	string map_file_ = "../data/highway_map.csv";
	// The max s value before wrapping around the track back to 0
	double max_s = 6945.554;

	std::ifstream in_map_(map_file_.c_str(), std::ifstream::in);

	string line;
	while (getline(in_map_, line)) {
		std::istringstream iss(line);
		double x;
		double y;
		float s;
		float d_x;
		float d_y;
		iss >> x;
		iss >> y;
		iss >> s;
		iss >> d_x;
		iss >> d_y;
		map_waypoints_x.push_back(x);
		map_waypoints_y.push_back(y);
		map_waypoints_s.push_back(s);
		map_waypoints_dx.push_back(d_x);
		map_waypoints_dy.push_back(d_y);
	}
	int lane = 1;
	double ref_vel = 0.0;
	h.onMessage([&ref_vel, &map_waypoints_x, &map_waypoints_y, &map_waypoints_s,
		&map_waypoints_dx, &map_waypoints_dy, &lane]
		(uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length,
			uWS::OpCode opCode) {
		// "42" at the start of the message means there's a websocket message event.
		// The 4 signifies a websocket message
		// The 2 signifies a websocket event
		if (length && length > 2 && data[0] == '4' && data[1] == '2') {

			auto s = hasData(data);

			if (s != "") {
				auto j = json::parse(s);

				string event = j[0].get<string>();

				if (event == "telemetry") {
					// j[1] is the data JSON object

					// Main car's localization Data
					double car_x = j[1]["x"];
					double car_y = j[1]["y"];
					double car_s = j[1]["s"];
					double s_of_car = j[1]["s"];
					double car_d = j[1]["d"];
					double car_yaw = j[1]["yaw"];
					double car_speed = j[1]["speed"];

					// Previous path data given to the Planner
					auto previous_path_x = j[1]["previous_path_x"];
					auto previous_path_y = j[1]["previous_path_y"];
					// Previous path's end s and d values
					double end_path_s = j[1]["end_path_s"];
					double end_path_d = j[1]["end_path_d"];

					// Sensor Fusion Data [id, x, y, vx, vy, s, d]
					//a list of all other cars on the same side   of the road.
					auto sensor_fusion = j[1]["sensor_fusion"];

					json msgJson;

					int prev_size = previous_path_x.size();

					if (prev_size>0) { car_s = end_path_s; }

          std::array<bool, total_lane> any_car_ahead;
          std::array<bool, total_lane> any_car_behind;
          std::array<bool, total_lane> any_lanes_available;
					std::array<double, total_lane> distance_ahead;
					any_car_ahead.fill(false);
					any_car_behind.fill(false);
					any_lanes_available.fill(true);
					distance_ahead.fill(6950);
          //too_close indicates a car jump into the previous size point

					//for (int i = 0; i<sensor_fusion.size(); i++)
					//{

          available_lanes(lane,s_of_car,car_s, prev_size,sensor_fusion,
							                   any_car_ahead, any_car_behind,
																 any_lanes_available,
															   distance_ahead);

					//}

					bool any_lane_available=false;
					for(auto b:any_lanes_available){any_lane_available|=b;}
					std::cout<<"any lane available: "<<any_lane_available<<std::endl;

					if (any_car_ahead[lane])
					{
						ref_vel -=std::max(0.224*std::exp(3.0/distance_ahead[lane]),0.02);
						if (lane==0 && any_lanes_available[1]){
						  lane++;
						}
						if((lane+1)==total_lane && any_lanes_available[lane-1]){
						  lane--;
						}
						if ((lane>0 &&any_lanes_available[lane-1]) ||
						    ((lane+1)<total_lane&&any_lanes_available[lane+1])){
							if(any_lanes_available[lane-1] && any_lanes_available[lane+1]){
							  lane += distance_ahead[lane-1]>distance_ahead[lane+1]?-1:1;
						  }else{
								lane += any_lanes_available[lane-1] ?-1:1;
							}
						}
					}
					else
					{
						if (ref_vel<speed_limit)
						  ref_vel +=  0.224;
						if(lane==0 && any_lanes_available[1]){
							lane += distance_ahead[0]>distance_ahead[1]?0:1;
						}
						if((lane+1)==total_lane && any_lanes_available[lane-1]){
						  lane += distance_ahead[lane-1]>distance_ahead[lane]?-1:0;
						}
					}
					for(auto item:any_lanes_available){std::cout<<item<<" ";}
					std::cout<<std::endl;
					/**
					* TODO: define a path made up of (x,y) points that the car will visit
					*   sequentially every .02 seconds
					*/
					vector<double> ptsx;
					vector<double> ptsy;
					double ref_x = car_x;
					double ref_y = car_y;
					double ref_yaw = deg2rad(car_yaw);
					if (prev_size<2)
					{
						double prev_car_x = car_x - cos(car_yaw);
						double prev_car_y = car_y - sin(car_yaw);
						ptsx.push_back(prev_car_x);
						ptsx.push_back(car_x);
						ptsy.push_back(prev_car_y);
						ptsy.push_back(car_y);
					}
					else
					{
						ref_x = previous_path_x[prev_size - 1];
						ref_y = previous_path_y[prev_size - 1];
						double ref_x_prev = previous_path_x[prev_size - 2];
						double ref_y_prev = previous_path_y[prev_size - 2];
						ref_yaw = atan2(ref_y - ref_y_prev, ref_x - ref_x_prev);
						ptsx.push_back(ref_x_prev);
						ptsx.push_back(ref_x);
						ptsy.push_back(ref_y_prev);
						ptsy.push_back(ref_y);
					}
					vector<double> next_wp0 = getXY(car_s + 30, (2 + 4 * lane), map_waypoints_s, map_waypoints_x, map_waypoints_y);
					vector<double> next_wp1 = getXY(car_s + 60, (2 + 4 * lane), map_waypoints_s, map_waypoints_x, map_waypoints_y);
					vector<double> next_wp2 = getXY(car_s + 90, (2 + 4 * lane), map_waypoints_s, map_waypoints_x, map_waypoints_y);
					ptsx.push_back(next_wp0[0]);
					ptsx.push_back(next_wp1[0]);
					ptsx.push_back(next_wp2[0]);

					ptsy.push_back(next_wp0[1]);
					ptsy.push_back(next_wp1[1]);
					ptsy.push_back(next_wp2[1]);


					for (int i = 0; i<ptsx.size(); i++)
					{
						double shift_x = ptsx[i] - ref_x;
						double shift_y = ptsy[i] - ref_y;

						ptsx[i] = (shift_x*cos(0 - ref_yaw) - shift_y * sin(0 - ref_yaw));
						ptsy[i] = (shift_x*sin(0 - ref_yaw) + shift_y * cos(0 - ref_yaw));
					}

					tk::spline spln;
					spln.set_points(ptsx, ptsy);

					vector<double> next_x_vals;
					vector<double> next_y_vals;


					for (int i = 0; i < previous_path_x.size(); i++) {

						next_x_vals.push_back(previous_path_x[i]);
						next_y_vals.push_back(previous_path_y[i]);
					}
					double target_x = 30.0;
					double target_y = spln(target_x);
					double target_dist = sqrt((target_x)*(target_x)+(target_y)*(target_y));
					double x_add_on = 0;

					for (int i = 1; i<40 - previous_path_x.size(); i++)
					{
						double N = (target_dist / (0.02*ref_vel / 2.24));
						double x_point = x_add_on + (target_x) / N;
						double y_point = spln(x_point);
						x_add_on = x_point;

						double x_ref = x_point;
						double y_ref = y_point;

						x_point = (x_ref * cos(ref_yaw) - y_ref * sin(ref_yaw));
						y_point = (x_ref * sin(ref_yaw) + y_ref * cos(ref_yaw));
						x_point += ref_x;
						y_point += ref_y;

						next_x_vals.push_back(x_point);
						next_y_vals.push_back(y_point);
					}

					/*END***********************************************/
					msgJson["next_x"] = next_x_vals;
					msgJson["next_y"] = next_y_vals;

					auto msg = "42[\"control\"," + msgJson.dump() + "]";

					ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
				}  // end "telemetry" if
			}
			else {
				// Manual driving
				std::string msg = "42[\"manual\",{}]";
				ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
			}
		}  // end websocket if
	}); // end h.onMessage

	h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
		std::cout << "Connected!!!" << std::endl;
	});

	h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code,
		char *message, size_t length) {
		ws.close();
		std::cout << "Disconnected" << std::endl;
	});

	int port = 4567;
	if (h.listen(port)) {
		std::cout << "Listening to port " << port << std::endl;
	}
	else {
		std::cerr << "Failed to listen to port" << std::endl;
		return -1;
	}

	h.run();
}
