# Heart rate monitor

This project parses video to monitor heart rate in BPM.

## Prerequisits

Build OpenCV [(linux reference)](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html). There is a convenience script that will fetch and build and install the opencv in the local directory using the following commands:

```sh
cd third_party/opencv
./build-install-opencv.sh
```

## Resources

1. Install bazelisk: [bazelisk reference](https://docs.bazel.build/versions/5.4.1/install-bazelisk.html)
2. Formatting library: [fmt](https://github.com/fmtlib/fmt)
3. Plotting library: [matplot++](https://github.com/alandefreitas/matplotplusplus)
