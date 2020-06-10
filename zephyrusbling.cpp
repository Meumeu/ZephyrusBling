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

#include "BlingDaemon.h"
#include <CLI/CLI.hpp>
#include <boost/lexical_cast.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <gsl/span>
#include <gsl/span_ext>
#include <iostream>

namespace
{
void split_string(std::vector<std::string_view> & split, std::string_view s, char separator)
{
	split.clear();

	size_t n;

	while (true)
	{
		n = s.find_first_of(separator);
		split.emplace_back(s.substr(0, n));

		if (n == std::string_view::npos)
			break;

		s = s.substr(n + 1);
	}
}

std::vector<sdbus::Struct<double, double>> vector_to_vector_of_tuple_2(gsl::span<std::string_view> values)
{
	std::vector<sdbus::Struct<double, double>> ret;
	std::vector<std::string_view> split;

	if (values.size() == 1)
	{
		split_string(split, values[0], ',');
		if (split.size() != 1)
			throw std::invalid_argument("Invalid argument");

		ret.emplace_back(0, boost::lexical_cast<double>(split[0]));

		return ret;
	}

	for (auto & i: values)
	{
		split_string(split, i, ',');
		if (split.size() != 2)
			throw std::invalid_argument("Invalid argument");

		ret.emplace_back(boost::lexical_cast<double>(split[0]), boost::lexical_cast<double>(split[1]));
	}

	return ret;
}

std::vector<sdbus::Struct<double, double, double>> vector_to_vector_of_tuple_3(gsl::span<std::string_view> values)
{
	std::vector<sdbus::Struct<double, double, double>> ret;
	std::vector<std::string_view> split;

	if (values.size() == 1)
	{
		split_string(split, values[0], ',');
		if (split.size() != 2)
			throw std::invalid_argument("Invalid argument");

		ret.emplace_back(0, boost::lexical_cast<double>(split[0]), boost::lexical_cast<double>(split[1]));

		return ret;
	}

	for (auto & i: values)
	{
		split_string(split, i, ',');
		if (split.size() != 3)
			throw std::invalid_argument("Invalid argument");

		ret.emplace_back(boost::lexical_cast<double>(split[0]), boost::lexical_cast<double>(split[1]),
		                 boost::lexical_cast<double>(split[2]));
	}

	return ret;
}
} // namespace

int main(int argc, char ** argv)
{
	std::string image;
	std::string text;
	std::string font;
	std::vector<std::string> fx;
	double duration = 3;
	int z_order = 0;

	CLI::App app("description");

	std::map<std::string, double> time_units{{"s", 1}, {"ms", 0.001}};

	auto opt_image = app.add_option("--image", image, "Image to display")->check(CLI::ExistingFile);
	auto opt_text = app.add_option("--text", text, "Text to display");

	auto opt_font = app.add_option("--font", font, "Font to use");
	auto opt_fx = app.add_option("--fx", fx, "Transform to apply, see below");
	auto opt_duration =
	        app.add_option("--duration", duration, "How long the text or image is displayed")
	                ->transform(CLI::AsNumberWithUnit{time_units, CLI::AsNumberWithUnit::CASE_SENSITIVE, "s"});

	auto opt_zorder = app.add_option("--z-order", z_order, "Not implemented");

	opt_image->excludes(opt_text);
	opt_font->needs(opt_text);

	app.footer(
	        R"(Exactly one of --text, --image is required.

By default, the text or image disappears after 3 seconds, use the --duration
option to override it.

Each --fx specifies a transform to apply, with the following format:
effect:params(:params)*

Each param is a sequence of comma separated floating point numbers whose meaning
depends on the effect.

If only one param is given for the effect, it stays constant while the image is
displayed. If there are several, the first number of each param is a timestamp
since the image is first displayed and the others are the arguments. In the latter
case, the arguments are linearly interpolated with respect to time.

The effect can be:
- translate: the 2 arguments are the displacement in cm on the X and Y axes
- rotate: the argument is the angle in radian
- scale: the 2 arguments are the scaling factor on the X and Y axes
- brightness: the argument is a multiplier applied on the grey channel of the image
- alpha: the argument is a multiplier applied on the alpha channel of the image)");

	app.get_formatter()->column_width(40);

	CLI11_PARSE(app, argc, argv);

	if (app.count("--text") + app.count("--image") != 1)
	{
		std::cerr << app.help();
		return 1;
	}

	sdbus::ObjectPath bling_path;
	std::unique_ptr<sdbus::IConnection> session_bus;
	std::unique_ptr<BlingDaemonProxy> daemon;

	try
	{
		session_bus = sdbus::createSessionBusConnection();
		daemon = std::make_unique<BlingDaemonProxy>(*session_bus);

		if (app.count("--text"))
		{
			bling_path = daemon->CreateText(text, font);
		}
		else if (app.count("--image"))
		{
			bling_path = daemon->CreateImage(std::filesystem::absolute(image));
		}
		else
		{
			assert(false);
		}

		BlingProxy bling(*session_bus, bling_path);

		std::vector<std::string_view> split;
		for (std::string & i: fx)
		{
			split_string(split, i, ':');

			assert(!split.empty());
			std::string_view type = split.front();
			gsl::span<std::string_view> data = gsl::make_span(split).subspan(1);

			if (type == "translate")
				bling.AddTranslate(vector_to_vector_of_tuple_3(data));
			else if (type == "rotate")
				bling.AddRotate(vector_to_vector_of_tuple_2(data));
			else if (type == "scale")
				bling.AddScale(vector_to_vector_of_tuple_3(data));
			else if (type == "brightness")
				bling.AddBrightness(vector_to_vector_of_tuple_2(data));
			else if (type == "alpha")
				bling.AddAlpha(vector_to_vector_of_tuple_2(data));
			else
				throw std::invalid_argument("Invalid effect: " + std::string(type));
		}

		daemon->Show(bling_path, duration, z_order);

		fmt::print("Bling {} created\n", bling_path);
	}
	catch (std::exception & e)
	{
		fmt::print(stderr, "{}\n", e.what());

		if (bling_path != "")
			daemon->Destroy(bling_path);

		return 1;
	}
}
