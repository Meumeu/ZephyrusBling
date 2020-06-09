/*
 * ZephyrusBling, a utility to display images on the LEDs of Zephyrus G14 laptop
 * Copyright (C) 2020 Guillaume Meunier <guillaume.meunier@centraliens.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Image.h"

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

Image::Image(const std::string & filename)
{
	pixel * data = reinterpret_cast<pixel *>(stbi_load(filename.c_str(), &w, &h, nullptr, 2));

	if (!data)
		throw std::runtime_error("Cannot load " + filename + ": " + stbi_failure_reason());

	pixels.assign(data, data + w * h);

	for (pixel & i: pixels)
	{
		i.grey = i.grey * i.alpha / 255;
	}

	stbi_image_free(data);
}

Image::Image(gsl::span<uint8_t> buffer)
{
	pixel * data =
	        reinterpret_cast<pixel *>(stbi_load_from_memory(buffer.data(), buffer.size(), &w, &h, nullptr, 2));

	if (!data)
		throw std::runtime_error(std::string("Cannot load from memory: ") + stbi_failure_reason());

	pixels.assign(data, data + w * h);

	for (pixel & i: pixels)
	{
		i.grey = i.grey * i.alpha / 255;
	}

	stbi_image_free(data);
}

void Image::negate()
{
	for (pixel & i: pixels)
	{
		i.grey = ~i.grey;
	}
}

pixel Image::operator()(int x, int y) const
{
	if (x < 0 || x >= w || y < 0 || y >= h)
		return {0, 0};
	else
		return pixels[x + y * w];
}
