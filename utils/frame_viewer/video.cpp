#include "video.hpp"

/**
 * @fn draw_info
 * @brief Draw frame information on the video frame.
 * @param frame The video frame to draw on.
 * @param idx Current frame index.
 * @param frame_count Total number of frames.
 * @param fps Frames per second of the video.
 * @param playing Playback state (true if playing, false if paused).
 */
void draw_info(cv::Mat& frame, int idx, int frame_count, double fps,
               bool playing) {
  std::string info = cv::format("Frame: %d/%d  FPS: %.2f  %s", idx + 1,
                                frame_count, fps, playing ? "PLAY" : "PAUSE");
  cv::putText(frame, info, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.7,
              cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
}

/**
 * @fn show_frame_seek
 * @brief Seek to a specific frame and display it.
 * @param cap VideoCapture object.
 * @param window_name Name of the display window.
 * @param idx Frame index to seek to.
 * @param rotate_code Rotation code for frame orientation.
 * @param rotate_flag OpenCV RotateFlags corresponding to rotate_code.
 */
void show_frame_seek(cv::VideoCapture& cap, const std::string& window_name,
                     int idx, int rotate_code, cv::RotateFlags rotate_flag) {
  int frame_count = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
  idx = std::clamp(idx, 0, frame_count - 1);
  cap.set(cv::CAP_PROP_POS_FRAMES, idx);
  cv::Mat frame;
  if (cap.read(frame)) {
    if (rotate_code != 0) {
      cv::rotate(frame, frame, rotate_flag);
    }
    draw_info(frame, idx, frame_count, cap.get(cv::CAP_PROP_FPS), false);
    cv::imshow(window_name, frame);
  }
}

/**
 * @fn get_rotate_flag
 * @brief Get OpenCV RotateFlags based on rotation code.
 * @param rotate_code Rotation code (90, 180, 270).
 * @return Corresponding cv::RotateFlags.
 */
cv::RotateFlags get_rotate_flag(int rotate_code) {
  switch (rotate_code) {
    case 90:
      return cv::ROTATE_90_CLOCKWISE;  // wip
    case 180:
      return cv::ROTATE_90_COUNTERCLOCKWISE;  // wip
    case 270:
      return cv::ROTATE_180;
    default:
      return cv::ROTATE_90_CLOCKWISE;  // Default case
  }
}
