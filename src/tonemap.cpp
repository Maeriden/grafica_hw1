#include "image.h"
#include "yocto_utils.h"

int main(int argc, char** argv) {
    // command line parsing
    auto parser =
        yu::cmdline::make_parser(argc, argv, "tonemap", "tonemap images");
    auto filmic = yu::cmdline::parse_flag(
        parser, "--filmic", "-f", "enable filmic tone mapping");
    auto no_srgb = yu::cmdline::parse_flag(
        parser, "--no-srgb", "", "disable srgb correction");
    auto exposure =
        yu::cmdline::parse_optf(parser, "--exposure", "-e", "exposure", 0);
    auto imageout = yu::cmdline::parse_opts(
        parser, "--output", "-o", "output image", "out.png");
    auto imagein =
        yu::cmdline::parse_args(parser, "imagein", "input image", "", true);
    yu::cmdline::check_parser(parser);

    // load images and tonemap
    auto hdr = load_image4f(imagein);
    auto ldr = tonemap(hdr, exposure, filmic, no_srgb);
    save_image(imageout, ldr);
}
