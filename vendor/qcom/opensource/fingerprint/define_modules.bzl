load("//build/kernel/kleaf:kernel.bzl", "ddk_module")
load("//build/bazel_common_rules/dist:dist.bzl", "copy_to_dist_dir")

def define_basic_modules(targets, variants):
    for t in targets:
        for v in variants:
            define_modules(t, v)

def define_modules(target, variant):
    tv = "{}_{}".format(target, variant)
    rule_base = "{}_goodix_fp".format(tv)

    ddk_module(
        name = rule_base,
        out = "goodix_fp.ko",
        deps = ["//msm-kernel:all_headers"],
        srcs = [
            "goodix/gf_spi.c",
            "goodix/netlink.c",
            "goodix/platform.c",
            "goodix/gf_spi.h"
        ],
        includes = ["include/linux"],
        kernel_build = "//msm-kernel:{}".format(tv),
        visibility = ["//visibility:public"]
    )

    copy_to_dist_dir(
        name = "{}_dist".format(rule_base),
        data = [":{}".format(rule_base)],
        dist_dir = "../out/target/product/{}/dlkm/lib/modules/".format(target),
        flat = True,
        wipe_dist_dir = False,
        allow_duplicate_filenames = False,
        mode_overrides = {"**/*": "644"},
        log = "info",
    )
