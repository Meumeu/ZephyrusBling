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

				// translate 10 leds to the right
				transform = glm::translate(glm::mat3{1}, {10, 0}) * transform;

				// transform from LED coordinates to image coordinates
				transform = glm::inverse(transform);

				for (size_t j = 0; j < leds.size(); ++j)
				{
					if (!leds[j].visible)
						continue;

					glm::vec3 pos = transform * glm::vec3(leds[j].position, 1.0);

					pixel p = img(pos);
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
