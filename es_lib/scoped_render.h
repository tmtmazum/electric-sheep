#pragma once

#include "sdl.h"
#include <chrono>
#include <thread>

namespace es
{
namespace sdl
{

struct scoped_render
{
	scoped_render(context& c, veci num_blocks = { 0, 0 })
		: m_context{ c }
		, m_num_blocks{num_blocks}
	{
		m_context.begin_render();
	}

	void add(SDL_Texture& t)
	{
		m_context.render_copy(t);
	}

	void add(SDL_Texture& t, SDL_Rect src, SDL_Rect dest)
	{
		m_context.render_copy(t, &src, &dest);
	}

	void add_from(SDL_Texture& t, SDL_Rect src)
	{
		m_context.render_copy(t, &src, nullptr);
	}

	void add_to(SDL_Texture& t, SDL_Rect dest)
	{
		m_context.render_copy(t, nullptr, &dest);
	}

	void add_to_background(SDL_Texture& t)
	{
		auto const ws = m_context.get_window_size();
		add_to(t, SDL_Rect{ 0, 0, ws.first, ws.second });
	}

	template <int NumH, int NumW>
	void add(int64_t const (&map) [NumH][NumW])
	{
		auto const ws = m_context.get_window_size();
		
		auto unit = std::make_pair(ws.first / NumW, ws.second / NumH);

		for (int y = 0; y < NumH; y++)
		{
			for (int x = 0; x < NumW; x++)
			{
				if (auto const visual = static_cast<uint32_t>(map[y][x] >> 32))
				{
					add_to(m_context.texture_at(visual), SDL_Rect{ x*unit.first, y*unit.second, unit.first, unit.second });
				}
			}
		}
	}

	void add(sprite const& s)
	{
		assert(m_num_blocks.x && m_num_blocks.y);

		auto const ws = m_context.get_window_size();
		auto unit = std::make_pair(ws.first / m_num_blocks.x, ws.second / m_num_blocks.y);

		add_to(m_context.texture_at(s.visual), SDL_Rect{ 
			static_cast<int>(s.position.x * unit.first), static_cast<int>(s.position.y * unit.second), 
			static_cast<int>(s.size.x * unit.first), static_cast<int>(s.size.y * unit.second) 
		});
	}

	void set_num_blocks(veci v) { m_num_blocks = v; }

	void set_frames_per_second(uint32_t f) { m_frames_per_second = f; }

	void wait_for_frame()
	{
		auto const time_between_frames = std::chrono::microseconds(static_cast<int>(1000000.0f / m_frames_per_second));
		m_last_update = m_context.last_update();
		auto const next_update_due = m_context.last_update() + time_between_frames;
		if (clock_t::now() < next_update_due)
		{
			std::this_thread::sleep_until(next_update_due - m_context.wait_buffer());
		}
	}

	~scoped_render()
	{
		using namespace std::chrono;

		wait_for_frame();
		m_context.end_render();

		auto const time_between_frames = microseconds(static_cast<int>(1000000.0f / m_frames_per_second));
		auto const now = m_context.last_update();
		bool const lag = now > (m_last_update + time_between_frames);
			
		auto const diff = static_cast<float>(duration_cast<microseconds>(now - (m_last_update + time_between_frames)).count());

		// lagging behind
		auto const time_elapsed = duration_cast<microseconds>(now - m_last_update);
		auto const expected_fps = 1000000.0f / time_between_frames.count();
		auto const actual_fps = 1000000.0f / static_cast<uint32_t>(time_elapsed.count());

		auto const diff_fps = std::abs(actual_fps - expected_fps);
		if (diff_fps > 5.0)
		{
			printf("%.2f %c %.2f fps (%.2f fps)\n", expected_fps, diff > 0.0 ? '-' : '+', diff_fps, actual_fps);
		}
	}

private:
	context& m_context;

	veci	m_num_blocks;

	uint32_t m_frames_per_second = 60;

	uint32_t m_actual_frame_rate;

	using clock_t = std::chrono::steady_clock;

	std::chrono::steady_clock::time_point m_last_update;

	std::chrono::nanoseconds m_render_duration{ 0 };
};


}
}