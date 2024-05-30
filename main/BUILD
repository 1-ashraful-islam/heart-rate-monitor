load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "monitor",
    srcs = ["main.cc"],
    deps = [
        "@fmt",
        "@opencv",
    ],
)

cc_binary(
    name = "monitor-threaded",
    srcs = ["multiThreaded.cc"],
    deps = [
        "@fmt",
        "@opencv",
    ],
)
