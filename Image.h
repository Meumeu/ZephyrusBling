#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct pixel
{
	uint8_t grey;
	uint8_t alpha;
};

struct Image
{
	int w, h;
	std::vector<pixel> pixels;

	Image(const std::string & filename);

	Image(const Image &) = default;
	Image(Image &&) = default;

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
