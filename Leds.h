#pragma once

#include <array>
#include <glm/glm.hpp>

struct Led
{
	bool visible;
	glm::vec2 position;

	constexpr Led() : visible(false), position(0, 0) {}
	constexpr Led(double x, double y) : visible(true), position(x, y) {}
	constexpr Led(const Led &) = default;
};

class Leds
{
	static const std::array<Led, 1245> leds_;

public:
	static constexpr const double scale_x = 0.8; // cm
	static constexpr const double scale_y = 0.3; // cm

	// Returns the unscaled position of each LED, multiply the position by scale_[xy]
	// to have its coordinates in cm
	constexpr static const std::array<Led, 1245> & leds_position()
	{
		return leds_;
	}
};
