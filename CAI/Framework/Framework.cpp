#include "Framework.h"

Framework::Framework()
{
    host = "127.0.0.1";
	port = 7777;
	end_point = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port);
	base_server = std::make_unique<MainServer>(io_context_, end_point);
	base_server->start();
	engine = GraphicsEngine();
	agent_network.subscribe_to_network(&engine);
	std::initializer_list<Component<message::message>*> msg_based = { &*base_server, &interpreter } ;
	std::initializer_list<Component<message::ParsedMessage>*> pmsg_based = { &agent_network, &engine };
	SystemMediator = std::make_unique<ConcreteMediator>(msg_based, pmsg_based);
}

void Framework::start()
{
	context_thread = std::thread([this]() { io_context_.run(); });
	interpreter_thread = std::thread([this]() { interpreter.run(); });
	representational_network_thread = std::thread([this]() { agent_network.run(); });
	engine.run();
}

void Framework::halt()
{

}

void Framework::close()
{
	interpreter.kill();
	engine.quit();
	base_server->close();
}

Framework::~Framework()
{
	context_thread.join();
	interpreter.kill();
	interpreter_thread.join();
}