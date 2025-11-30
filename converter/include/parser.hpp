#pragma once
#include <map>
#include <string>

struct Basics {
  std::string video_path;
  std::string output_dir;
  std::string output_name;
  int start_frame;
  int end_frame;
  int jpeg_quality;
  int default_step;
  bool adjust_step_enabled;
  std::string thumbnail_path;
  std::string difficulty;
};

struct Config {
  Basics basics;
  std::map<int, int> step;
};

bool load_config(const std::string &filename, Config &config);
