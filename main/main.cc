#include <opencv2/opencv.hpp>
#include <vector>
#include <fmt/core.h>

int main(int argc, char *argv[])
{
  // read the video file from the argument
  if (argc != 3)
  {
    fmt::print(stderr, "Required argument: <working_directory> <video_file>\nExample: $PWD codingtest.mov");
    return -1;
  }

  auto sourceFile = fmt::format("{}/{}", argv[1], argv[2]);

  auto source = cv::VideoCapture(sourceFile);

  if (!source.isOpened())
  {
    fmt::print(stderr, "Error: VideoCapture failed to open source: {}", sourceFile);
    return -1;
  }

  // video writer paramters
  auto videoOutFile = fmt::format("{}/processed_video.mp4", argv[1]);
  auto fps = 30;
  auto frameWidth = source.get(cv::CAP_PROP_FRAME_WIDTH);
  auto frameHeight = source.get(cv::CAP_PROP_FRAME_HEIGHT);
  cv::Size frameSize(frameWidth, frameHeight);
  cv::VideoWriter videoOut(videoOutFile, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, frameSize);

  // ROI
  cv::Point top_left(345, 185);
  cv::Point bottom_right(580, 400);

  // Create a mask with ROI
  cv::Mat mask = cv::Mat::zeros(frameSize, CV_8UC1);
  cv::rectangle(mask, top_left, bottom_right, cv::Scalar(255), cv::FILLED);

  // annotation text parameters
  int fontFace = cv::FONT_HERSHEY_PLAIN;
  double fontScale = 1.5;
  cv::Scalar fontColor(0, 255, 0);
  int thickness = 2;
  cv::Point textOrigin(50, 100);

  // performance stat
  cv::TickMeter tickMeter;

  std::vector<double> green_t;
  cv::Mat frame;
  int count = 0;

  while (true)
  {
    tickMeter.start();
    count++;
    source >> frame;
    if (frame.empty())
    {
      fmt::print("No more frames available. Processed frame count: {}", count - 1);
      break;
    }
    auto average = cv::mean(frame, mask);
    green_t.push_back(average[1]);

    auto text = fmt::format("Red: {:.2f} Green: {:.2f} Blue: {:.2f} FPS: {:.2f}", average[2], average[1], average[0], tickMeter.getFPS());
    cv::rectangle(frame, top_left, bottom_right, fontColor, thickness, cv::LINE_8);
    cv::putText(frame, text, textOrigin, fontFace, fontScale, fontColor, thickness, cv::LINE_AA);
    videoOut.write(frame);
    tickMeter.stop();
  }

  return 0;
}
