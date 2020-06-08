#include "Bling.h"
#include "Effects.h"
#include "Image.h"
#include "Leds.h"
#include "RogcoreProxy.h"
#include <fmt/format.h>
#include <signal.h>
#include <unistd.h>

volatile bool quit = false;

void sigint(int)
{
	quit = true;
}

// Text rendering: convert -background black -fill white -font /usr/share/fonts/TTF/Hack-Regular.ttf -pointsize 30
// label:"hello world" helloworld.png

int main(int argc, char ** argv)
{
	RogcoreProxy rogcore;

	std::vector<uint8_t> data(Leds::leds_position().size());

	signal(SIGINT, &sigint);
	int return_value = 0;

	while (!quit)
	{
		for (int i = 1; i < argc; ++i)
		{
			try
			{
				fmt::print("{}\n", argv[i]);
				data.assign(data.size(), 0);

				Bling blg{Image(argv[i])};

				// 				blg.add_effect<Translate>({-5, -5});
				//
				// 				blg.add_effect<Rotate>()
				// 					.add_keyframe(0, 0)
				// 					.add_keyframe(2, 4 * 3.1415926);
				//
				// 				blg.add_effect<Scale>()
				// 					.add_keyframe(0, {1, 1})
				// 					.add_keyframe(0.5, {0.2, 0.2})
				// 					.add_keyframe(1, {1, 0.2})
				// 					.add_keyframe(1.5, {0.2, 1})
				// 					.add_keyframe(2, {1, 1});
				//
				// 				blg.add_effect<Translate>({5, 5});
				//
				// 				blg.add_effect<Translate>()
				// 					.add_keyframe(0, {0, 0})
				// 					.add_keyframe(1, {20, 0})
				// 					.add_keyframe(2, {0, 0});

				blg.add_effect<Translate>({-8, -8});
				blg.add_effect<Scale>({0.5, 0.5});
				blg.add_effect<Translate>({20, 10});

				blg.add_effect<Translate>().add_keyframe(0, {0, 0}).add_keyframe(1, {0, -5});

				blg.add_effect<Alpha>().add_keyframe(0, 1).add_keyframe(1, 0);

				for (float t = 0; t < 1.5 && !quit; t += 0.01)
				{
					std::vector<pixel> buffer = blg.render(t);
					size_t j = 0;
					for (pixel p: buffer)
						data[j++] = p.grey;

					rogcore.AnimatrixWrite(data);
					usleep(10000);
				}
			}
			catch (std::exception & e)
			{
				fmt::print(stderr, "{}\n", e.what());
				return_value = 1;
				break;
			}
		}
	}

	data.assign(data.size(), 0);
	rogcore.AnimatrixWrite(data);
	return return_value;
}
