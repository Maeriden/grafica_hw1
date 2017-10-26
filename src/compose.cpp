
#include "image.h"

#include "yocto_utils.h"

int main(int argc, char** argv) {
    // command line parsing
    auto parser =
        yu::cmdline::make_parser(argc, argv, "compose", "compose images");
    auto no_srgb = yu::cmdline::parse_flag(
        parser, "--no-srgb", "", "disable srgb correction");
    auto imageout = yu::cmdline::parse_opts(
        parser, "--output", "-o", "output image", "out.png");
    auto imagesin = yu::cmdline::parse_argas(
        parser, "imagein", "input images", {}, -1, true);
    yu::cmdline::check_parser(parser);

    // load images and tonemap
    auto imgs = std::vector<image4b>();
    for (auto imagein : imagesin) imgs.push_back(load_image4b(imagein));
    auto comp = compose(imgs, false, no_srgb);
    save_image(imageout, comp);
}
