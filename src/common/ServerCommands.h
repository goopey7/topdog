#pragma once

#include <sstream>
#include <variant>
#include <vector>

#include <boost/mp11.hpp>

struct StartGame
{
};

struct NewClient
{
	std::string name;
};

struct ClientDisconnected
{
	std::string name;
};

struct ClientReady
{
	std::string name;
	bool ready;
};

struct ClientUpdateStatus
{
	std::string name;
	float posx;
	float posy;
	float velx;
	float vely;
	float angle;
	bool fire;
};

#define SERVER_COMMANDS StartGame, NewClient, ClientDisconnected, ClientReady, ClientUpdateStatus

using ServerCommand = std::variant<SERVER_COMMANDS>;

#define STRINGIFY_SERVER_COMMAND(cmd)                                                              \
	[&]()                                                                                          \
	{                                                                                              \
		std::stringstream ss;                                                                      \
		std::visit(                                                                                \
			[&](auto&& arg)                                                                        \
			{                                                                                      \
				ss << cmd.index();                                                                 \
				using T = std::decay_t<decltype(arg)>;                                             \
				if constexpr (std::is_same_v<T, NewClient> ||                                      \
							  std::is_same_v<T, ClientDisconnected>)                               \
				{                                                                                  \
					ss << ":" << arg.name;                                                         \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, ClientReady>)                                 \
				{                                                                                  \
					ss << ":" << arg.name << ":" << arg.ready;                                     \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, ClientUpdateStatus>)                          \
				{                                                                                  \
					ss << ":" << arg.name << ":" << arg.posx << ":" << arg.posy << ":" << arg.velx \
					   << ":" << arg.vely << ":" << arg.angle << ":" << arg.fire;                  \
				}                                                                                  \
			},                                                                                     \
			cmd);                                                                                  \
		ss << ":";                                                                                 \
		return ss.str();                                                                           \
	}()

// https://stackoverflow.com/questions/60564132/default-constructing-an-stdvariant-from-index
template <typename V> auto srv_variant_from_index(size_t index) -> V
{
	using namespace boost::mp11;
	return mp_with_index<mp_size<V>>(index, [](auto I) { return V(std::in_place_index<I>); });
}

inline ServerCommand parseServerCommand(const std::string& str)
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

	auto cmd = srv_variant_from_index<ServerCommand>(std::stoi(tokens[0]));

	if (tokens.size() == 8)
	{
		if (std::holds_alternative<ClientUpdateStatus>(cmd))
		{
			std::get<ClientUpdateStatus>(cmd).name = tokens[1];
			std::get<ClientUpdateStatus>(cmd).posx = std::stof(tokens[2]);
			std::get<ClientUpdateStatus>(cmd).posy = std::stof(tokens[3]);
			std::get<ClientUpdateStatus>(cmd).velx = std::stof(tokens[4]);
			std::get<ClientUpdateStatus>(cmd).vely = std::stof(tokens[5]);
			std::get<ClientUpdateStatus>(cmd).angle = std::stof(tokens[6]);
			std::get<ClientUpdateStatus>(cmd).fire = std::stoi(tokens[7]);
		}
	}
	else if (tokens.size() == 3)
	{
		if (std::holds_alternative<ClientReady>(cmd))
		{
			std::get<ClientReady>(cmd).name = tokens[1];
			std::get<ClientReady>(cmd).ready = std::stoi(tokens[2]);
		}
	}
	else if (tokens.size() == 2)
	{
		if (std::holds_alternative<ClientDisconnected>(cmd))
		{
			std::get<ClientDisconnected>(cmd).name = tokens[1];
		}
		else if (std::holds_alternative<NewClient>(cmd))
		{
			std::get<NewClient>(cmd).name = tokens[1];
		}
	}

	return cmd;
}
