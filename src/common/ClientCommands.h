#pragma once

#include <sstream>
#include <variant>

#include <boost/mp11.hpp>

struct Disconnect
{
	int id = 0;
};

struct Ready
{
	int id = 1;
};

struct NotReady
{
	int id = 2;
};

struct UpdatePosition
{
	int id = 3;
	float x = 0;
	float y = 0;
};

struct UpdateVelocity
{
	int id = 4;
	float x = 0;
	float y = 0;
};

struct UpdateRotation
{
	int id = 5;
	float angle = 0;
};

#define COMMANDS Disconnect, Ready, NotReady, UpdatePosition, UpdateVelocity, UpdateRotation

using ClientCommand = std::variant<COMMANDS>;

#define STRINGIFY_COMMAND(cmd)                                                                     \
	[&]()                                                                                          \
	{                                                                                              \
		std::stringstream ss;                                                                      \
		std::visit(                                                                                \
			[&](auto&& arg)                                                                        \
			{                                                                                      \
				ss << arg.id;                                                                      \
				using T = std::decay_t<decltype(arg)>;                                             \
				if constexpr (std::is_same_v<T, UpdatePosition> ||                                 \
							  std::is_same_v<T, UpdateVelocity>)                                   \
				{                                                                                  \
					ss << ":" << arg.x << ":" << arg.y;                                            \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, UpdateRotation>)                              \
				{                                                                                  \
					ss << ":" << arg.angle;                                                        \
				}                                                                                  \
			},                                                                                     \
			cmd);                                                                                  \
		return ss.str();                                                                           \
	}()

// https://stackoverflow.com/questions/60564132/default-constructing-an-stdvariant-from-index
template <typename V> auto variant_from_index(size_t index) -> V
{
	using namespace boost::mp11;
	return mp_with_index<mp_size<V>>(index, [](auto I) { return V(std::in_place_index<I>); });
}
