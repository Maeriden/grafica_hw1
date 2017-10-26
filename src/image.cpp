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

#include "image.h"

// needed for image loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// needed for image writing
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#if !defined(DEBUGBREAK)
	#if defined(__clang__)
		#define DEBUGBREAK __builtin_trap
	#elif defined(__GNUC__)
		#define DEBUGBREAK __builtin_trap
	#elif defined(_MSC_VER)
		#define DEBUGBREAK __debugbreak
	#endif
#endif


#if ENABLE_ASSERT
	#define ASSERT(c) do{ if(!(c)) DEBUGBREAK(); } while(0)
#else
	#define ASSERT(c) do{ (void)sizeof(c); } while(0)
#endif


static inline
float
filmic(float x)
{
	float num = 2.51f*x*x + 0.03f*x;
	float den = 2.43f*x*x + 0.59f*x + 0.14f;
	return num / den;
}


static inline
vec4f
normalize255(vec4b v)
{
	vec4f result = {
		v.x / 255.0f,
		v.y / 255.0f,
		v.z / 255.0f,
		v.w / 255.0f
	};
	return result;
}


image4f load_image4f(const std::string& filename)
{
	int    w     = 0;
	int    h     = 0;
	int    chans = 0;
	float* data  = stbi_loadf(filename.c_str(), &w, &h, &chans, 4);
	ASSERT(data);
	
	image4f result = image4f(w, h);
	for(int i = 0; i < w*h; ++i)
	{
		float* pixel = data + (i*4);
		result.pixels[i] = {pixel[0], pixel[1], pixel[2], pixel[3]};
	}
	stbi_image_free(data);
	
	return result;
}


image4b load_image4b(const std::string& filename)
{
	int      w     = 0;
	int      h     = 0;
	int      chans = 0;
	stbi_uc* data  = stbi_load(filename.c_str(), &w, &h, &chans, 4);
	ASSERT(data);
	
	image4b result = image4b(w, h);
	for(int i = 0; i < w*h; ++i)
	{
		stbi_uc* pixel = data + (i*4);
		result.pixels[i] = {pixel[0], pixel[1], pixel[2], pixel[3]};
	}
	stbi_image_free(data);
	
	return result;
}


void save_image(const std::string& filename, const image4f& img)
{
	int success = stbi_write_hdr(filename.c_str(),
	                             img.width,
	                             img.height,
	                             4,
	                             (float*)img.pixels.data());
	ASSERT(success);
}


void save_image(const std::string& filename, const image4b& img)
{
	int success = stbi_write_png(filename.c_str(),
	                             img.width,
	                             img.height,
	                             4,
	                             img.pixels.data(),
	                             img.width*sizeof(float));
	ASSERT(success);
}


image4b tonemap(const image4f& hdr, float exposure, bool use_filmic, bool no_srgb)
{
	constexpr float INVERSE_GAMMA = 1.0f/2.2f;
	bool output_in_sRGB = !no_srgb;
	
	image4b result = image4b(hdr.width, hdr.height);
	for(int i = 0; i < hdr.pixels.size(); ++i)
	{
		vec4f pixel = hdr.pixels[i];
		
		// Scale by exposure
		pixel.x = pow(2.0f, exposure) * pixel.x;
		pixel.y = pow(2.0f, exposure) * pixel.y;
		pixel.z = pow(2.0f, exposure) * pixel.z;
		
		if(use_filmic)
		{
			// Apply filmic filter
			pixel.x = filmic(pixel.x);
			pixel.y = filmic(pixel.y);
			pixel.z = filmic(pixel.z);
		}
		
		if(output_in_sRGB)
		{
			// Map color intensities to sRGB color space (make them brighter)
			// We save them applying reverse gamma so that when the display
			// hardware (driver?) applies gamma correction, the resulting color
			// is what we wanted to show
			pixel.x = pow(pixel.x, INVERSE_GAMMA);
			pixel.y = pow(pixel.y, INVERSE_GAMMA);
			pixel.z = pow(pixel.z, INVERSE_GAMMA);
		}
		
		// Clamp to [0, 1]
		pixel.x = min(pixel.x, 1.0f);
		pixel.y = min(pixel.y, 1.0f);
		pixel.z = min(pixel.z, 1.0f);
		
		// Map to [0, 255] and cast to int
		result.pixels[i].x = 255.0f * pixel.x;
		result.pixels[i].y = 255.0f * pixel.y;
		result.pixels[i].z = 255.0f * pixel.z;
		result.pixels[i].w = 255.0f * pixel.w;
	}
	
	return result;
}


image4b compose(const std::vector<image4b>& imgs, bool premultiplied, bool no_srgb)
{
	if(imgs.size() == 0)
		return image4b();
	
	constexpr float INVERSE_GAMMA = 1.0f/2.2f;
	bool output_in_sRGB = !no_srgb;
	
	image4b result = image4b(imgs[0].width, imgs[0].height);
	for(int image_index = 0; image_index < imgs.size(); ++image_index)
	{
		const image4b& source = imgs[image_index];
		
		for(int i = 0; i < source.pixels.size(); ++i)
		{
			vec4f pixel_above = normalize255(source.pixels[i]);
			vec4f pixel_below = normalize255(result.pixels[i]);
			
			if(output_in_sRGB)
			{
				pixel_above.x = pow(pixel_above.x, 2.2f);
				pixel_above.y = pow(pixel_above.y, 2.2f);
				pixel_above.z = pow(pixel_above.z, 2.2f);
				
				pixel_below.x = pow(pixel_below.x, 2.2f);
				pixel_below.y = pow(pixel_below.y, 2.2f);
				pixel_below.z = pow(pixel_below.z, 2.2f);
			}
			
			if(!premultiplied)
			{
				// Premultiply
				pixel_above.x *= pixel_above.w;
				pixel_above.y *= pixel_above.w;
				pixel_above.z *= pixel_above.w;
			}
			
			// Blend One OneMinusSrcAlpha
			vec4f pixel = pixel_above + pixel_below * (1.0f - pixel_above.w);
			
			if(output_in_sRGB)
			{
				pixel.x = pow(pixel.x, INVERSE_GAMMA);
				pixel.y = pow(pixel.y, INVERSE_GAMMA);
				pixel.z = pow(pixel.z, INVERSE_GAMMA);
			}
			
			result.pixels[i].x = 255.0f * pixel.x;
			result.pixels[i].y = 255.0f * pixel.y;
			result.pixels[i].z = 255.0f * pixel.z;
			result.pixels[i].w = 255.0f * pixel.w;
		}
	}
	
	return result;
}
