#pragma once
#include <opencv2/opencv.hpp>
#include <string>

void draw_info(cv::Mat& frame, int idx, int frame_count, double fps,
               bool playing);
void show_frame_seek(cv::VideoCapture& cap, const std::string& window_name,
                     int idx, int rotate_code, cv::RotateFlags rotate_flag);
cv::RotateFlags get_rotate_flag(int rotate_code);
