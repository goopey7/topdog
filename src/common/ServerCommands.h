#pragma once

#include <algorithm>
#include <iostream>
#include <sstream>
#include <variant>
#include <vector>

#include <boost/mp11.hpp>

#define SERVER_TCP_PORT 4916
#define SERVER_UDP_PORT 4917

struct StartGame
{
	double time;
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

struct ClientUpdateVel
{
	std::string name;
	float velx;
	float vely;
	float time;
};

struct ClientUpdatePos
{
	std::string name;
	float posx;
	float posy;
	float time;
};

struct ClientRotStart
{
	std::string name;
	float angle;
	int dir;
	float time;
};

struct ClientRotEnd
{
	std::string name;
	float angle;
	float time;
};

struct ClientFire
{
	std::string name;
	float posx;
	float posy;
	float velx;
	float vely;
	float time;
};

struct ClientHealthChange
{
	std::string name;
	float health;
	bool isDead;
};

#define SERVER_COMMANDS                                                                            \
	StartGame, NewClient, ClientDisconnected, ClientReady, ClientFire, ClientUpdateVel,            \
		ClientUpdatePos, ClientRotStart, ClientRotEnd, ClientHealthChange

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
				else if constexpr (std::is_same_v<T, ClientUpdateVel>)                             \
				{                                                                                  \
					ss << ":" << arg.name << ":" << arg.velx << ":" << arg.vely << ":"             \
					   << arg.time;                                                                \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, ClientUpdatePos>)                             \
				{                                                                                  \
					ss << ":" << arg.name << ":" << arg.posx << ":" << arg.posy << ":"             \
					   << arg.time;                                                                \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, ClientRotStart>)                              \
				{                                                                                  \
					ss << ":" << arg.name << ":" << arg.angle << ":" << arg.dir << ":"             \
					   << arg.time;                                                                \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, ClientRotEnd>)                                \
				{                                                                                  \
					ss << ":" << arg.name << ":" << arg.angle << ":" << arg.time;                  \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, ClientFire>)                                  \
				{                                                                                  \
					ss << ":" << arg.name << ":" << arg.posx << ":" << arg.posy << ":" << arg.velx \
					   << ":" << arg.vely << ":" << arg.time;                                      \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, StartGame>)                                   \
				{                                                                                  \
					ss << ":" << arg.time;                                                         \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, ClientHealthChange>)                          \
				{                                                                                  \
					ss << ":" << arg.name << ":" << arg.health << ":" << arg.isDead;               \
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

	// check if tokens[0] is a number
	if (tokens.size() == 0 || !std::all_of(tokens[0].begin(), tokens[0].end(), ::isdigit))
	{
		throw std::runtime_error("Invalid Server Command: " + str);
	}

	auto cmd = srv_variant_from_index<ServerCommand>(std::stoi(tokens[0]));

	if (tokens.size() == 5)
	{
		if (std::holds_alternative<ClientUpdateVel>(cmd))
		{
			std::get<ClientUpdateVel>(cmd).name = tokens[1];
			std::get<ClientUpdateVel>(cmd).velx = std::stof(tokens[2]);
			std::get<ClientUpdateVel>(cmd).vely = std::stof(tokens[3]);
			std::get<ClientUpdateVel>(cmd).time = std::stof(tokens[4]);
		}
		else if (std::holds_alternative<ClientUpdatePos>(cmd))
		{
			std::get<ClientUpdatePos>(cmd).name = tokens[1];
			std::get<ClientUpdatePos>(cmd).posx = std::stof(tokens[2]);
			std::get<ClientUpdatePos>(cmd).posy = std::stof(tokens[3]);
			std::get<ClientUpdatePos>(cmd).time = std::stof(tokens[4]);
		}
		else if (std::holds_alternative<ClientRotStart>(cmd))
		{
			std::get<ClientRotStart>(cmd).name = tokens[1];
			std::get<ClientRotStart>(cmd).angle = std::stof(tokens[2]);
			std::get<ClientRotStart>(cmd).dir = std::stoi(tokens[3]);
			std::get<ClientRotStart>(cmd).time = std::stof(tokens[4]);
		}
	}
	else if (tokens.size() == 4)
	{
		if (std::holds_alternative<ClientRotEnd>(cmd))
		{
			std::get<ClientRotEnd>(cmd).name = tokens[1];
			std::get<ClientRotEnd>(cmd).angle = std::stof(tokens[2]);
			std::get<ClientRotEnd>(cmd).time = std::stof(tokens[3]);
		}
		else if (std::holds_alternative<ClientHealthChange>(cmd))
		{
			std::get<ClientHealthChange>(cmd).name = tokens[1];
			std::get<ClientHealthChange>(cmd).health = std::stof(tokens[2]);
			std::get<ClientHealthChange>(cmd).isDead = std::stoi(tokens[3]);
		}
	}
	else if (tokens.size() == 7)
	{
		if (std::holds_alternative<ClientFire>(cmd))
		{
			std::get<ClientFire>(cmd).name = tokens[1];
			std::get<ClientFire>(cmd).posx = std::stof(tokens[2]);
			std::get<ClientFire>(cmd).posy = std::stof(tokens[3]);
			std::get<ClientFire>(cmd).velx = std::stof(tokens[4]);
			std::get<ClientFire>(cmd).vely = std::stof(tokens[5]);
			std::get<ClientFire>(cmd).time = std::stof(tokens[6]);
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
		else if (std::holds_alternative<StartGame>(cmd))
		{
			std::get<StartGame>(cmd).time = std::stof(tokens[1]);
		}
	}

	return cmd;
}
