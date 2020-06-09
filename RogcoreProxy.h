#pragma once

#include "dbus/OrgRogcoreDaemonProxy.h"
#include <gsl/span>

class RogcoreProxy : public sdbus::ProxyInterfaces<org::rogcore::Daemon_proxy>
{
	static constexpr char destination[] = "org.rogcore.Daemon";
	static constexpr char objectPath[] = "/org/rogcore/Daemon";

	static constexpr uint8_t header1[] = {0x5e, 0xc0, 0x02, 0x01, 0x00, 0x73, 0x02};
	static constexpr uint8_t header2[] = {0x5e, 0xc0, 0x02, 0x74, 0x02, 0x73, 0x02};

public:
	RogcoreProxy() : sdbus::ProxyInterfaces<org::rogcore::Daemon_proxy>(destination, objectPath)
	{
		registerProxy();
	}

	~RogcoreProxy()
	{
		unregisterProxy();
	}

	using org::rogcore::Daemon_proxy::AnimatrixWrite;

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

		AnimatrixWrite(packet1, packet2);
	}

protected:
	void onLedCancelEffect() override {}
};
