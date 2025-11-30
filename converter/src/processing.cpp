#include "processing.hpp"

#include <iomanip>

#include "constants.hpp"

/**
 * @fn load_warped_frames
 * @brief Load frames from a video, apply perspective warp, and return the
 * processed frames.
 * @param video_path The path to the input video file.
 * @param M The perspective transformation matrix.
 * @param config The configuration object containing settings.
 * @return A vector of warped frames.
 */
std::vector<cv::Mat> load_warped_frames(const cv::Mat& M, Config& config) {
  std::vector<cv::Mat> frames;

  cv::VideoCapture cap(config.basics.video_path);
  if (!cap.isOpened()) {
    std::cerr << "Error: Cannot open video." << std::endl;
    exit(-1);
  }

  int start_frame = std::max(0, config.basics.start_frame);
  int end_frame = std::min(static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT)),
                           config.basics.end_frame);

  int f = start_frame;
  int step = config.basics.default_step;

  while (f < end_frame) {
    std::cout << "\rProgress: " << std::fixed << std::setprecision(2)
              << (static_cast<double>(f - start_frame) /
                  (end_frame - start_frame)) *
                     100.0
              << "% (" << f - start_frame << "/" << end_frame - start_frame
              << ")  step: " << step << "      " << std::flush;

    cap.set(cv::CAP_PROP_POS_FRAMES, f);
    cv::Mat frame;
    if (!cap.read(frame)) break;

    // adjust step size according to config
    if (config.basics.adjust_step_enabled) {
      auto it = config.step.upper_bound(f);
      if (it != config.step.begin()) {
        --it;
        step = it->second;
      }
    }
    // std::cout << "frame: " << f << ", step: " << step << std::endl;

    cv::Mat warped;
    if (static_cast<int>(cap.get(cv::CAP_PROP_ORIENTATION_META)) == 270) {
      cv::rotate(frame, frame, cv::ROTATE_180);
    }

    cv::warpPerspective(frame, warped, M, cv::Size(dst_width, dst_height),
                        cv::INTER_LINEAR);
    frames.push_back(warped);

    f += step;
  }

  cap.release();
  return frames;
}

/**
 * @fn stack_and_split
 * @brief Stack frames vertically and split them into parts when exceeding max
 * height.
 * @param frames The input frames to be stacked and split.
 * @return A vector of part images.
 */
std::vector<cv::Mat> stack_and_split(const std::vector<cv::Mat>& frames) {
  std::vector<cv::Mat> part_images;

  if (frames.empty()) return part_images;

  cv::Mat result;
  int accumulated_height = 0;

  for (auto it = frames.rbegin(); it != frames.rend(); ++it) {
    if (result.empty()) {
      result = *it;
      accumulated_height = it->rows;
    } else {
      cv::vconcat(result, *it, result);
      accumulated_height += it->rows;
    }

    if (accumulated_height >= max_height) {
      part_images.push_back(result);
      result.release();
      accumulated_height = 0;
    }
  }

  if (!result.empty()) {
    part_images.push_back(result);
  }

  return part_images;
}

/**
 * @fn generate_image
 * @brief Generate the final image by concatenating part images with gaps and
 * padding.
 * @param part_images The input part images to be concatenated.
 * @return The final padded image.
 */
cv::Mat generate_image(const std::vector<cv::Mat>& part_images,
                       Config& config) {
  if (part_images.empty()) {
    std::cerr << "No frames processed." << std::endl;
    return cv::Mat();
  }

  // align heights
  int max_h = 0;
  for (auto& img : part_images) {
    max_h = std::max(max_h, img.rows);
  }

  std::vector<cv::Mat> padded_parts;
  padded_parts.reserve(part_images.size());
  for (auto& img : part_images) {
    if (img.rows < max_h) {
      cv::Mat padded;
      cv::copyMakeBorder(img, padded, 0, max_h - img.rows, 0, 0,
                         cv::BORDER_CONSTANT, cv::Scalar(50, 50, 50));
      padded_parts.push_back(padded);
    } else {
      padded_parts.push_back(img);
    }
  }

  std::reverse(padded_parts.begin(), padded_parts.end());

  // calculate final size
  int total_width = 0;
  for (auto& img : padded_parts) {
    total_width += img.cols;
  }
  total_width += gap_width * static_cast<int>(padded_parts.size() - 1);

  // allocate output image first (initialized to black)
  cv::Mat final_image(max_h, total_width, CV_8UC3, cv::Scalar(0, 0, 0));

  // calculate positions and copy
  int x = 0;
  for (size_t i = 0; i < padded_parts.size(); i++) {
    padded_parts[i].copyTo(
        final_image(cv::Rect(x, 0, padded_parts[i].cols, max_h)));
    x += padded_parts[i].cols;
    if (i < padded_parts.size() - 1) {
      x += gap_width;
    }
  }

  // add padding around the final image
  cv::Mat final_image_padded;
  Padding pad;
  cv::copyMakeBorder(final_image, final_image_padded, pad.TOP, pad.BOTTOM,
                     pad.LEFT, pad.RIGHT, cv::BORDER_CONSTANT,
                     cv::Scalar(0, 0, 0));

  // add thumbnail image at the bottom-left corner
  cv::Mat thumbnail = cv::imread(config.basics.thumbnail_path);
  if (!thumbnail.empty()) {
    cv::resize(thumbnail, thumbnail, cv::Size(900, 900));
    thumbnail.copyTo(final_image_padded(
        cv::Rect(pad.LEFT + 10,
                 final_image_padded.rows - pad.BOTTOM - 10 - thumbnail.rows,
                 thumbnail.cols, thumbnail.rows)));
  }

  // add Difficulty text above the thumbnail
  if (difficulty_map.find(config.basics.difficulty) != difficulty_map.end()) {
    cv::Scalar color(difficulty_map.at(config.basics.difficulty)[0],
                     difficulty_map.at(config.basics.difficulty)[1],
                     difficulty_map.at(config.basics.difficulty)[2]);
    int font_face = cv::FONT_HERSHEY_SIMPLEX;
    double font_scale = 6.5;
    int thickness = 20;
    int baseline = 0;
    cv::Size text_size = cv::getTextSize(config.basics.difficulty, font_face,
                                         font_scale, thickness, &baseline);
    cv::Point text_org(pad.LEFT + 10, final_image_padded.rows - pad.BOTTOM -
                                          thumbnail.rows - text_size.height +
                                          80);
    cv::putText(final_image_padded, config.basics.difficulty, text_org,
                font_face, font_scale, color, thickness, cv::LINE_AA);
  }

  return final_image_padded;
}

/**
 * @fn save_final_image
 * @brief Save the final image to the specified output directory with given
 * filename.
 * @param final_image The final image to be saved.
 * @param config The configuration object containing settings.
 */
void save_final_image(const cv::Mat& final_image, Config& config) {
  if (final_image.empty()) {
    std::cerr << "Final image is empty. Nothing to save." << std::endl;
    return;
  }
  std::string output_path =
      config.basics.output_dir + "/" + config.basics.output_name + ".jpg";
  std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY,
                             config.basics.jpeg_quality};
  cv::imwrite(output_path, final_image, params);
  std::cout << "Saved: " << output_path << std::endl;
}
