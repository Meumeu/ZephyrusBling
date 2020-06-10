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

#include "Image.h"
#include "dbus/OrgMeumeuBlingAdaptor.h"
#include "dbus/OrgMeumeuBlingProxy.h"
#include <chrono>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class BlingFxBase
{
public:
	virtual glm::mat3 transform(float t);
	virtual double brightness(float t);
	virtual double alpha(float t);
	virtual ~BlingFxBase();
};

template <typename T>
struct KeyFrame
{
	float t;
	T value;
};

template <typename Effect, typename ParameterType>
class BlingFx : public BlingFxBase
{
public:
	using parameter_type = ParameterType;

private:
	std::vector<KeyFrame<parameter_type>> keyframes_;
	parameter_type value_;

	parameter_type interpolate(float t, parameter_type value0, parameter_type value1)
	{
		assert(t >= 0);
		assert(t <= 1);

		return value0 + t * (value1 - value0);
	}

protected:
	parameter_type get_value_at(float t)
	{
		assert(!keyframes_.empty());

		if (keyframes_[0].t >= t)
			return keyframes_[0].value;

		for (size_t i = 1, n = keyframes_.size(); i < n; ++i)
		{
			assert(keyframes_[i - 1].t <= keyframes_[i].t);
			assert(keyframes_[i - 1].t < t);

			if (keyframes_[i].t < t)
				continue;

			double lambda = (t - keyframes_[i - 1].t) / (keyframes_[i].t - keyframes_[i - 1].t);

			return interpolate(lambda, keyframes_[i - 1].value, keyframes_[i].value);
		}

		return keyframes_.back().value;
	}

public:
	template <typename... Args>
	BlingFx & add_keyframe(float t, Args &&... args)
	{
		keyframes_.emplace_back(KeyFrame<parameter_type>{t, parameter_type(std::forward<Args>(args)...)});
		return *this;
	}

	~BlingFx() {}
};

class BlingProxy : public sdbus::ProxyInterfaces<org::meumeu::bling_proxy>
{
public:
	BlingProxy(sdbus::IConnection & connection, std::string object_path) :
	        sdbus::ProxyInterfaces<org::meumeu::bling_proxy>(connection, "org.meumeu.bling", std::move(object_path))
	{
		registerProxy();
	}

	~BlingProxy()
	{
		unregisterProxy();
	}
};

class Bling : public sdbus::AdaptorInterfaces<org::meumeu::bling_adaptor>
{
	std::vector<std::unique_ptr<BlingFxBase>> effects_;

	Image image_;

public:
	std::chrono::steady_clock::time_point start_time;
	std::chrono::steady_clock::duration duration;
	int zorder;
	const std::string id;
	bool visible = false;

	Bling(sdbus::IConnection & connection, std::string object_path, Image && image);
	Bling(sdbus::IConnection & connection, std::string object_path, const std::string & text,
	      const std::string & font);
	Bling(const Bling &) = delete;
	Bling(Bling &&) = delete;
	virtual ~Bling();

	template <typename T>
	T & add_effect()
	{
		return static_cast<T &>(*effects_.emplace_back(std::make_unique<T>()));
	}

	template <typename T>
	T & add_effect(typename T::parameter_type value)
	{
		auto & fx = add_effect<T>();

		fx.add_keyframe(0, value);

		return fx;
	}

	template <typename T, typename U>
	T & add_effect(std::vector<U> keyframes)
	{
		if (keyframes.empty())
			throw std::invalid_argument("keyframes");

		auto & fx = add_effect<T>();

		std::sort(keyframes.begin(), keyframes.end());

		for (const auto & i: keyframes)
		{
			std::apply([&fx](auto... args) { fx.add_keyframe(args...); }, i);
		}

		return fx;
	}

	std::vector<pixel> render(std::chrono::steady_clock::duration t) const;

protected:
	void AddRotate(const std::vector<sdbus::Struct<double, double>> & frames) override;
	void AddTranslate(const std::vector<sdbus::Struct<double, double, double>> & frames) override;
	void AddScale(const std::vector<sdbus::Struct<double, double, double>> & frames) override;
	void AddBrightness(const std::vector<sdbus::Struct<double, double>> & frames) override;
	void AddAlpha(const std::vector<sdbus::Struct<double, double>> & frames) override;
};
