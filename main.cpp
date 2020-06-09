#include "BlingDaemon.h"
#include <CLI/CLI.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/lexical_cast.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <gsl/span>
#include <gsl/span_ext>
#include <iostream>

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

int main(int argc, char ** argv)
{
	bool daemon;
	std::string image;
	std::string text;
	std::string font;
	std::vector<std::string> fx;
	double duration = 3;
	int z_order = 0;

	CLI::App app("description");

	app.add_flag("--daemon,-d", daemon);
	app.add_option("--image", image, "Display an image");
	app.add_option("--text", text, "Display a text");
	app.add_option("--font", font);
	app.add_option("--fx", fx);
	app.add_option("--duration", duration, "Duration in seconds");
	app.add_option("--z-order", z_order);

	CLI11_PARSE(app, argc, argv);

	if (app.count("--daemon") + app.count("--text") + app.count("--image") != 1)
	{
		std::cerr << "Exactly one of --daemon, --image, --text is required\n";
		return 1;
	}

	if (daemon)
	{
		// Create boost::asio context
		boost::asio::io_context io;

		// Create D-Bus connection to the session bus and requests name on it.
		std::unique_ptr<sdbus::IConnection> session_bus = sdbus::createSessionBusConnection("org.meumeu.bling");

		// Quit on SIGINT or SIGTERM
		boost::asio::signal_set signals(io, SIGINT, SIGTERM);
		signals.async_wait([&io, &session_bus](const boost::system::error_code & error, int signal_number) {
			io.stop();
			session_bus->leaveEventLoop();
		});

		BlingDaemon daemon(io, *session_bus);

		session_bus->enterEventLoopAsync();
		io.run();

		daemon.clear();

		return 0;
	}
	else
	{
		sdbus::ObjectPath bling_path;
		std::unique_ptr<sdbus::IConnection> session_bus;
		std::unique_ptr<BlingDaemonProxy> daemon;

		try
		{
			session_bus = sdbus::createSessionBusConnection();
			daemon = std::make_unique<BlingDaemonProxy>(*session_bus);

			if (app.count("--text"))
			{
				if (app.count("--font"))
					bling_path = daemon->CreateText(text, font);
				else
					bling_path = daemon->CreateText(text, "");
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
				{
					bling.AddTranslate(vector_to_vector_of_tuple_3(data));
				}
				else if (type == "rotate")
				{
					bling.AddRotate(vector_to_vector_of_tuple_2(data));
				}
				else if (type == "scale")
				{
					bling.AddScale(vector_to_vector_of_tuple_3(data));
				}
				else if (type == "brightness")
				{
					bling.AddBrightness(vector_to_vector_of_tuple_2(data));
				}
				else if (type == "alpha")
				{
					bling.AddAlpha(vector_to_vector_of_tuple_2(data));
				}
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
}
