#pragma once
#include <string>
#include "message.h"

class Message_Generator
{
public:
	static message::Message generate_error_message(std::string type)
	{
		if (type == "new no id")
		{

		}
		else if (type == "")
		{

		}
		message::Message a;
		a.direction = message::Message::message_direction::Out;
		return a;
	}

	static message::Message generate_disconnect_message(int id)
	{
		message::Message a;
		a.header.connection_id = id;
		a.header.type = "disconnect";
		a.direction = message::Message::message_direction::In;
		return a;
	}

	//static message::Message generate_disconnect_message(int id)
	//{
	//	message::Message a;
	//	a.header.connection_id = id;
	//	a.header.type = "disconnect";
	//	a.direction = message::Message::message_direction::In;
	//	return a;
	//}
};