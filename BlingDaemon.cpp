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

#include "Bling.h"
#include "Leds.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <fmt/format.h>
#include <thread>

using namespace std::chrono_literals;

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

			auto bling = std::make_unique<Bling>(getObject().getConnection(), id, Image{image});

			{
				std::scoped_lock<std::mutex> _(blings_lock_);
				assert(bling->id == id);
				blings_.emplace_back(std::move(bling));
			}

			result.returnResults(id);
		}
		catch (std::exception & e)
		{
			result.returnError(sdbus::Error{"org.meumeu.bling.CreateImageError", e.what()});
		}
	}).detach();
}

void BlingDaemon::CreateText(sdbus::Result<sdbus::ObjectPath> && result, std::string text, std::string font)
{
	std::thread([this, result = std::move(result), text = std::move(text), font = std::move(font)]() {
		try
		{
			std::string id = create_id();

			auto bling =
			        std::make_unique<Bling>(getObject().getConnection(), id, text,
			                                font == "" ? "/usr/share/fonts/TTF/Hack-Regular.ttf" : font);

			{
				std::scoped_lock<std::mutex> _(blings_lock_);
				assert(bling->id == id);
				blings_.emplace_back(std::move(bling));
			}

			result.returnResults(id);
		}
		catch (std::exception & e)
		{
			result.returnError(sdbus::Error{"org.meumeu.bling.CreateTextError", e.what()});
		}
	}).detach();
}

void BlingDaemon::Show(const sdbus::ObjectPath & id, const double & duration, const int32_t & zorder)
{
	std::scoped_lock<std::mutex> _(blings_lock_);

	for (auto & i: blings_)
	{
		if (i->id == id)
		{
			i->zorder = zorder;
			i->duration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
			        std::chrono::duration<double>(duration));
			i->start_time = std::chrono::steady_clock::now();
			i->visible = true;

			std::stable_sort(blings_.begin(), blings_.end(),
			                 [](const auto & a, const auto & b) { return a->zorder < b->zorder; });

			io_.post([this]() { update(); });

			return;
		}
	}

	throw sdbus::Error("org.meumeu.bling.ShowError", "Bling " + id + " not found");
}

void BlingDaemon::Destroy(const sdbus::ObjectPath & id)
{
	std::scoped_lock<std::mutex> _(blings_lock_);

	auto it = std::remove_if(blings_.begin(), blings_.end(), [&id](const auto & bling) { return id == bling->id; });

	if (it == blings_.end())
		throw sdbus::Error("org.meumeu.bling.DestroyError", "Bling " + id + " not found");

	blings_.erase(it, blings_.end());

	io_.post([this]() { update(); });
}

void BlingDaemon::start_main_loop()
{
	io_.post([this]() { update(); });
}

void BlingDaemon::clear()
{
	framebuffer.assign(Leds::leds_position().size(), 0);
	asuslinux.AnimatrixWrite(framebuffer);
}

void BlingDaemon::update()
{
	bool any_bling_visible = false;
	auto now = std::chrono::steady_clock::now();

	// Display all blings
	framebuffer.assign(Leds::leds_position().size(), 0);

	{
		std::scoped_lock<std::mutex> _(blings_lock_);

		for (const auto & i: blings_)
		{
			if (!i->visible)
				continue;

			auto t = now - i->start_time;
			if (t > i->duration)
			{
				continue;
			}

			any_bling_visible = true;

			std::vector<pixel> buffer = i->render(t);

			size_t j = 0;
			for (pixel p: buffer)
			{
				framebuffer[j] = std::min(255, framebuffer[j] * (255 - p.alpha) / 255 + p.grey);
				++j;
			}
		}

		auto it = std::remove_if(blings_.begin(), blings_.end(), [now](const auto & i) {
			return i->visible && now > i->start_time + i->duration;
		});
		blings_.erase(it, blings_.end());
	}

	asuslinux.AnimatrixWrite(framebuffer);

	if (any_bling_visible)
	{
		// TODO: set a reasonable delay, or wait for asusd to signal a frame is displayed
		timer_.expires_after(20ms);

		timer_.async_wait([this](const boost::system::error_code & ec) {
			if (!ec)
				update(); // TODO: error handling
		});
	}
}
