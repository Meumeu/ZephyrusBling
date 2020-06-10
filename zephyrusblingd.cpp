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
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

int main(int argc, char ** argv)
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
}
