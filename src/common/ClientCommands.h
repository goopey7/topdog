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

struct UpdatePos
{
	float posx;
	float posy;
	float time;
};

struct UpdateVel
{
	float velx;
	float vely;
	float time;
};

struct RotStart
{
	float angle;
	int dir; // -1 for left, 0 for none, 1 for right
	float time;
};

struct RotEnd
{
	float angle;
	float time;
};

struct Fire
{
	float posx;
	float posy;
	float velx;
	float vely;
	float time;
};

struct HealthChange
{
	float health;
	bool isDead;
};

#define CLIENT_COMMANDS                                                                            \
	Disconnect, Ready, Fire, UpdatePos, UpdateVel, RotStart, RotEnd, HealthChange

using ClientCommand = std::variant<CLIENT_COMMANDS>;

#define STRINGIFY_CLIENT_COMMAND(cmd)                                                              \
	[&]()                                                                                          \
	{                                                                                              \
		std::stringstream ss;                                                                      \
		std::visit(                                                                                \
			[&](auto&& arg)                                                                        \
			{                                                                                      \
				ss << "+";                                                                         \
				ss << cmd.index();                                                                 \
				using T = std::decay_t<decltype(arg)>;                                             \
				if constexpr (std::is_same_v<T, UpdateVel>)                                        \
				{                                                                                  \
					ss << ":" << arg.velx << ":" << arg.vely << ":" << arg.time;                   \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, UpdatePos>)                                   \
				{                                                                                  \
					ss << ":" << arg.posx << ":" << arg.posy << ":" << arg.time;                   \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, RotStart>)                                    \
				{                                                                                  \
					ss << ":" << arg.angle << ":" << arg.dir << ":" << arg.time;                   \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, RotEnd>)                                      \
				{                                                                                  \
					ss << ":" << arg.angle << ":" << arg.time;                                     \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, Ready>)                                       \
				{                                                                                  \
					ss << ":" << arg.ready;                                                        \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, Fire>)                                        \
				{                                                                                  \
					ss << ":" << arg.posx << ":" << arg.posy << ":" << arg.velx << ":" << arg.vely \
					   << ":" << arg.time;                                                         \
				}                                                                                  \
				else if constexpr (std::is_same_v<T, HealthChange>)                                \
				{                                                                                  \
					ss << ":" << arg.health << ":" << arg.isDead;                                  \
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

inline std::vector<ClientCommand> parseClientCommands(const std::string& str)
{
	// split string by '+'
	std::vector<std::string> commands;
	int start = 0;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == '+')
		{
			commands.push_back(str.substr(start, i - start));
			start = i + 1;
		}
	}

	// split string by ':'
	std::vector<ClientCommand> clientCommands;
	for (int i = 0; i < commands.size(); i++)
	{
		std::vector<std::string> tokens;
		start = 0;
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
		if (tokens.size() == 4)
		{
			if (std::holds_alternative<UpdateVel>(cmd))
			{
				auto uv = std::get<UpdateVel>(cmd);
				uv.velx = std::stof(tokens[1]);
				uv.vely = std::stof(tokens[2]);
				uv.time = std::stof(tokens[3]);
				cmd = uv;
			}
			else if (std::holds_alternative<UpdatePos>(cmd))
			{
				auto up = std::get<UpdatePos>(cmd);
				up.posx = std::stof(tokens[1]);
				up.posy = std::stof(tokens[2]);
				up.time = std::stof(tokens[3]);
				cmd = up;
			}
			else if (std::holds_alternative<RotStart>(cmd))
			{
				auto ur = std::get<RotStart>(cmd);
				ur.angle = std::stof(tokens[1]);
				ur.dir = std::stoi(tokens[2]);
				ur.time = std::stof(tokens[3]);
				cmd = ur;
			}
		}
		else if (tokens.size() == 3)
		{
			if (std::holds_alternative<RotEnd>(cmd))
			{
				auto ur = std::get<RotEnd>(cmd);
				ur.angle = std::stof(tokens[1]);
				ur.time = std::stof(tokens[2]);
				cmd = ur;
			}
			else if (std::holds_alternative<HealthChange>(cmd))
			{
				auto hc = std::get<HealthChange>(cmd);
				hc.health = std::stof(tokens[1]);
				hc.isDead = std::stoi(tokens[2]);
				cmd = hc;
			}
		}
		else if (tokens.size() == 6)
		{
			if (std::holds_alternative<Fire>(cmd))
			{
				auto fire = std::get<Fire>(cmd);
				fire.posx = std::stof(tokens[1]);
				fire.posy = std::stof(tokens[2]);
				fire.velx = std::stof(tokens[3]);
				fire.vely = std::stof(tokens[4]);
				fire.time = std::stof(tokens[5]);
				cmd = fire;
			}
		}
		else if (tokens.size() == 2)
		{
			if (std::holds_alternative<Ready>(cmd))
			{
				std::get<Ready>(cmd).ready = std::stoi(tokens[1]);
			}
		}

		clientCommands.push_back(cmd);
	}

	return clientCommands;
}
