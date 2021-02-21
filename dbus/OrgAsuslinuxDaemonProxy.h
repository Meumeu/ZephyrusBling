
/*
 * This file was automatically generated by sdbus-c++-xml2cpp; DO NOT EDIT!
 */

#ifndef __sdbuscpp__dbus_OrgAsuslinuxDaemonProxy_h__proxy__H__
#define __sdbuscpp__dbus_OrgAsuslinuxDaemonProxy_h__proxy__H__

#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <tuple>

namespace org
{
namespace asuslinux
{
class Daemon_proxy
{
public:
	static constexpr const char * INTERFACE_NAME = "org.asuslinux.Daemon";

protected:
	Daemon_proxy(sdbus::IProxy & proxy) : proxy_(proxy) {}

	~Daemon_proxy() = default;

public:
	void WriteImage(const std::vector<std::vector<uint8_t>> & input)
	{
		proxy_.callMethod("WriteImage").onInterface(INTERFACE_NAME).withArguments(input).dontExpectReply();
	}

	void WriteDirect(const std::vector<uint8_t> & input)
	{
		proxy_.callMethod("WriteDirect").onInterface(INTERFACE_NAME).withArguments(input).dontExpectReply();
	}

	void SetOnOff(const bool & status)
	{
		proxy_.callMethod("SetOnOff").onInterface(INTERFACE_NAME).withArguments(status).dontExpectReply();
	}

	void SetBootOnOff(const bool & status)
	{
		proxy_.callMethod("SetBootOnOff").onInterface(INTERFACE_NAME).withArguments(status).dontExpectReply();
	}

private:
	sdbus::IProxy & proxy_;
};

} // namespace asuslinux
} // namespace org

#endif
