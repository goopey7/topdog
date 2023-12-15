enum class ClientCommand
{
	DISCONNECT = 0,
	READY = 1,
	NOT_READY = 2,
	UPDATE_POSITION = 3,
	UPDATE_VELOCITY = 4,
	UPDATE_ROTATION = 5,
};

#define cmd(x) std::to_string((int)ClientCommand::x)
