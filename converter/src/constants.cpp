#include "constants.hpp"

const int dst_width = 800;
const int dst_height = 450;
const int max_height = 6150;
const int gap_width = 150;

const std::vector<cv::Point2f> src_pts = {
    {351, 790}, {1567, 790}, {1905, 1415}, {15, 1415}};
const std::vector<cv::Point2f> dst_pts = {
    {0, 0},
    {static_cast<float>(dst_width), 0},
    {static_cast<float>(dst_width), static_cast<float>(dst_height)},
    {0, static_cast<float>(dst_height)}};

// diffuculty / color map
const std::map<std::string, std::array<uint, 3>> difficulty_map = {
    {"NORMAL", {115, 155, 70}}, {"HARD", {173, 133, 29}},
    {"HARD+", {130, 74, 161}},  {"ABYSS", {98, 43, 96}},
    {"S.HARD", {160, 70, 50}},  {"S.HARD+", {8, 0, 135}},
    {"CHAOS", {126, 39, 103}},  {"COSMOS", {121, 104, 6}},
    {"EXTRA", {150, 5, 70}},
};
