cc_library(
    name = "simple_thread_pool",
    hdrs = ["thread_pool.h"],
    srcs = ["thread_pool.cpp"]
)

cc_binary(
    name = "thread_pool_example",
    srcs = ["thread_pool_example.cpp"],
    deps = [":simple_thread_pool"],
)
