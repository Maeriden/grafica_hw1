//
// LICENSE:
//
// Copyright (c) 2016 -- 2017 Fabio Pellacini
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include "vmath.h"

struct image4f {
    int width = 0, height = 0;
    std::vector<vec4f> pixels;

    image4f() {}
    image4f(int w, int h) : width(w), height(h), pixels(w * h, {0, 0, 0, 0}) {}

    vec4f& at(int i, int j) { return pixels[j * width + i]; }
    const vec4f& at(int i, int j) const { return pixels[j * width + i]; }
};

struct image4b {
    int width = 0, height = 0;
    std::vector<vec4b> pixels;

    image4b() {}
    image4b(int w, int h) : width(w), height(h), pixels(w * h, {0, 0, 0, 0}) {}

    vec4b& at(int i, int j) { return pixels[j * width + i]; }
    const vec4b& at(int i, int j) const { return pixels[j * width + i]; }
};

image4f load_image4f(const std::string& filename);
image4b load_image4b(const std::string& filename);

void save_image(const std::string& filename, const image4f& img);
void save_image(const std::string& filename, const image4b& img);

image4b tonemap(
    const image4f& hdr, float exposure, bool use_filmic, bool no_srgb = false);

image4b compose(const std::vector<image4b>& imgs, bool premultiplied = false,
    bool no_srgb = false);

#endif
