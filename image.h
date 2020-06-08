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

struct image
{
	int w, h;
	std::vector<pixel> pixels;

	image(const std::string & filename);

	image(const image &) = default;
	image(image &&) = default;

	void negate();

	pixel operator()(int x, int y);

	pixel operator()(glm::vec2 position)
	{
		return operator()(position.x, position.y);
	}

	pixel operator()(glm::vec3 position)
	{
		return operator()(position.x, position.y);
	}
};
