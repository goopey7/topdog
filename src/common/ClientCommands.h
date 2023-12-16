#pragma once

#include <sstream>
#include <variant>
#include <vector>

#include <boost/mp11.hpp>

#include <raylib.h>

struct Disconnect
{
};

struct Ready
{
	bool ready;
};

struct UpdateStatus
{
	float posx;
	float posy;
	float velx;
	float vely;
	float angle;
};

#define CLIENT_COMMANDS Disconnect, Ready, UpdateStatus

using ClientCommand = std::variant<CLIENT_COMMANDS>;

#define STRINGIFY_CLIENT_COMMAND(cmd)                                                              \
	[&]()                                                                                          \
	{                                                                                              \
		std::stringstream ss;                                                                      \
		std::visit(                                                                                \
			[&](auto&& arg)                                                                        \
			{                                                                                      \
				ss << cmd.index();                                                                 \
				using T = std::decay_t<decltype(arg)>;                                             \
				if constexpr (std::is_same_v<T, UpdateStatus>)                                     \
				{                                                                                  \
					ss << ":" << arg.posx << ":" << arg.posy << ":" << arg.velx \
					   << ":" << arg.vely << ":" << arg.angle;                                     \
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
	if (tokens.size() == 6)
	{
		if (std::holds_alternative<UpdateStatus>(cmd))
		{
			auto us = std::get<UpdateStatus>(cmd);
			us.posx = std::stof(tokens[1]);
			us.posy = std::stof(tokens[2]);
			us.velx = std::stof(tokens[3]);
			us.vely = std::stof(tokens[4]);
			us.angle = std::stof(tokens[5]);
			cmd = us;
		}
	}
	else if (tokens.size() == 2)
	{
		if (std::holds_alternative<Ready>(cmd))
		{
			std::get<Ready>(cmd).ready = std::stoi(tokens[1]);
		}
	}

	return cmd;
}
