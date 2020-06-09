#include "Bling.h"
#include "BlingDaemon.h"
#include "Effects.h"
#include "Image.h"
#include "Leds.h"
#include "RogcoreProxy.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <fmt/format.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
	// 	RogcoreProxy rogcore;
	//
	// 	std::vector<uint8_t> data(Leds::leds_position().size());
	//
	// 	signal(SIGINT, &sigint);
	// 	int return_value = 0;
	//
	// 	while (!quit)
	// 	{
	// 		for (int i = 1; i < argc; ++i)
	// 		{
	// 			try
	// 			{
	// 				fmt::print("{}\n", argv[i]);
	// 				data.assign(data.size(), 0);
	//
	// 				// 				Bling blg{Image(argv[i])};
	// 				Bling blg{argv[i]};
	//
	// 				// 				blg.add_effect<Translate>({-5, -5});
	// 				//
	// 				// 				blg.add_effect<Rotate>()
	// 				// 					.add_keyframe(0, 0)
	// 				// 					.add_keyframe(2, 4 * 3.1415926);
	// 				//
	// 				// 				blg.add_effect<Scale>()
	// 				// 					.add_keyframe(0, {1, 1})
	// 				// 					.add_keyframe(0.5, {0.2, 0.2})
	// 				// 					.add_keyframe(1, {1, 0.2})
	// 				// 					.add_keyframe(1.5, {0.2, 1})
	// 				// 					.add_keyframe(2, {1, 1});
	// 				//
	// 				// 				blg.add_effect<Translate>({5, 5});
	// 				//
	// 				// 				blg.add_effect<Translate>()
	// 				// 					.add_keyframe(0, {0, 0})
	// 				// 					.add_keyframe(1, {20, 0})
	// 				// 					.add_keyframe(2, {0, 0});
	//
	// 				blg.add_effect<Scale>({0.75, 0.75}).add_keyframe(1, {1, 1});
	//
	// 				// 				blg.add_effect<Translate>().add_keyframe(0, {0,
	// 				// 0}).add_keyframe(1, {0, -5});
	//
	// 				blg.add_effect<Alpha>().add_keyframe(0, 1).add_keyframe(1, 0);
	//
	// 				for (float t = 0; t < 1.5 && !quit; t += 0.01)
	// 				{
	// 					std::vector<pixel> buffer = blg.render(t);
	// 					size_t j = 0;
	// 					for (pixel p: buffer)
	// 						data[j++] = p.grey;
	//
	// 					rogcore.AnimatrixWrite(data);
	// 					usleep(10000);
	// 				}
	// 			}
	// 			catch (std::exception & e)
	// 			{
	// 				fmt::print(stderr, "{}\n", e.what());
	// 				return_value = 1;
	// 				break;
	// 			}
	// 		}
	// 	}

	// Create boost::asio context
	boost::asio::io_context io;

	// Create D-Bus connection to the session bus and requests name on it.
	std::unique_ptr<sdbus::IConnection> session_bus = sdbus::createSessionBusConnection("org.meumeu.bling");

	// Quit on SIGINT or SIGTERM
	boost::asio::signal_set signals(io, SIGINT, SIGTERM);
	signals.async_wait([&io, &session_bus](const boost::system::error_code & error, int signal_number) {
		io.stop();
		session_bus->leaveEventLoop();
	});

	BlingDaemon daemon(io, *session_bus);

	session_bus->enterEventLoopAsync();
	io.run();

	daemon.clear();

	// 	data.assign(data.size(), 0);
	// 	rogcore.AnimatrixWrite(data);
	// 	return return_value;
}
