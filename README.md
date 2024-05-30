# Heart rate monitor

This project parses video to monitor heart rate in BPM.

## Prerequisit

Build OpenCV [(linux reference)](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html). There is a convenience script that will fetch and build and install the opencv in the local directory using the following commands:

```sh
cd third_party/opencv
./build-install-opencv.sh
```

Install Bazel using [bazelisk](https://docs.bazel.build/versions/5.4.1/install-bazelisk.html).

## Usages

The program produces heart rate from the video analysis. Please provide absolute path for files. Output of the program is a `stdout` reading of the BPM and, and a `processed_video.mp4` in the specified directory. In addition, it also outputs number of processsed frame and execution time in the `stdout`.

Arguments:
  1. `<video_file>` - Required. Path to the input video file.
  2. `[video_out_file]` - Optional. Path for saving the processed output video.

Examples commands:
  1. `bazel run //main:monitor $PWD/codingtest.mov`
  2. `bazel run //main:monitor $PWD/codingtest.mov $PWD/processed_video.mp4`


## Performance Measurements


| package | VideoWrite| RUN1 | RUN2| RUN3| AVERAGE|
|---|---|---|---|---|---|
|`main:monitor`|No|3.196 | 2.792 | 2.685 | 2.891|
|`main:monitor`|Yes|11.145 | 11.239 | 10.997 | 11.127|
|---|---|---|---|---|---|


## Limitations and potential work around

1. If you are building and running this program on *Apple silicon mac*, there might be some issue with missing aarm64 version of libraries. One known workaround albeit with some limitations is using docker with debian based image to build and install the program.

## Extras

See the *Python* implementation details and additional plots [here](python/README.md).



## Resources

1. Install bazelisk: [bazelisk reference](https://docs.bazel.build/versions/5.4.1/install-bazelisk.html)
2. Formatting library: [fmt](https://github.com/fmtlib/fmt)
3. You can convert the processed video for wider device support using FFMPEG: `ffmpeg -i processed_video.mp4 -c:v libx264 -preset slow -crf 22 -c:a aac -b:a 128k processed_video_x264.mp4`
<!-- 3. Plotting library: [matplot++](https://github.com/alandefreitas/matplotplusplus) -->
