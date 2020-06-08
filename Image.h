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
