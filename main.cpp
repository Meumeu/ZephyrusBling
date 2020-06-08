#include "Leds.h"
#include "RogcoreProxy.h"
#include "image.h"
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/string_cast.hpp>
#include <signal.h>
#include <unistd.h>

volatile bool quit = false;

void sigint(int)
{
	quit = true;
}

pixel sample_pixel(const image & img, glm::mat3 transform, glm::vec2 pos)
{
	// assume the color data is premultiplied
	int sum_grey = 0;
	int sum_alpha = 0;
	int count = 0;

	int steps = 4;

	glm::vec2 x0 = pos + glm::vec2(transform * glm::vec3(0, -0.5, 0));
	glm::vec2 du = glm::vec2(transform * glm::vec3(-0.5 / steps, 0.5 / steps, 0));
	glm::vec2 dv = glm::vec2(transform * glm::vec3(+0.5 / steps, 0.5 / steps, 0));

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
	return pixel{grey, alpha};
}

int main(int argc, char ** argv)
{
	RogcoreProxy rogcore;

	auto leds = Leds::leds_position();

	std::vector<uint8_t> data(leds.size());

	// leds size in cm
	double physical_height = Leds::scale_y * 55;
	double physical_width = Leds::scale_x * 33;

	signal(SIGINT, &sigint);
	int return_value = 0;

	fmt::print("LEDs: {} cm x {} cm\n", physical_width, physical_height);

	while (!quit)
	{
		for (int i = 1; i < argc; ++i)
		{
			try
			{
				fmt::print("{}\n", argv[i]);

				image img(argv[i]);

				data.assign(data.size(), 0);

				// scale image to LEDs position
				double scale = std::min(physical_width / img.w, physical_height / img.h);

				glm::mat3 transform =
				        glm::scale(glm::mat3{1}, {scale / Leds::scale_x, scale / Leds::scale_y});

				// Get the width in LEDs
				int img_width_in_led = img.w * scale / Leds::scale_x;

				// align to the right
				transform = glm::translate(glm::mat3{1}, {32 - img_width_in_led, 0}) * transform;

				// transform from LED coordinates to image coordinates
				transform = glm::inverse(transform);

				for (size_t j = 0; j < leds.size(); ++j)
				{
					if (!leds[j].visible)
						continue;

					glm::vec3 pos = transform * glm::vec3(leds[j].position, 1.0);

					pixel p = sample_pixel(img, transform, pos);

					data[j] = p.grey * p.alpha / 255;
				}

				rogcore.AnimatrixWrite(data);
				usleep(1000000);
			}
			catch (std::exception & e)
			{
				fmt::print(stderr, "{}\n", e.what());
				return_value = 1;
				break;
			}
		}
	}

	data.assign(leds.size(), 0);
	rogcore.AnimatrixWrite(data);
	return return_value;
}
