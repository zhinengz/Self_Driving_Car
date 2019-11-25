#include "kalman_filter.h"
# define PI          3.141592653589793238462643383279502884L
using Eigen::MatrixXd;
using Eigen::VectorXd;

/* 
 * Please note that the Eigen library does not initialize 
 *   VectorXd or MatrixXd objects with zeros upon creation.
 */

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
   * TODO: predict the state
   */
  //the same prediction function is applied to both Lidar and Radar
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
   * TODO: update the state by using Kalman Filter equations
   */
  //this function is called for Lidar data
  VectorXd z_pred = H_ * x_;
  VectorXd y      = z - z_pred;
  CombinedUpdate(y);
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
   * TODO: update the state by using Extended Kalman Filter equations
   */
  //this function is called for Radar data
  float px = x_(0);
  float py = x_(1);
  float vx = x_(2);
  float vy = x_(3);
  
  float ro     = sqrt(px * px + py * py);
  float theta  = atan2(py, px);
  float ro_dot = (px * vx + py * vy) / ro;
  VectorXd Hj_ = VectorXd(3);
  Hj_ << ro, theta, ro_dot;
  VectorXd y      = z - Hj_;
  while(y(1) > PI)
  {
    y(1) -= PI;
  }
  while(y(1)<-PI)
  {
  	y(1) += PI;
  }
  
  CombinedUpdate(y);
}

 void KalmanFilter::CombinedUpdate(const VectorXd &y) 
 {
   MatrixXd Ht     = H_.transpose();
   MatrixXd S      = H_ * P_ * Ht + R_;
   MatrixXd Si     = S.inverse();
   MatrixXd PHt    = P_ * Ht;
   MatrixXd K      = PHt * Si;
  
   //new estimation
   x_ = x_ + (K * y);
   long x_size = x_.size();
   MatrixXd I = MatrixXd::Identity(x_size, x_size);
   P_ = (I - K * H_) * P_;
 }