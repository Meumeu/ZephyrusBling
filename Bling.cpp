#include "Bling.h"

#include "Effects.h"
#include "Leds.h"
#include "print.h"
#include <boost/process.hpp>
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

Bling::Bling(sdbus::IConnection & connection, std::string object_path, Image && image) :
        sdbus::AdaptorInterfaces<org::meumeu::bling_adaptor>(connection, std::move(object_path)),
        image_(std::move(image))
{
	registerAdaptor();
}

Bling::Bling(sdbus::IConnection & connection, std::string object_path, const std::string & text,
             const std::string & font) :
        sdbus::AdaptorInterfaces<org::meumeu::bling_adaptor>(connection, std::move(object_path))
{
	namespace bp = boost::process;
	using bp::child;
	using bp::ipstream;
	using bp::search_path;
	using bp::std_err;
	using bp::std_out;

	ipstream is;
	ipstream err;

	child imagemagick(search_path("convert"), "-background", "transparent", "-fill", "white", "-font", font,
	                  "-pointsize", "30", "label:" + text, "png:", std_out > is, std_err > err);

	std::vector<uint8_t> pngdata{std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()};
	std::string error{std::istreambuf_iterator<char>(err), std::istreambuf_iterator<char>()};
	imagemagick.join();

	if (error != "")
	{
		throw std::runtime_error(fmt::format("Error rendering text: {}", error));
	}

	image_ = Image{pngdata};

	registerAdaptor();
}

Bling::~Bling()
{
	unregisterAdaptor();
}

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
double physical_height = []() {
	float max_y = std::numeric_limits<float>::min();
	float min_y = std::numeric_limits<float>::max();
	for (const auto & i: Leds::leds_position())
	{
		min_y = std::min(min_y, i.position.y);
		max_y = std::max(max_y, i.position.y);
	}
	return Leds::scale_y * (max_y - min_y + 1);
}();

double physical_width = []() {
	float max_x = std::numeric_limits<float>::min();
	float min_x = std::numeric_limits<float>::max();
	for (const auto & i: Leds::leds_position())
	{
		min_x = std::min(min_x, i.position.x);
		max_x = std::max(max_x, i.position.x);
	}
	return Leds::scale_x * (max_x - min_x + 1);
}();

std::vector<pixel> Bling::render(std::chrono::steady_clock::duration dur) const
{
	double t = std::chrono::duration<double>(dur).count();

	double brightness = 1;
	double alpha = 1;

	// center image on (0,0)
	glm::mat3 center_image = glm::translate(glm::mat3{1}, {-0.5 * image_.w, -0.5 * image_.h});

	// scale image pixel to centimeters
	double scale = std::min(physical_width / image_.w, physical_height / image_.h);
	glm::mat3 cm_from_px = glm::scale(glm::mat3{1}, {scale, scale});

	glm::mat3 bling_transform_cm{1};
	for (auto & i: effects_)
	{
		bling_transform_cm = i->transform(t) * bling_transform_cm;
		brightness *= i->brightness(t);
		alpha *= i->alpha(t);
	}

	// centimeters to LED
	glm::mat3 led_from_cm = glm::scale(glm::mat3{1}, {1 / Leds::scale_x, 1 / Leds::scale_y});

	glm::mat3 position_image_in_leds = glm::translate(glm::mat3{1}, {20, 20});

	// combine all the transforms to get the transform from LED coordinates to image coordinates
	glm::mat3 led_from_px = position_image_in_leds * led_from_cm * bling_transform_cm * cm_from_px * center_image;

	// fmt::print("image size: {}x{}\n", image_.w, image_.h);
	// fmt::print("led_from_px: {}\n", led_from_px);
	// fmt::print("topleft: {}, btmright: {}\n", led_from_px * glm::vec3{0,0,1}, led_from_px *
	// glm::vec3{image_.w-1,image_.h-1,1});

	// use the inverse transform to sample the pixels
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

namespace std
{
template <typename... Elements>
struct tuple_size<::sdbus::Struct<Elements...>> : public integral_constant<std::size_t, sizeof...(Elements)>
{};
} // namespace std

static_assert(std::tuple_size_v<const sdbus::Struct<double, double>> == 2);

void Bling::AddRotate(const std::vector<sdbus::Struct<double, double>> & frames)
{
	auto & fx = add_effect<Rotate>(frames);
}

void Bling::AddTranslate(const std::vector<sdbus::Struct<double, double, double>> & frames)
{
	auto & fx = add_effect<Translate>(frames);
}

void Bling::AddScale(const std::vector<sdbus::Struct<double, double, double>> & frames)
{
	auto & fx = add_effect<Scale>(frames);
}

void Bling::AddBrightness(const std::vector<sdbus::Struct<double, double>> & frames)
{
	auto & fx = add_effect<Brightness>(frames);
}

void Bling::AddAlpha(const std::vector<sdbus::Struct<double, double>> & frames)
{
	auto & fx = add_effect<Alpha>(frames);
}
