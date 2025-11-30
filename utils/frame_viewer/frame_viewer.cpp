#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

#include "video.hpp"

template <typename T>
T clamp(T val, T min_val, T max_val) {
  return std::max(min_val, std::min(max_val, val));
};

const std::string window_name =
    "Frame Viewer - q:quit  ←/b:prev  →/f:next  space:play・pause  s:save";

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <video file> (-s start_frame)"
              << std::endl;
    return 1;
  }

  std::string video_path = argv[1];
  if (!std::filesystem::exists(video_path)) {
    std::cerr << "[\033[31mERROR\033[0m] File not found: " << video_path
              << std::endl;
    return 1;
  }

  int start_frame = 0;
  if (argc == 4 && std::string(argv[2]) == "-s") {
    start_frame = std::max(std::stoi(argv[3]), start_frame);
  }

  std::filesystem::create_directory("../captures");

  cv::VideoCapture cap(video_path, cv::CAP_FFMPEG);
  if (!cap.isOpened()) {
    std::cerr << "[\033[31mERROR\033[0m] Failed to open video: " << video_path
              << std::endl;
    return 1;
  }

  int rotate_code = static_cast<int>(cap.get(cv::CAP_PROP_ORIENTATION_META));
  // std::cout << "[\033[32mINFO\033[0m] Video orientation meta: " <<
  // rotate_code << std::endl;
  cv::RotateFlags rotate_flag = get_rotate_flag(rotate_code);

  int frame_count = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
  double fps = cap.get(cv::CAP_PROP_FPS);
  if (static_cast<int>(fps) <= 0) fps = 30.0;
  double delay_play = 1000.0 / fps / 5;
  start_frame = clamp(start_frame, 0, frame_count - 1);
  std::cout << "[\033[32mINFO\033[0m] Start frame set to " << start_frame << "."
            << std::endl;

  cap.set(cv::CAP_PROP_POS_FRAMES, start_frame);

  int cur = start_frame;
  bool playing = false;

  cv::namedWindow(window_name, cv::WINDOW_NORMAL);

  show_frame_seek(cap, window_name, cur, rotate_code, rotate_flag);
  while (true) {
    int key;

    cv::Mat frame;
    if (playing) {
      if (!cap.read(frame)) break;

      if (rotate_code != 0) {
        cv::rotate(frame, frame, rotate_flag);
      }
      draw_info(frame, cur, frame_count, fps, playing);
      cv::imshow(window_name, frame);

      cur++;
      if (cur >= frame_count) {
        cur = frame_count - 1;
        playing = false;
      }

      key = cv::waitKey((int)delay_play);
    } else {
      key = cv::waitKey(0);
    }

    if (key == 27 || key == 'q') {
      break;
    } else if (key == ' ') {
      playing = !playing;
    } else if (key == 83 || key == 'n') {  // →
      cur = clamp(cur + 1, 0, frame_count - 1);
    } else if (key == 81 || key == 'p') {  // ←
      cur = clamp(cur - 1, 0, frame_count - 1);
    } else if (key == 'f') {  // +100
      cur = clamp(cur + 100, 0, frame_count - 1);
    } else if (key == 'b') {  // -100
      cur = clamp(cur - 100, 0, frame_count - 1);
    } else if (key == 's') {  // save current frame
      int target = cur;
      cap.set(cv::CAP_PROP_POS_FRAMES, target);
      cv::Mat frame;
      if (!cap.read(frame)) {
        std::cerr << "[\033[31mERROR\033[0m]: cap.read() failed at frame "
                  << target << std::endl;
        continue;
      }
      if (frame.empty()) {
        std::cerr << "[\033[31mERROR\033[0m]: Frame is empty at frame "
                  << target << std::endl;
        continue;
      }
      char fname[64];
      snprintf(fname, sizeof(fname), "../captures/frame_%06d.png", target + 1);
      if (!cv::imwrite(fname, frame)) {
        std::cerr << "[\033[31mERROR\033[0m] Failed to save " << fname
                  << std::endl;
        continue;
      }
      std::cout << "[\033[32mINFO\033[0m] Saved: " << fname << std::endl;
    }
    show_frame_seek(cap, window_name, cur, rotate_code, rotate_flag);
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}
