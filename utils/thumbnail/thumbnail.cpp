#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <input_filepath> <output_filename>"
              << std::endl;
    return 1;
  }

  std::string input_path = argv[1];
  std::string output_dir = "../../thumbnails/";
  std::string output_path = output_dir + argv[2];

  try {
    std::filesystem::create_directories(output_dir);
  } catch (const std::exception& e) {
    std::cerr << "Error: Failed to create directory: " << e.what() << std::endl;
    return 1;
  }

  cv::Mat img = cv::imread(input_path);
  if (img.empty()) {
    std::cerr << "Error: Failed to read image: " << input_path << std::endl;
    return 1;
  }

  // Define cropping parameters according to the output of check_pixels.cpp
  // (w_edge, h_edge) is the top-left corner of the square crop
  // size is the width and height of the square crop
  const int w_edge = 1141;
  const int h_edge = 384;
  const int size = 599;
  cv::Mat thumbnail = img(cv::Rect(w_edge, h_edge, size, size));

  std::vector<int> params = {cv::IMWRITE_PNG_COMPRESSION, 0};
  if (!cv::imwrite(output_path, thumbnail, params)) {
    std::cerr << "Error: Failed to save thumbnail to: " << output_path
              << std::endl;
    return 1;
  }

  std::cout << "Thumbnail saved to: " << output_path << std::endl;
  return 0;
}
