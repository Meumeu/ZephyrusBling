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
