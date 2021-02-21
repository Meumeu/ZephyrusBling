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

#include "dbus/OrgAsuslinuxDaemonProxy.h"
#include <gsl/span>

class AsuslinuxProxy : public sdbus::ProxyInterfaces<org::asuslinux::Daemon_proxy>
{
	static constexpr char destination[] = "org.asuslinux.Daemon";
	static constexpr char objectPath[] = "/org/asuslinux/Anime";

public:
	AsuslinuxProxy() : sdbus::ProxyInterfaces<org::asuslinux::Daemon_proxy>(destination, objectPath)
	{
		registerProxy();
	}

	~AsuslinuxProxy()
	{
		unregisterProxy();
	}

	using org::asuslinux::Daemon_proxy::WriteDirect;

	void AnimatrixWrite(gsl::span<uint8_t> data)
	{
		assert(data.size() == 1245);

		std::vector<uint8_t> packet(1254, 0x00);
		std::copy(data.begin(), data.end(), packet.begin());
		WriteDirect(packet);
	}
};
