#include "BlingDaemon.h"

#include "Bling.h"
#include "Leds.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <fmt/format.h>
#include <thread>

namespace
{
std::string create_id()
{
	static uint64_t next = 1;
	return fmt::format("/org/meumeu/blings/{}", next++);
}
} // namespace

void BlingDaemon::CreateImage(sdbus::Result<sdbus::ObjectPath> && result, std::string image)
{
	std::thread([this, result = std::move(result), image = std::move(image)]() {
		try
		{
			std::string id = create_id();

			auto bling = std::make_unique<Bling>(connection_, id, Image{image});

			auto [it, inserted] = [this, &id, &bling]() {
				std::scoped_lock<std::mutex> _(blings_lock_);
				return blings_.emplace(id, std::move(bling));
			}();

			assert(inserted);

			it->second->id = id;

			fmt::print("Created bling from image ({})\n", image);
			result.returnResults(id);
		}
		catch (std::exception & e)
		{
			result.returnError({"org.meumeu.bling.CreateImageError", e.what()});
		}
	}).detach();
}

void BlingDaemon::CreateText(sdbus::Result<sdbus::ObjectPath> && result, std::string text, std::string font)
{
	std::thread([this, result = std::move(result), text = std::move(text), font = std::move(font)]() {
		try
		{
			std::string id = create_id();

			auto bling = std::make_unique<Bling>(
			        connection_, id, text, font == "" ? "/usr/share/fonts/TTF/Hack-Regular.ttf" : font);

			auto [it, inserted] = [this, &id, &bling]() {
				std::scoped_lock<std::mutex> _(blings_lock_);
				return blings_.emplace(id, std::move(bling));
			}();

			assert(inserted);

			it->second->id = id;

			fmt::print("Created bling from text ({})\n", text);
			result.returnResults(id);
		}
		catch (std::exception & e)
		{
			result.returnError({"org.meumeu.bling.CreateTextError", e.what()});
		}
	}).detach();
}

void BlingDaemon::Show(const sdbus::ObjectPath & id, const double & duration, const int32_t & zorder)
{
	try
	{
		std::scoped_lock<std::mutex> _(blings_lock_);

		Bling & b = *blings_.at(id);
		b.zorder = zorder;
		b.duration = std::chrono::milliseconds{(int)(duration / 1000.0)};
		b.start_time = std::chrono::steady_clock::now();
		b.visible = true;
	}
	catch (std::out_of_range & e)
	{
		throw sdbus::Error("org.meumeu.bling.ShowError", e.what());
	}

	io_.post([this]() { update(); });
}

void BlingDaemon::Destroy(const sdbus::ObjectPath & id)
{
	std::scoped_lock<std::mutex> _(blings_lock_);

	blings_.erase(id);

	io_.post([this]() { update(); });
}

void BlingDaemon::start_main_loop()
{
	io_.post([this]() { update(); });
}

using namespace std::chrono_literals;

void BlingDaemon::update()
{
	bool any_bling_visible = false;
	auto now = std::chrono::steady_clock::now();

	// Display all blings
	framebuffer.assign(Leds::leds_position().size(), 0);

	// TODO: sort by z order
	{
		std::scoped_lock<std::mutex> _(blings_lock_);
		for (const auto & i: blings_)
		{
			if (!i.second->visible)
				continue;

			auto t = i.second->start_time - now;
			if (t > i.second->duration)
			{
				// TODO: destroy it
				continue;
			}

			any_bling_visible = true;

			std::vector<pixel> buffer = i.second->render(t);

			size_t j = 0;
			for (pixel p: buffer)
			{
				framebuffer[j] = std::min(255, framebuffer[j] * (255 - p.alpha) / 255 + p.grey);
				++j;
			}
		}
	}

	rogcore.AnimatrixWrite(framebuffer);

	if (any_bling_visible)
	{
		timer_.expires_after(
		        100ms); // TODO: set a reasonable delay, or wait for rogcore to signal a frame is displayed

		timer_.async_wait([this](const boost::system::error_code & ec) {
			if (!ec)
				update(); // TODO: error handling
		});
	}
}
