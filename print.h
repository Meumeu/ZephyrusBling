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
