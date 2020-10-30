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

#include "AsuslinuxProxy.h"
#include "Bling.h"
#include "dbus/OrgMeumeuBlingAdaptor.h"
#include "dbus/OrgMeumeuBlingProxy.h"
#include <boost/asio/steady_timer.hpp>
#include <mutex>

namespace boost::asio
{
class io_context;
}

class BlingDaemonProxy : public sdbus::ProxyInterfaces<org::meumeu::blingdaemon_proxy>
{
public:
	BlingDaemonProxy(sdbus::IConnection & connection) :
	        sdbus::ProxyInterfaces<org::meumeu::blingdaemon_proxy>(connection, "org.meumeu.bling",
	                                                               "/org/meumeu/blingdaemon")
	{
		registerProxy();
	}

	~BlingDaemonProxy()
	{
		unregisterProxy();
	}
};

class BlingDaemon : public sdbus::AdaptorInterfaces<org::meumeu::blingdaemon_adaptor>
{
	boost::asio::io_context & io_;
	boost::asio::steady_timer timer_;
	AsuslinuxProxy asuslinux;

	std::vector<uint8_t> framebuffer;

	std::vector<std::unique_ptr<Bling>> blings_;
	std::mutex blings_lock_;

public:
	BlingDaemon(boost::asio::io_context & io, sdbus::IConnection & connection) :
	        sdbus::AdaptorInterfaces<org::meumeu::blingdaemon_adaptor>(connection, "/org/meumeu/blingdaemon"),
	        io_(io),
	        timer_(io_)
	{
		registerAdaptor();
		start_main_loop();
	}

	~BlingDaemon()
	{
		unregisterAdaptor();
	}

	void clear();

private:
	void start_main_loop();
	void update();

protected:
	void CreateImage(sdbus::Result<sdbus::ObjectPath> && result, std::string image) override;
	void CreateText(sdbus::Result<sdbus::ObjectPath> && result, std::string text, std::string font) override;
	void Show(const sdbus::ObjectPath & id, const double & duration, const int32_t & zorder) override;
	void Destroy(const sdbus::ObjectPath & id) override;
};
