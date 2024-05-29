#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
  cv::Mat image = cv::Mat::zeros(100, 100, CV_8UC3);
  if (image.empty()) {
      std::cerr << "Could not create image" << std::endl;
      return -1;
  }
  std::cout << "Image created successfully" << std::endl;
  return 0;
}
