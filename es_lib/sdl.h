#pragma once

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include "asset_map.h"
#include <memory>
#include <cassert>

#define ES_EXPECT_SDL_ZERO(op) if(auto const reported_value = op) \
	printf("Fail | " #op " -> %d, error='%s'\n", reported_value, SDL_GetError() ); \
else  \
	printf("Success | " #op "\n")

#define ES_EXPECT_SDL_PTR(op) [&] { if(auto const ptr = op) return ptr; else { printf("Fail | " #op " -> %s\n", SDL_GetError()); assert(false && #op); return decltype(ptr){}; } }()\


namespace es { 

constexpr static auto default_screen_width = int{ 1280 };
constexpr static auto default_screen_height = int{ 720 };

// Keep this fixed for simplicity.
constexpr static auto default_aspect_ratio = float{ 16 } / 9;

namespace sdl {

struct destroy_window_t { void operator()(SDL_Window* ptr) { if(ptr) SDL_DestroyWindow(ptr); } };

struct free_surface_t { void operator()(SDL_Surface* ptr) { if (ptr) SDL_FreeSurface(ptr); } };

using window_t = std::unique_ptr<SDL_Window, destroy_window_t>;

using surface_t = std::unique_ptr<SDL_Surface, free_surface_t>;

auto create_bmp(char const* filename)
{
	auto image = surface_t{ ES_EXPECT_SDL_PTR( SDL_LoadBMP(filename) ) };

	return surface_t{ ES_EXPECT_SDL_PTR(SDL_ConvertSurfaceFormat(image.get(), SDL_PIXELFORMAT_RGBA32, 0)) };
}

class context
{
public:
	context()
	{
		ES_EXPECT_SDL_ZERO(SDL_Init(SDL_INIT_EVERYTHING));

		auto wptr = ES_EXPECT_SDL_PTR(SDL_CreateWindow("electric sheep", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			es::default_screen_width, es::default_screen_height, SDL_WINDOW_SHOWN));
		m_window = window_t{ wptr };
	}

	auto get_window_surface() const { return SDL_GetWindowSurface(m_window.get()); }

	auto get_window_size() const
	{
		int x, y;
		SDL_GetWindowSize(m_window.get(), &x, &y);
		return std::make_pair(x, y);
	}

	void blit_surface_to(SDL_Surface* s, SDL_Rect dest) const
	{
		ES_EXPECT_SDL_ZERO(SDL_BlitScaled(s, nullptr, ES_EXPECT_SDL_PTR(get_window_surface()), &dest));
	}

	void blit_surface_to_background(SDL_Surface* s) const
	{
		auto const ws = get_window_size();

		blit_surface_to(s, SDL_Rect{ 0, 0, ws.first, ws.second });
	}

	void update()
	{
		SDL_UpdateWindowSurface(m_window.get());
	}

	template <int NumH, int NumW>
	void print(int64_t const (&map) [NumH][NumW])
	{
		auto const ws = get_window_size();
		
		auto unit = std::make_pair(ws.first / NumW, ws.second / NumH);

		for (int y = 0; y < NumH; y++)
		{
			for (int x = 0; x < NumW; x++)
			{
				if (auto const visual = static_cast<uint32_t>(map[y][x] >> 32))
				{
					auto const it = g_asset_map().find(visual);
					assert(it != g_asset_map().end());
					auto surface = it->second->to_surface();
					assert(surface && "Specified surface does not exist");
					blit_surface_to(surface, SDL_Rect{ x * unit.first, y * unit.second, unit.first, unit.second });
				}
			}
		}
	}

	~context()
	{
		SDL_Quit();
	}

private:
	window_t m_window;
};


}}
