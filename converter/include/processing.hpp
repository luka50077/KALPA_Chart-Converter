#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

#include "parser.hpp"

std::vector<cv::Mat> load_warped_frames(const cv::Mat& M, Config& config);
std::vector<cv::Mat> stack_and_split(const std::vector<cv::Mat>& frames);
cv::Mat generate_image(const std::vector<cv::Mat>& part_images, Config& config);
void save_final_image(const cv::Mat& final_image, Config& config);