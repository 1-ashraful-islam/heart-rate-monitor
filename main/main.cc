#include <opencv2/opencv.hpp>
#include <vector>
#include <fmt/format.h>
#include <chrono>

double calculateBPM(const std::vector<double> &signal, int fps)
{
  int window_size = signal.size();
  cv::Mat dataInput(signal, true);

  // perform DFT
  cv::Mat dftOut;
  cv::dft(dataInput, dftOut, cv::DFT_COMPLEX_OUTPUT);

  // Split the results into real and imaginary parts
  cv::Mat planes[2];
  cv::split(dftOut, planes);

  // Compute the magnitude
  cv::Mat magnitude;
  cv::magnitude(planes[0], planes[1], magnitude);

  // compute the power spectrum (magnitude * magnitude)
  cv::Mat psd = magnitude.mul(magnitude);

  // switch to logarithmic scale
  cv::log(psd, psd);

  // compute the frequency axis, and the relevant power spectrum
  std::vector<double> frequencies(window_size / 2 + 1);
  std::vector<double> power(window_size / 2 + 1);

  for (int i = 0; i <= window_size / 2; i++)
  {
    frequencies[i] = i * static_cast<double>(fps) / window_size;

    // From Jim W. bpm detected by Presage is 80-180bpm, 1.33 to 3 hz
    // well trained athlete can have as low as 40bpm, 0.66 hz
    // Filter frequencies outside the range of interest (0.66 Hz to 3 Hz)
    if (frequencies[i] >= 0.66 && frequencies[i] <= 3.0)
    {
      power[i] = psd.at<double>(i);
    }
  }

  // Find the frequency with the highest power
  int highest_peak = std::distance(power.begin(), std::max_element(power.begin(), power.end()));
  double bpm = frequencies[highest_peak] * 60;

  return bpm;
}

int main(int argc, char *argv[])
{

  auto start_time = std::chrono::high_resolution_clock::now();

  // check the number of arguments
  if (argc < 2)
  {
    fmt::print(stderr, "Usages: \n\
    Produces heart rate from the video analysis. Please provide absolute path for files.\n\
    Arguments:\n\
    \t<video_file> - Required. Path to the input video file.\n\
    \t[video_out_file] - Optional. Path for saving the processed output video.\n\
    Examples:\n\
    \t>> bazel run //main:monitor $PWD/codingtest.mov\n\
    \t>> bazel run //main:monitor $PWD/codingtest.mov $PWD/processed_video.mp4\n");
    return -1;
  }

  // open video capture from the video file
  auto sourceFile = fmt::format("{}", argv[1]);
  auto source = cv::VideoCapture(sourceFile);
  if (!source.isOpened())
  {
    fmt::print(stderr, "Error: VideoCapture failed to open source: {}", sourceFile);
    return -1;
  }

  // video paramters
  auto fps = 30;
  auto frameWidth = source.get(cv::CAP_PROP_FRAME_WIDTH);
  auto frameHeight = source.get(cv::CAP_PROP_FRAME_HEIGHT);
  cv::Size frameSize(frameWidth, frameHeight);

  // setup video writer
  cv::VideoWriter videoOut;
  if (argc > 2)
  {
    auto videoOutFile = fmt::format("{}", argv[2]);
    videoOut = cv::VideoWriter(videoOutFile, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps, frameSize);
  }

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
  int window_size = 120;

  while (true)
  {
    tickMeter.start();
    source >> frame;
    if (frame.empty())
    {
      break;
    }
    count++;

    // calculate the mean over the ROI
    auto average = cv::mean(frame, mask);
    green_t.push_back(average[1]);

    if (videoOut.isOpened())
    {
      // calculate bpm with rolling window and annotate
      if (static_cast<int>(green_t.size()) > window_size)
      {
        std::vector<double> windowed_signal(green_t.end() - window_size, green_t.end());
        auto bpm = calculateBPM(windowed_signal, fps);
        auto text = fmt::format("BPM: {:.3f}", bpm);
        cv::putText(frame, text, top_left, fontFace, fontScale, fontColor, thickness, cv::LINE_AA);
      }

      // annotate the ROI and channel averages
      auto text = fmt::format("Red: {:.2f} Green: {:.2f} Blue: {:.2f} FPS: {:.2f}", average[2], average[1], average[0], tickMeter.getFPS());
      cv::rectangle(frame, top_left, bottom_right, fontColor, thickness, cv::LINE_8);
      cv::putText(frame, text, textOrigin, fontFace, fontScale, fontColor, thickness, cv::LINE_AA);

      // write the video frames
      videoOut.write(frame);
    }
    tickMeter.stop();
  }

  // perform the bpm calculation over entire clip
  auto bpm = calculateBPM(green_t, fps);
  fmt::print("BPM: {:.3f}\n", bpm);

  // performance stats
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end_time - start_time;
  fmt::print("\n\nProcessed frame count: {}.\nExecution took: {:.3f} seconds\n", count, duration.count());

  return 0;
}
