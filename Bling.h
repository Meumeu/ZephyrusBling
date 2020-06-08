#pragma once

#include "Image.h"
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
			assert(keyframes_[i - 1].t < t);

			if (keyframes_[i].t < t)
				continue;

			double lambda = (t - keyframes_[i - 1].t) / (keyframes_[i].t - keyframes_[i - 1].t);

			return interpolate(lambda, keyframes_[i - 1].value, keyframes_[i].value);
		}

		return keyframes_.back().value;
	}

public:
	BlingFx & add_keyframe(float t, parameter_type kf)
	{
		keyframes_.emplace_back(KeyFrame<parameter_type>{t, kf});
		return *this;
	}

	~BlingFx() {}
};

class Bling
{
	std::vector<std::unique_ptr<BlingFxBase>> effects_;

	Image image_;

public:
	explicit Bling(Image && image);

	template <typename T>
	T & add_effect()
	{
		effects_.emplace_back(std::make_unique<T>());
		return static_cast<T &>(*effects_.back());
	}

	template <typename T>
	T & add_effect(typename T::parameter_type value)
	{
		effects_.emplace_back(std::make_unique<T>());
		static_cast<T &>(*effects_.back()).add_keyframe(0, value);

		return static_cast<T &>(*effects_.back());
	}

	std::vector<pixel> render(double t);
};
