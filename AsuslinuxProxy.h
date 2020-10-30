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

	static constexpr uint8_t header1[] = {0x5e, 0xc0, 0x02, 0x01, 0x00, 0x73, 0x02};
	static constexpr uint8_t header2[] = {0x5e, 0xc0, 0x02, 0x74, 0x02, 0x73, 0x02};

public:
	AsuslinuxProxy() : sdbus::ProxyInterfaces<org::asuslinux::Daemon_proxy>(destination, objectPath)
	{
		registerProxy();
	}

	~AsuslinuxProxy()
	{
		unregisterProxy();
	}

	using org::asuslinux::Daemon_proxy::SetAnime;

	void AnimatrixWrite(gsl::span<uint8_t> data)
	{
		assert(data.size() == 1245);
		auto ptr = data.begin();

		auto data1 = data.subspan(0, 627);
		auto data2 = data.subspan(627, 618 /*1245 - 627*/);

		std::vector<uint8_t> packet1(640, 0xff);
		std::copy(std::begin(header1), std::end(header1), std::begin(packet1));
		std::copy(data1.begin(), data1.end(), &packet1[7]);

		std::vector<uint8_t> packet2(640, 0xff);
		std::copy(std::begin(header2), std::end(header2), std::begin(packet2));
		std::copy(data2.begin(), data2.end(), &packet2[7]);

		std::vector<std::vector<uint8_t>> out {packet1, packet2};
		SetAnime(out);
	}
};
