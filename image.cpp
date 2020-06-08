#include "image.h"

#include <stdexcept>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#pragma GCC diagnostic pop

// Basic usage (see HDR discussion below for HDR usage):
//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    // ... but 'n' will always be the number that it would have been if you said 0
//    stbi_image_free(data)

image::image(const std::string & filename)
{
	pixel * data = reinterpret_cast<pixel *>(stbi_load(filename.c_str(), &w, &h, nullptr, 2));

	if (!data)
		throw std::runtime_error("Cannot load " + filename + ": " + stbi_failure_reason());

	pixels.assign(data, data + w * h);
	stbi_image_free(data);
}

void image::negate()
{
	for (pixel & i: pixels)
	{
		i.grey = ~i.grey;
	}
}

pixel image::operator()(int x, int y)
{
	if (x < 0 || x >= w || y < 0 || y >= h)
		return {0, 0};
	else
		return pixels[x + y * w];
}
