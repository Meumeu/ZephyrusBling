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

#include "Bling.h"

class Rotate : public BlingFx<Rotate, float>
{
public:
	glm::mat3 transform(float t) override;
	~Rotate();
};

class Translate : public BlingFx<Translate, glm::vec2>
{
public:
	glm::mat3 transform(float t) override;
	~Translate();
};

class Scale : public BlingFx<Scale, glm::vec2>
{
public:
	glm::mat3 transform(float t) override;
	~Scale();
};

class Brightness : public BlingFx<Brightness, float>
{
public:
	double brightness(float t) override;
	~Brightness();
};

class Alpha : public BlingFx<Alpha, float>
{
public:
	double alpha(float t) override;
	~Alpha();
};
