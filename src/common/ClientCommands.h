#pragma once

#include <sstream>
#include <variant>
#include <vector>

#include <boost/mp11.hpp>

struct Disconnect
{
};

struct Ready
{
	bool ready;
};

struct UpdatePosition
{
	float x = 0;
	float y = 0;
};

struct UpdateVelocity
{
	float x = 0;
	float y = 0;
};

struct UpdateRotation
{
	float angle = 0;
};

#define CLIENT_COMMANDS Disconnect, Ready, UpdatePosition, UpdateVelocity, UpdateRotation

using ClientCommand = std::variant<CLIENT_COMMANDS>;

#define STRINGIFY_CLIENT_COMMAND(cmd)                                                                     \
	[&]()                                                                                          \
	{                                                                                              \
		std::stringstream ss;                                                                      \
		std::visit(                                                                                \
			[&](auto&& arg)                                                                        \
			{                                                                                      \
				ss << cmd.index();                                                                 \
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
				else if constexpr (std::is_same_v<T, Ready>)                                       \
				{                                                                                  \
					ss << ":" << arg.ready;                                                        \
				}                                                                                  \
			},                                                                                     \
			cmd);                                                                                  \
		ss << ":";                                                                                 \
		return ss.str();                                                                           \
	}()

// https://stackoverflow.com/questions/60564132/default-constructing-an-stdvariant-from-index
template <typename V> auto variant_from_index(size_t index) -> V
{
	using namespace boost::mp11;
	return mp_with_index<mp_size<V>>(index, [](auto I) { return V(std::in_place_index<I>); });
}

inline ClientCommand parseClientCommand(const std::string& str)
{
	// split string by ':'
	std::vector<std::string> tokens;
	int start = 0;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == ':')
		{
			tokens.push_back(str.substr(start, i - start));
			start = i + 1;
		}
	}

	auto cmd = variant_from_index<ClientCommand>(std::stoi(tokens[0]));

	// initialize structs with arguments if necessary
	if (tokens.size() == 3)
	{
		if (std::holds_alternative<UpdatePosition>(cmd))
		{
			std::get<UpdatePosition>(cmd).x = std::stof(tokens[1]);
			std::get<UpdatePosition>(cmd).y = std::stof(tokens[2]);
		}
		else if (std::holds_alternative<UpdateVelocity>(cmd))
		{
			std::get<UpdateVelocity>(cmd).x = std::stof(tokens[1]);
			std::get<UpdateVelocity>(cmd).y = std::stof(tokens[2]);
		}
	}
	else if (tokens.size() == 2)
	{
		if (std::holds_alternative<UpdateRotation>(cmd))
		{
			std::get<UpdateRotation>(cmd).angle = std::stof(tokens[1]);
		}
		else if (std::holds_alternative<Ready>(cmd))
		{
			std::get<Ready>(cmd).ready = std::stoi(tokens[1]);
		}
	}

	return cmd;
}

