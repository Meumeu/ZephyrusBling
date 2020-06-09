#include "BlingDbus.h"

BlingDaemonProxy::BlingDaemonProxy() :
        sdbus::ProxyInterfaces<org::meumeu::blingdaemon_proxy>("org.meumeu.bling", "/org/meumeu/blingdaemon")
{
	registerProxy();
}

BlingDaemonProxy::~BlingDaemonProxy()
{
	unregisterProxy();
}

BlingDaemonAdaptor::BlingDaemonAdaptor(sdbus::IConnection & connection) :
        sdbus::AdaptorInterfaces<org::meumeu::blingdaemon_adaptor>(connection, "/org/meumeu/blingdaemon")
{
	registerAdaptor();
}

BlingDaemonAdaptor::~BlingDaemonAdaptor()
{
	unregisterAdaptor();
}

BlingProxy::BlingProxy(std::string object_path) :
        sdbus::ProxyInterfaces<org::meumeu::bling_proxy>("org.meumeu.bling", std::move(object_path))
{
	registerProxy();
}

BlingProxy::~BlingProxy()
{
	unregisterProxy();
}

BlingAdaptor::BlingAdaptor(sdbus::IConnection & connection, std::string objectPath) :
        sdbus::AdaptorInterfaces<org::meumeu::bling_adaptor>(connection, std::move(objectPath))
{
	registerAdaptor();
}

BlingAdaptor::~BlingAdaptor()
{
	unregisterAdaptor();
}

sdbus::ObjectPath BlingDaemonAdaptor::CreateImage(const std::string & image)
{
	return "";
}

sdbus::ObjectPath BlingDaemonAdaptor::CreateText(const std::string & text, const std::string & font)
{
	return "";
}

void BlingAdaptor::AddRotate(const std::vector<sdbus::Struct<double, double>> & frames) {}

void BlingAdaptor::AddTranslate(const std::vector<sdbus::Struct<double, double, double>> & frames) {}

void BlingAdaptor::AddScale(const std::vector<sdbus::Struct<double, double, double>> & frames) {}

void BlingAdaptor::AddBrightness(const std::vector<sdbus::Struct<double, double>> & frames) {}

void BlingAdaptor::AddAlpha(const std::vector<sdbus::Struct<double, double>> & frames) {}

void BlingAdaptor::Show(const double & duration, const int32_t & zorder) {}

void BlingAdaptor::Destroy() {}
