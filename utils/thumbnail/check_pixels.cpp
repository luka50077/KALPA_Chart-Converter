#include <iostream>
#include <opencv2/opencv.hpp>

void onMouse(int event, int x, int y, int flags, void* userdata) {
  if (event == cv::EVENT_MOUSEMOVE) {
    std::cout << "Mouse Position: (" << x << ", " << y << ")" << std::endl;
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <image_path>\n";
    return 1;
  }
  std::string imagepath = argv[1];
  cv::Mat img = cv::imread(imagepath);
  if (img.empty()) {
    std::cerr << "Failed to load image.\n";
    return 1;
  }

  cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
  cv::setMouseCallback("image", onMouse, nullptr);

  while (true) {
    cv::imshow("image", img);
    int key = cv::waitKey(10);
    if (key == 27 || key == 'q') break;
  }

  return 0;
}
