#pragma once

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include "asset_map.h"
#include <memory>
#include <cassert>
#include <unordered_map>
#include <chrono>

#define ES_EXPECT_SDL_ZERO(op) if(auto const reported_value = op) \
	printf("Fail | " #op " -> %d, error='%s'\n", reported_value, SDL_GetError() ); \

#define ES_EXPECT_SDL_PTR(op) [&] { if(auto const ptr = op) return ptr; else { printf("Fail | " #op " -> %s\n", SDL_GetError()); assert(false && #op); return decltype(ptr){}; } }()\


namespace es { 

constexpr static auto default_screen_width = int{ 1280 };
constexpr static auto default_screen_height = int{ 720 };

// Keep this fixed for simplicity.
constexpr static auto default_aspect_ratio = float{ 16 } / 9;

namespace sdl {

struct destroy_window_t { void operator()(SDL_Window* ptr) { if(ptr) SDL_DestroyWindow(ptr); } };

struct free_surface_t { void operator()(SDL_Surface* ptr) { if (ptr) SDL_FreeSurface(ptr); } };

struct destroy_renderer_t { void operator()(SDL_Renderer* ptr) { if (ptr) SDL_DestroyRenderer(ptr); } };

struct destroy_texture_t { void operator()(SDL_Texture* ptr) { if (ptr) SDL_DestroyTexture(ptr); } };

using window_t = std::unique_ptr<SDL_Window, destroy_window_t>;

using surface_t = std::unique_ptr<SDL_Surface, free_surface_t>;

using renderer_t = std::unique_ptr<SDL_Renderer, destroy_renderer_t>;

using texture_t = std::unique_ptr<SDL_Texture, destroy_texture_t>;

using texture_map_t = std::unordered_map<std::uint32_t, texture_t>;

auto create_bmp(char const* filename)
{
	auto image = surface_t{ ES_EXPECT_SDL_PTR( SDL_LoadBMP(filename) ) };

	return surface_t{ ES_EXPECT_SDL_PTR(SDL_ConvertSurfaceFormat(image.get(), SDL_PIXELFORMAT_RGBA32, 0)) };
}

struct vecf { float x; float y; };

struct veci { int x; int y; };

struct moveable_object
{
	vecf position;		// blocks per frame
	vecf velocity;
	vecf acceleration;
	vecf size { 1.0, 1.0 };
};

class sprite : public moveable_object
{
public:
	int32_t visual;
	int32_t physical;
};

class context
{
	friend struct scoped_render;

public:
	context()
	{
		ES_EXPECT_SDL_ZERO(SDL_Init(SDL_INIT_EVERYTHING));

		SDL_Window* win;
		SDL_Renderer* rend;

		ES_EXPECT_SDL_ZERO(SDL_CreateWindowAndRenderer(es::default_screen_width, es::default_screen_height, SDL_WINDOW_SHOWN
			, &win, &rend));
		m_window = window_t{ win };
		m_renderer = renderer_t{ rend };
	}

	auto get_window_surface() const { return SDL_GetWindowSurface(m_window.get()); }

	auto get_window_size() const
	{
		int x, y;
		SDL_GetWindowSize(m_window.get(), &x, &y);
		return std::make_pair(x, y);
	}

	void blit_surface(SDL_Surface* s, SDL_Rect src, SDL_Rect dest) const
	{
		ES_EXPECT_SDL_ZERO(SDL_BlitScaled(s, &src, ES_EXPECT_SDL_PTR(get_window_surface()), &dest));
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

	void clear()
	{
		SDL_RenderClear(m_renderer.get());
	}

	void begin_render() { ES_EXPECT_SDL_ZERO(SDL_RenderClear(m_renderer.get())); }

	void end_render() 
	{ 
		SDL_RenderPresent(m_renderer.get()); 
		m_last_update = std::chrono::steady_clock::now();
	}

	void render_copy(SDL_Texture& t, SDL_Rect* src = nullptr, SDL_Rect* dest = nullptr)
	{
		ES_EXPECT_SDL_ZERO(SDL_RenderCopy(m_renderer.get(), &t, src, dest));
	}

	~context()
	{
		SDL_Quit();
	}

	void add_texture(std::uint32_t id, surface_t const& surface)
	{
		texture_t t{ ES_EXPECT_SDL_PTR(SDL_CreateTextureFromSurface(m_renderer.get(), surface.get())) };
		m_texture_map.emplace(id, std::move(t));
	}

	SDL_Texture& texture_at(std::uint32_t id) { return *m_texture_map.at(id); }

	auto const& last_update() const noexcept { return m_last_update; }

	auto& wait_buffer() noexcept { return m_wait_buffer; }

private:
	window_t m_window;

	renderer_t m_renderer;

	texture_map_t m_texture_map;

	std::chrono::steady_clock::time_point m_last_update;

	std::chrono::microseconds m_wait_buffer{ 1500 };
};

}}
