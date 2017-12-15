#include "es_lib/sdl.h"
#include "es_lib/asset_map.h"
#include "es_lib/scoped_render.h"
#include <memory>
#include <cstdio>
#include <cassert>
#include <functional>
#include <chrono>
#include <thread>

namespace es
{

template <typename T, typename D>
auto make_scoped(T* t, D d)
{
	assert(t);
	return std::unique_ptr<T, D>(t, std::move(d));
}

auto B(int32_t visual_value, int32_t physical_value)
{
	return (static_cast<int64_t>(visual_value) << 32) | physical_value;
}

int64_t map[18][32] = {
	B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(1,1), B(1,1), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(1,1), B(1,1), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(1,1), B(1,1), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(1,1), B(1,1), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(1,1), B(1,1), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(1,1), B(1,1), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(1,1), B(1,1), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(1,1), B(1,1), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0),
	B(1,1), B(1,1), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(1,1), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0), B(0,0)
};

}

enum state
{
	stationary,
	free_fall,

};

// -- TO DO
using transition_map = std::unordered_map<state, std::function<state(es::sdl::sprite const&)>>;

static auto& g_transiton_map()
{
	static transition_map g_map;
	return g_map;
}
// ---

enum class key_action
{
	move_left,
	move_right,
	move_up,
	move_down,
};

using keyboard_map = std::unordered_map<uint64_t, key_action>;

int main(int argc, char *argv[]) try
{
	using namespace es::sdl;

	es::sdl::context sdl;
	sdl.add_texture(0, es::sdl::create_bmp("assets/rect2985.bmp"));
	sdl.add_texture(1, es::sdl::create_bmp("assets/visual-1.bmp"));
	sdl.add_texture(20, es::sdl::create_bmp("assets/rect-player.bmp"));

	//sdl.print(es::map);

	es::sdl::sprite spr{};
	spr.visual = 20;
	spr.physical = 1;
	spr.size = es::sdl::vecf{ 1.0, 2.0 };
	//sdl.print(spr, es::map);

	keyboard_map kmap
	{
		{SDL_SCANCODE_W, key_action::move_up},
		{SDL_SCANCODE_S, key_action::move_down},
		{SDL_SCANCODE_A, key_action::move_left},
		{SDL_SCANCODE_D, key_action::move_right}
	};

	bool quit = false;
	while (!quit)
	{
		//SDL_Event evt;
		//while (SDL_PollEvent(&evt))
		//{
		//	if (evt.type == SDL_QUIT)
		//	{
		//		quit = true;
		//	}
		//}

		SDL_PumpEvents();

		scoped_render render{ sdl, {32, 18} };

		auto keystate = SDL_GetKeyboardState(nullptr);
		for (auto const& k : kmap)
		{
			if (keystate[k.first])
			{
				//printf("first: %u -> %u\n", k.first, keystate[k.first]);
				auto action = k.second;
				constexpr auto speed = 0.1;
				if (action == key_action::move_up)
				{
					spr.position.y -= speed;
				}
				if (action == key_action::move_down)
				{
					spr.position.y += speed;
				}
				if (action == key_action::move_left)
				{
					spr.position.x -= speed;
				}
				if (action == key_action::move_right)
				{
					spr.position.x += speed;
					//printf("moving right: {%f, %f}\n", spr.position.x, spr.position.y);
				}
			}
		}

		render.add_to_background(sdl.texture_at(0));
		render.add(es::map);
		render.add(spr);

	}

	printf("JOBS DONE\n");
	return 0;
}
catch (std::exception const& e)
{
	printf("Caught exception: %s\n", e.what());
	return 1;
}
catch (...)
{
	printf("Unknown exception caught.");
	return 1;
}
