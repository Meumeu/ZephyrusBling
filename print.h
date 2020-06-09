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

#include <fmt/format.h>
#include <glm/gtx/string_cast.hpp>

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct fmt::formatter<glm::mat<C, R, T, Q>>
{
	constexpr auto parse(format_parse_context & ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const glm::mat<C, R, T, Q> & m, FormatContext & ctx)
	{
		return format_to(ctx.out(), "{}", glm::to_string(m));
	}
};

template <glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>>
{
	constexpr auto parse(format_parse_context & ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const glm::vec<L, T, Q> & m, FormatContext & ctx)
	{
		return format_to(ctx.out(), "{}", glm::to_string(m));
	}
};
