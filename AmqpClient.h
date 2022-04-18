#ifndef AMQP_CLIENT
#define AMQP_CLIENT

#include <string>
#include <map>
#include "rabbitmq-c/amqp.h"
#include "rabbitmq-c/tcp_socket.h"
#include "AmqpChannel.h"

class AmqpClient
{
private:
	amqp_connection_state_t _conn;
	std::map<uint16_t, AmqpChannel*> _channels;

protected:
	AmqpClient() : _conn(NULL) {}

public:
	~AmqpClient() { Logout(); }

	static AmqpClient& GetInstance() {
		static AmqpClient instance;
		return instance;
	}

	int Login(const std::string& hostIp, int port, const std::string& account,
		const std::string& password, const std::string& vhost);
	int Logout();
	AmqpChannel* Channel(uint16_t id);
};

#endif // AMQP_CLIENT