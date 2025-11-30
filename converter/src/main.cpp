#include <filesystem>
#include <iostream>

#include "constants.hpp"
#include "parser.hpp"
#include "processing.hpp"

int main() {
  Config config;
  if (!load_config("../config.json", config)) {
    std::cerr << "Failed to load configuration." << std::endl;
    return -1;
  }

  std::filesystem::create_directories(config.basics.output_dir);

  cv::Mat M = cv::getPerspectiveTransform(src_pts, dst_pts);

  auto frames = load_warped_frames(M, config);
  auto part_images = stack_and_split(frames);
  auto final_image_padded = generate_image(part_images, config);

  save_final_image(final_image_padded, config);
  return 0;
}
