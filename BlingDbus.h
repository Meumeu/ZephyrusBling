#pragma once

#include "dbus/OrgMeumeuBlingAdaptor.h"
#include "dbus/OrgMeumeuBlingProxy.h"

class BlingDaemonProxy : public sdbus::ProxyInterfaces<org::meumeu::blingdaemon_proxy>
{
public:
	BlingDaemonProxy();
	~BlingDaemonProxy();
};

class BlingProxy : public sdbus::ProxyInterfaces<org::meumeu::bling_proxy>
{
public:
	BlingProxy(std::string object_path);
	~BlingProxy();
};

class BlingDaemonAdaptor : public sdbus::AdaptorInterfaces<org::meumeu::blingdaemon_adaptor>
{
public:
	BlingDaemonAdaptor(sdbus::IConnection & connection);
	~BlingDaemonAdaptor();

protected:
	sdbus::ObjectPath CreateImage(const std::string & image) override;
	sdbus::ObjectPath CreateText(const std::string & text, const std::string & font) override;
};

class BlingAdaptor : public sdbus::AdaptorInterfaces<org::meumeu::bling_adaptor>
{
public:
	BlingAdaptor(sdbus::IConnection & connection, std::string object_path);
	~BlingAdaptor();

protected:
	void AddRotate(const std::vector<sdbus::Struct<double, double>> & frames) override;
	void AddTranslate(const std::vector<sdbus::Struct<double, double, double>> & frames) override;
	void AddScale(const std::vector<sdbus::Struct<double, double, double>> & frames) override;
	void AddBrightness(const std::vector<sdbus::Struct<double, double>> & frames) override;
	void AddAlpha(const std::vector<sdbus::Struct<double, double>> & frames) override;
	void Show(const double & duration, const int32_t & zorder) override;
	void Destroy() override;
};
