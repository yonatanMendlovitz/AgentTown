#include "ClientThreadConnection.h"
//#include "../CAI/Framework/Concrete_Mediator.h"

ClientThreadConnection::ClientThreadConnection(tcp::socket&& socket, std::unordered_map<int, std::shared_ptr<Connection>>& connections, int connection_id_, Interface_Mediator* mediator_) : socket_(std::move(socket))
{
	this->mediator_ = mediator_;
	this->connections_ = &connections;
	this->connection_id_ = connection_id_;
	message_buffer.resize(MAX_MESSAGE_SIZE);
}

ClientThreadConnection::~ClientThreadConnection()
{
	if(connections_->count(connection_id_) != 0)
		disconnect();
}

int ClientThreadConnection::get_id() const
{
	return connection_id_;
}

void ClientThreadConnection::run()
{
	read();
}

bool ClientThreadConnection::proccess_complete_message(const string& input, const size_t size)
{

	if (size > MAX_MESSAGE_SIZE)
		return false;

	//Find location of data
	int start_type_location = input.find(message::open_header);
	int end_type_location = input.find(message::close_header);

	if (start_type_location == -1 || end_type_location == -1 || start_type_location > end_type_location)
		return false;


	int start_message = input.find(message::open_message);
	int end_message = input.find(message::close_message);

	if (start_message == -1 || end_message == -1 || start_message > end_message)
		return false;

	//extract type
	incoming_msg.header.type = input.substr(start_type_location + message::open_header.length(), end_type_location - message::close_header.length());

	std::vector<char> incoming_data(input.begin() + start_message + message::open_message.length(), input.begin() + end_message);
	incoming_msg << incoming_data;
	incoming_msg.header.connection_id = this->connection_id_;
	incoming_msg.direction = message::Message::message_direction::In;

	mediator_->push_message(incoming_msg);
	message::reset(incoming_msg);

	if (end_message < input.length())
		proccess_complete_message(input.substr(end_message + 1), size - (end_message + 1));

	return true;
}


void ClientThreadConnection::read() {
	try
	{
		auto self(shared_from_this());
		socket_.async_read_some(buffer(message_buffer.data(), message_buffer.size()),
			[self, this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					if (!proccess_complete_message(message_buffer.data(), length))
					{
						std::cout << "client serial number (" << connection_id_ << ") rejected" << endl;
						disconnect();
					}
					else
						read();
				}
				else
				{
					disconnect();
				}
			});
	}
	catch (std::exception e)
	{
		cout << e.what();
	}
}

void ClientThreadConnection::send(const string& message){
	std::scoped_lock<std::mutex> lock(send_mutex);
	boost::asio::write(socket_, boost::asio::buffer(message));
}


void ClientThreadConnection::send_all(const string& message) {
	for (auto& connection : *connections_)
		connection.second->send(message);
}

void ClientThreadConnection::send_to_id_list(const std::string& message, const std::vector<int> id_list)
{
	for (auto& connection : *connections_)
		if(std::find(id_list.begin(), id_list.end(),connection.first) != id_list.end())
			connection.second->send(message);
}


void ClientThreadConnection::disconnect() {

	try {
		socket_.close();
		connections_->erase(connection_id_);
		mediator_->push_message(Message_Generator::generate_disconnect_message(connection_id_));
		std::cout << "client serial number (" << connection_id_ << ") disconnected" << endl;
	}
	catch (...) {
		std::cout << "closing failed: connection already closed" << std::endl;
	}
}


void ClientThreadConnection::provide_message(message::Message& msg)
{
	send(message::to_string(msg));
}

std::string ClientThreadConnection::service_name()
{
	return "connection id:" + this->connection_id_;
}
