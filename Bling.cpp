#include "Bling.h"

#include "Leds.h"
#include <glm/gtx/matrix_transform_2d.hpp>

BlingFxBase::~BlingFxBase() {}

double BlingFxBase::brightness(float)
{
	return 1;
}

double BlingFxBase::alpha(float)
{
	return 1;
}

glm::mat3 BlingFxBase::transform(float)
{
	return glm::mat3(1.0f);
}

Bling::Bling(Image && image) : image_(std::move(image)) {}

static const auto & leds = Leds::leds_position();

static std::vector<pixel> sample_pixels(const Image & img, glm::mat3 px_from_led)
{
	// assume the color data is premultiplied
	std::vector<pixel> data;

	int steps = 4;
	glm::vec2 du = glm::vec2(px_from_led * glm::vec3(-0.5 / steps, 0.5 / steps, 0));
	glm::vec2 dv = glm::vec2(px_from_led * glm::vec3(+0.5 / steps, 0.5 / steps, 0));

	data.resize(leds.size());
	for (size_t j = 0; j < leds.size(); ++j)
	{
		if (!leds[j].visible)
			continue;

		int sum_grey = 0;
		int sum_alpha = 0;
		int count = 0;

		glm::vec3 pos(leds[j].position, 1);

		glm::vec2 x0 = px_from_led * (pos + glm::vec3(0, -0.5, 0));

		for (int u = 0; u < steps; ++u)
		{
			for (int v = 0; v < steps; ++v)
			{
				pixel p = img(x0 + (float)u * du + (float)v * dv);

				sum_grey += p.grey;
				sum_alpha += p.alpha;
				++count;
			}
		}

		uint8_t grey = sum_grey / count;
		uint8_t alpha = sum_alpha / count;

		data[j] = pixel{grey, alpha};
	}

	return data;
}

// leds size in cm
double physical_height = Leds::scale_y * 55;
double physical_width = Leds::scale_x * 33;

std::vector<pixel> Bling::render(double t)
{
	// 	glm::mat3 transform(1.0f);
	double brightness = 1;
	double alpha = 1;

	// scale image pixel to centimeters
	double scale = std::min(physical_width / image_.w, physical_height / image_.h);
	glm::mat3 cm_from_px = glm::scale(glm::mat3{1}, {scale, scale});

	// Image pixel to LED
	glm::mat3 led_from_cm = glm::scale(glm::mat3{1}, {1 / Leds::scale_x, 1 / Leds::scale_y});

	// 	// Get the width in number of LEDs
	// 	int img_width_in_led = image_.w * scale / Leds::scale_x;
	//
	// 	// Align to the right
	// 	transform = glm::translate(glm::mat3{1}, {32 - img_width_in_led, 0}) * transform;

	glm::mat3 bling_transform_cm{1};
	for (auto & i: effects_)
	{
		bling_transform_cm = i->transform(t) * bling_transform_cm;
		brightness *= i->brightness(t);
		alpha *= i->alpha(t);
	}

	// transform from LED coordinates to image coordinates
	glm::mat3 led_from_px = led_from_cm * bling_transform_cm * cm_from_px;

	std::vector<pixel> buffer = sample_pixels(image_, glm::inverse(led_from_px));

	brightness = std::max(brightness, 0.0);
	alpha = std::max(alpha, 0.0);

	for (pixel & i: buffer)
	{
		i.grey = std::min<int>(255, i.grey * brightness * alpha);
		i.alpha = std::min<int>(255, i.alpha * alpha);
	}

	return buffer;
}
