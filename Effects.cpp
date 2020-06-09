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

#include "Effects.h"

#include <glm/gtx/matrix_transform_2d.hpp>

Rotate::~Rotate() {}
Translate::~Translate() {}
Scale::~Scale() {}
Brightness::~Brightness() {}
Alpha::~Alpha() {}

glm::mat3 Rotate::transform(float t)
{
	return glm::rotate(glm::mat3(1.0), get_value_at(t));
}

glm::mat3 Translate::transform(float t)
{
	return glm::translate(glm::mat3(1.0), get_value_at(t));
}

glm::mat3 Scale::transform(float t)
{
	return glm::scale(glm::mat3(1.0), get_value_at(t));
}

double Brightness::brightness(float t)
{
	return get_value_at(t);
}

double Alpha::alpha(float t)
{
	return get_value_at(t);
}
