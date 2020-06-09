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

#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <gsl/span>
#include <string>
#include <vector>

struct pixel
{
	uint8_t grey;
	uint8_t alpha;
};

struct Image
{
	int w = 0, h = 0;
	std::vector<pixel> pixels;

	Image(const std::string & filename);
	Image(gsl::span<uint8_t> data);

	Image() = default;
	Image(const Image &) = default;
	Image(Image &&) = default;
	Image & operator=(const Image &) = default;
	Image & operator=(Image &&) = default;

	void negate();

	pixel operator()(int x, int y) const;

	pixel operator()(glm::vec2 position) const
	{
		return operator()(position.x, position.y);
	}

	pixel operator()(glm::vec3 position) const
	{
		return operator()(position.x, position.y);
	}
};
