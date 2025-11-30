#include "parser.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "constants.hpp"

bool load_config(const std::string &filename, Config &config) {
  std::ifstream ifs(filename);
  if (!ifs) {
    std::cerr << "Error: Cannot open file " << filename << std::endl;
    return false;
  }

  nlohmann::json j;
  try {
    ifs >> j;
  } catch (const std::exception &e) {
    std::cerr << "JSON parse error: " << e.what() << std::endl;
    return false;
  }

  try {
    // basic settings
    config.basics.video_path = j["basics"]["video_path"].get<std::string>();
    config.basics.output_dir = j["basics"]["output_dir"].get<std::string>();
    config.basics.output_name = j["basics"]["output_name"].get<std::string>();
    config.basics.start_frame = j["basics"]["start_frame"].get<int>();
    config.basics.end_frame = j["basics"]["end_frame"].get<int>();
    config.basics.jpeg_quality = j["basics"]["jpeg_quality"].get<int>();
    config.basics.default_step = j["basics"]["default_step"].get<int>();
    config.basics.adjust_step_enabled =
        j["basics"]["adjust_step_enabled"].get<bool>();
    config.basics.thumbnail_path =
        j["basics"]["thumbnail_path"].get<std::string>();
    config.basics.difficulty = j["basics"]["difficulty"].get<std::string>();

    config.basics.jpeg_quality = std::clamp(config.basics.jpeg_quality, 1, 100);
    config.basics.default_step = std::max(config.basics.default_step, 1);

    if (difficulty_map.find(config.basics.difficulty) == difficulty_map.end()) {
      std::cerr << "Warning: Unknown difficulty '" << config.basics.difficulty
                << "'" << std::endl;
    }

    // step settings
    config.step.clear();
    for (auto &item : j["step"].items()) {
      int key = std::stoi(item.key());
      int value = std::max(item.value().get<int>(), 1);
      config.step[key] = value;
    }

  } catch (const std::exception &e) {
    std::cerr << "Error reading config fields: " << e.what() << std::endl;
    return false;
  }

  return true;
}
