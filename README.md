# Heart rate monitor

This project parses video to monitor heart rate in BPM.

## Prerequisits

Build OpenCV [(linux reference)](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html). There is a convenience script that will fetch and build and install the opencv in the local directory using the following commands:

```sh
cd third_party/opencv
./build-install-opencv.sh
```

Install Bazel using [bazelisk](https://docs.bazel.build/versions/5.4.1/install-bazelisk.html).

## Running the program

This program takes two arguments: 1. working directory, 2. video_file name. The program *assumes* video file is in the working directory. Output of the program is a `stdout` reading of the BPM and, and a `processed_video.mp4` in the working directory. In addition, it also outputs number of processsed frame and execution time in the `stdout`.

To build and run the program, if you have `codingtest.mov` in the `$PWD` directory use:

```sh
bazel run //main:monitor $PWD codingtest.mov
```

## Extras

See the *Python* implementation details and additional plots [here](python/README.md).

## Resources

1. Install bazelisk: [bazelisk reference](https://docs.bazel.build/versions/5.4.1/install-bazelisk.html)
2. Formatting library: [fmt](https://github.com/fmtlib/fmt)
<!-- 3. Plotting library: [matplot++](https://github.com/alandefreitas/matplotplusplus) -->
