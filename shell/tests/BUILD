cc_test(
  name = "unit_tests",
  srcs = glob(["unit/*_test.cpp"]),
  deps = ["//shell:shell",
          "@googletest//:gtest_main"]
)

cc_test(
  name = "integration_tests",
  srcs = ["integration/integration_tests.cpp"],
  deps = ["//shell:shell",
          "@googletest//:gtest_main"]
)
