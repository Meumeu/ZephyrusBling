#pragma once

#include "Bling.h"
#include "RogcoreProxy.h"
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
	BlingDaemonProxy() :
	        sdbus::ProxyInterfaces<org::meumeu::blingdaemon_proxy>("org.meumeu.bling", "/org/meumeu/blingdaemon")
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
	sdbus::IConnection & connection_;
	RogcoreProxy rogcore;

	std::vector<uint8_t> framebuffer;

	std::unordered_map<std::string, std::unique_ptr<Bling>> blings_;
	std::mutex blings_lock_;

public:
	BlingDaemon(boost::asio::io_context & io, sdbus::IConnection & connection) :
	        sdbus::AdaptorInterfaces<org::meumeu::blingdaemon_adaptor>(connection, "/org/meumeu/blingdaemon"),
	        io_(io),
	        timer_(io_),
	        connection_(connection)
	{
		registerAdaptor();
		start_main_loop();
	}

	~BlingDaemon()
	{
		unregisterAdaptor();
	}

private:
	void start_main_loop();
	void update();

protected:
	void CreateImage(sdbus::Result<sdbus::ObjectPath> && result, std::string image) override;
	void CreateText(sdbus::Result<sdbus::ObjectPath> && result, std::string text, std::string font) override;
	void Show(const sdbus::ObjectPath & id, const double & duration, const int32_t & zorder) override;
	void Destroy(const sdbus::ObjectPath & id) override;
};
