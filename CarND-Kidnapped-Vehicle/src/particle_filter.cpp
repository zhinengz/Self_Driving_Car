/**
 * particle_filter.cpp
 *
 * Created on: Dec 12, 2016
 * Author: Tiffany Huang
 */

#include "particle_filter.h"

#include <math.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "helper_functions.h"
using namespace std;
using std::string;
using std::vector;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
  /**
   * TODO: Set the number of particles. Initialize all particles to 
   *   first position (based on estimates of x, y, theta and their uncertainties
   *   from GPS) and all weights to 1. 
   * TODO: Add random Gaussian noise to each particle.
   * NOTE: Consult particle_filter.h for more information about this method 
   *   (and others in this file).
   */
  num_particles = 800;  // TODO: Set the number of particles
  particles.reserve(num_particles);
  weights.reserve(num_particles);  
  
  normal_distribution<double> dist_x(x, std[0]);
  normal_distribution<double> dist_y(y, std[1]);
  normal_distribution<double> dist_theta(theta, std[2]);
  default_random_engine gen;
  
  for(int i=0;i<num_particles;i++)
  {
    Particle p={i,               //id
                dist_x(gen),     //x 
                dist_y(gen),     //y
                dist_theta(gen), //theta
                1.0};            //weight
    particles.push_back(p);
  }  
  is_initialized = true;
}

void ParticleFilter::prediction(double delta_t, double std_pos[], 
                                double velocity, double yaw_rate) {
  /**
   * TODO: Add measurements to each particle and add random Gaussian noise.
   * NOTE: When adding noise you may find std::normal_distribution 
   *   and std::default_random_engine useful.
   *  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
   *  http://www.cplusplus.com/reference/random/default_random_engine/
   */
  normal_distribution<double> dist_x(0, std_pos[0]);
  normal_distribution<double> dist_y(0, std_pos[1]);
  normal_distribution<double> dist_theta(0, std_pos[2]);
  default_random_engine gen;
  
  //double v_ovr_yaw = velocity / yaw_rate;
  //double theta_x_dt = yaw_rate * delta_t;
  for(auto& p : particles)
  {
	if(fabs(yaw_rate)<0.0001) //prevent x error too big issue
    {
    	p.x += velocity * delta_t * cos(p.theta);
    	p.y += velocity * delta_t * sin(p.theta);      
    }
    else{
      double v_ovr_yaw = velocity / yaw_rate;
      double theta_x_dt = yaw_rate * delta_t;      
      p.x +=  v_ovr_yaw *(sin(p.theta + theta_x_dt) - sin(p.theta)) + dist_x(gen) ;
      p.y +=  v_ovr_yaw * (cos(p.theta) - cos(p.theta + theta_x_dt)) + dist_y(gen);
      p.theta += theta_x_dt+ dist_theta(gen);
    }
  }
}

