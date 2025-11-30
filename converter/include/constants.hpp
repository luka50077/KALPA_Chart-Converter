#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

extern const int dst_width;
extern const int dst_height;
extern const int max_height;
extern const int gap_width;

extern const std::vector<cv::Point2f> src_pts;
extern const std::vector<cv::Point2f> dst_pts;

struct Padding {
  static constexpr int TOP = 200;
  static constexpr int BOTTOM = 200;
  static constexpr int LEFT = 300;
  static constexpr int RIGHT = 300;
};

extern const std::map<std::string, std::array<uint, 3>> difficulty_map;
