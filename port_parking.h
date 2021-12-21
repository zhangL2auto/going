/*
 * @Project: port_parking
 * @version: 1.0
 * @Author: lZhang
 * @Date: 2021-12-20 13:11:32
 */
#ifndef PORT_PARKING_H
#define PORT_PARKING_H

#include<opencv2/opencv.hpp>
#include<iostream>
#include<math.h>

float detect_line(cv::Mat frame);
bool  verify_line(float th, float lower = 1.5, float upper = 1.6);

#endif 

