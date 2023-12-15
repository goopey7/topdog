#pragma once

#include <sstream>
#include <variant>

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

using ClientCommand =
	std::variant<COMMANDS>;

using ClientCommandTypes =
	std::tuple<COMMANDS>;

inline std::string cmd(const ClientCommand& cmd)
{
	std::stringstream ss;

	std::visit(
		[&](const auto& c)
		{
			using T = std::decay_t<decltype(c)>;
			if constexpr (requires {
							  c.x;
							  c.y;
						  })
			{
				ss << c.x << ":" << c.y;
			}
			else if constexpr (requires { c.angle; })
			{
				ss << c.angle;
			}
		},
		cmd);

	return ss.str();
}
