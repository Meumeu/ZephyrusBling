#include "Leds.h"

namespace
{
// 1st line: 32px => 26.2cm
// 2nd line: 33px => 26.6cm
// avg: 0,812307692308 cm / px
//
// last row: 55px => 15.6cm, 0,283636363636 cm/px
//
// pixels are 2,86390532545 wider than they are tall
//
// center of each led is given by:
// led (x, y) => (x * 0.8 - (y%2) * 0.4, y * 0.28)
//
//  row	visible	total
//  0	33	33
//  1	33	35
//  2	33	33
//  3	33	35
//  4	33	33
//  5	33	34
//  6	33	33
//  7	32	33
//  8	32	32
//  9	31	32
//  10	31	31
//  11	30	31
//  12	30	30
//  13	29	30
//  14	29	29
//  15	28	29

constexpr int width(int y)
{
	if (y < 5)
		return 33;
	else
		return 36 - (y + 1) / 2;
}
static_assert(width(5) == 33);
static_assert(width(6) == 33);
static_assert(width(7) == 32);
static_assert(width(8) == 32);
static_assert(width(9) == 31);
static_assert(width(10) == 31);
static_assert(width(11) == 30);
static_assert(width(12) == 30);
static_assert(width(13) == 29);
static_assert(width(14) == 29);

constexpr int first_x(int y)
{
	if (y < 5)
		return 0;
	else
		return (y + 1) / 2 - 3;
}

constexpr int pitch(int y)
{
	if (y == 0)
		return 33;
	else if (y == 1)
		return 35;
	else if (y == 2)
		return 33;
	else if (y == 3)
		return 35;
	else if (y == 4)
		return 33;
	else
		return 36 - y / 2;
}
static_assert(pitch(5) == 34);
static_assert(pitch(6) == 33);
static_assert(pitch(7) == 33);
static_assert(pitch(8) == 32);
static_assert(pitch(9) == 32);
static_assert(pitch(10) == 31);
static_assert(pitch(11) == 31);
static_assert(pitch(12) == 30);
static_assert(pitch(13) == 30);
static_assert(pitch(14) == 29);

static const int height = 55;
} // namespace

const std::array<Led, 1245> Leds::leds_ = []() {
	std::array<Led, 1245> leds;

	int index = 1; // first byte is not used
	for (int y = 0; y < height; ++y)
	{
		int w = width(y);
		int p = pitch(y);
		int x0 = first_x(y);

		int idx = 0;
		for (int x = x0; idx < w; ++x, ++idx)
		{
			leds[index++] = Led(x - 0.5 * (y % 2), y);
		}

		for (; idx < p; ++idx)
		{
			leds[index++] = Led();
		}
	}

	return leds;
}();