void ParticleFilter::dataAssociation(vector<LandmarkObs> predicted, 
                                     vector<LandmarkObs>& observations) {
  /**
   * TODO: Find the predicted measurement that is closest to each 
   *   observed measurement and assign the observed measurement to this 
   *   particular landmark.
   * NOTE: this method will NOT be called by the grading code. But you will 
   *   probably find it useful to implement this method and use it as a helper 
   *   during the updateWeights phase.
   */

  for(auto& p_obs: observations)
  {
    double minDist = numeric_limits<double>::max();
  	for(auto& p_pre: predicted)
    {
    	double dist_obs_to_pre = dist(p_obs.x, p_obs.y, p_pre.x, p_pre.y);
        if(dist_obs_to_pre < minDist)
        {
        	p_obs.id = p_pre.id;
          	minDist = dist_obs_to_pre;
        }
    }
  }
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
                                   const vector<LandmarkObs> &observations, 
                                   const Map &map_landmarks) {
  /**
   * TODO: Update the weights of each particle using a mult-variate Gaussian 
   *   distribution. You can read more about this distribution here: 
   *   https://en.wikipedia.org/wiki/Multivariate_normal_distribution
   * NOTE: The observations are given in the VEHICLE'S coordinate system. 
   *   Your particles are located according to the MAP'S coordinate system. 
   *   You will need to transform between the two systems. Keep in mind that
   *   this transformation requires both rotation AND translation (but no scaling).
   *   The following is a good resource for the theory:
   *   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
   *   and the following is a good resource for the actual equation to implement
   *   (look at equation 3.33) http://planning.cs.uiuc.edu/node99.html
   */
  double sig_x = std_landmark[0];
  double sig_y = std_landmark[1];
  
  double gauss_norm;
  gauss_norm = 1.0/(2.0*M_PI * sig_x * sig_y);  
  double norm = 0.000000001; //preventing 0-division

  //Each observation could be from any of the particles
  //And each particle is a possible locaton where the kidnapped car is
  //Therefore loop through each particle to see which one is most likely
  //the car's location by looking at how close the predicted landmarks are to
  //observed marks
  for (auto& p: particles)
  {
  	double p_x = p.x;
    double p_y = p.y;
    double p_theta = p.theta;

    //if the car is assumed to be at the location particles[i], what map 
    //landmarks (predicted_landmarks_in_range) it can see given its sensing range 
    vector<LandmarkObs> predicted_landmarks_in_range;
    predicted_landmarks_in_range.reserve(map_landmarks.landmark_list.size());
    for(const auto& landmark: map_landmarks.landmark_list)
    {
      double dis = dist( p_x, p_y, landmark.x_f, landmark.y_f);
      if(dis < sensor_range)
      {
      	LandmarkObs l;
        l.id = landmark.id_i;
        l.x  = landmark.x_f;
        l.y  = landmark.y_f;
        predicted_landmarks_in_range.push_back(l);
      }
    }
    // The match predicted landmarks to the observed ones to see how colse they are
    // First transfer observations coordinates into maps ones
    vector<LandmarkObs> observed_landmarks_map;
    observed_landmarks_map.reserve(observations.size());
    for(auto& obs: observations)
    {
    	LandmarkObs l;
     	l.x = p_x + cos(p_theta) * obs.x - sin(p_theta) * obs.y;
     	l.y = p_y + sin(p_theta) * obs.x + cos(p_theta) * obs.y;    
        observed_landmarks_map.push_back(l);
    }
    
    dataAssociation(predicted_landmarks_in_range,
                     observed_landmarks_map);
    double weight=1.0;
    for(auto & obs: observed_landmarks_map){
      double mu_x, mu_y;
      //obs.id -1: lamdmark id start at 1, while vector index start at 0
      mu_x = map_landmarks.landmark_list[obs.id-1].x_f;
      mu_y = map_landmarks.landmark_list[obs.id-1].y_f;  
      
      double exponent;
      exponent = (pow(obs.x - mu_x, 2) / (2 * pow(sig_x,2))) +
                 (pow(obs.y - mu_y, 2) / (2 * pow(sig_y,2)));
      weight *= gauss_norm * exp(-exponent);
      
    } 
    norm += weight;
    p.weight = weight;
  }

  for(auto& p: particles)
  {
    p.weight = p.weight / norm;
    weights.push_back(p.weight); //after resampling weights is cleared, and
                          //therefore its size becomes 0 before any pushback
  }
}

void ParticleFilter::resample() {
  /**
   * TODO: Resample particles with replacement with probability proportional 
   *   to their weight. 
   * NOTE: You may find std::discrete_distribution helpful here.
   *   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
   */
  //random_device rd;
  //mt19937 gen(rd());
  default_random_engine gen;
  discrete_distribution<int> sample_prob(weights.begin(),weights.end());
  vector<Particle> particles_resampled;
  particles_resampled.reserve(num_particles);
  for(int i=0;i<num_particles;i++)
  {
    Particle p = particles[sample_prob(gen)];
  	particles_resampled.push_back(p);
  }
  particles = particles_resampled;
  weights.clear();
}

void ParticleFilter::SetAssociations(Particle& particle, 
                                     const vector<int>& associations, 
                                     const vector<double>& sense_x, 
                                     const vector<double>& sense_y) {
  // particle: the particle to which assign each listed association, 
  //   and association's (x,y) world coordinates mapping
  // associations: The landmark id that goes along with each listed association
  // sense_x: the associations x mapping already converted to world coordinates
  // sense_y: the associations y mapping already converted to world coordinates
  particle.associations= associations;
  particle.sense_x = sense_x;
  particle.sense_y = sense_y;
}

string ParticleFilter::getAssociations(Particle best) {
  vector<int> v = best.associations;
  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<int>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}

string ParticleFilter::getSenseCoord(Particle best, string coord) {
  vector<double> v;

  if (coord == "X") {
    v = best.sense_x;
  } else {
    v = best.sense_y;
  }

  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<float>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}