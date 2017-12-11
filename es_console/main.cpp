#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <memory>
#include <cstdio>
#include <cassert>

#define ES_EXPECT_SDL_ZERO(op) if(auto const reported_value = op) \
	printf("Fail | " #op " -> %d, error='%s'\n", reported_value, SDL_GetError() ); \
else  \
	printf("Success | " #op "\n")

#define ES_EXPECT_SDL_PTR(op) [&] { if(auto const ptr = op) return ptr; else { printf("Fail | " #op " -> %s\n", SDL_GetError()); assert(false && #op); return decltype(ptr){}; } }()\
	

namespace es
{

constexpr static auto default_screen_width = int{ 1280 };
constexpr static auto default_screen_height = int{ 720 };

// Keep this fixed for simplicity.
constexpr static auto default_aspect_ratio = float{ 16 } / 9;

template <typename T, typename D>
auto make_scoped(T* t, D d)
{
	assert(t);
	return std::unique_ptr<T, D>(t, std::move(d));
}


class sdl
{
	struct destroy_window_t { void operator()(SDL_Window* ptr) { if(ptr) SDL_DestroyWindow(ptr); } };

	struct free_surface_t { void operator()(SDL_Surface* ptr) { if (ptr) SDL_FreeSurface(ptr); } };

	using window_t = std::unique_ptr<SDL_Window, destroy_window_t>;

	using surface_t = std::unique_ptr<SDL_Surface, free_surface_t>;

public:
	window_t m_window;

	sdl()
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

	void blit_surface(char const* asset) const
	{
		auto image = surface_t{ ES_EXPECT_SDL_PTR( SDL_LoadBMP(asset) ) };

		auto optimized_image = surface_t{ ES_EXPECT_SDL_PTR(SDL_ConvertSurfaceFormat(image.get(), SDL_PIXELFORMAT_RGBA32, 0)) };

		auto const ws = get_window_size();

		SDL_Rect rect{};
		rect.w = ws.first;
		rect.h = ws.second;

		ES_EXPECT_SDL_ZERO(SDL_BlitScaled(optimized_image.get(), nullptr, ES_EXPECT_SDL_PTR( get_window_surface() ), &rect));

		SDL_UpdateWindowSurface(m_window.get());
	}

	~sdl()
	{
		SDL_Quit();
	}
};

}

int main(int argc, char *argv[])
{
	es::sdl sdl;
	sdl.blit_surface("assets/rect2985.bmp");
	printf("JOBS DONE\n");
	getchar();
	return 0;
}
