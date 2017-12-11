#pragma once

#include <cinttypes>
#include <unordered_map>
#include <string>
#include <memory>

struct SDL_Surface;

namespace es { namespace sdl {

class asset
{
public:
	virtual SDL_Surface* to_surface() const noexcept { return nullptr; }
};

template <typename T, typename Dt>
class unique_asset : public asset
{
public:
	explicit unique_asset(std::unique_ptr<T, Dt> p)
		: m_ptr{ std::move(p) } {}

	SDL_Surface* to_surface() const noexcept override { return m_ptr.get(); }

private:
	std::unique_ptr<T, Dt> m_ptr;
};

template <typename T, typename Dt>
auto make_unique_asset(std::unique_ptr<T, Dt> ptr)
{
	return std::make_unique<unique_asset<T, Dt>>(std::move(ptr));
}

using asset_map = std::unordered_map<uint32_t, std::unique_ptr<asset>>;

static auto& g_asset_map()
{
	static asset_map g_underlying_asset_map;
	return g_underlying_asset_map;
}

}}