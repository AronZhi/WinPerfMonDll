#ifndef AMQP_CHANNEL
#define AMQP_CHANNEL

#include <string>
#include "rabbitmq-c/amqp.h"
#include "rabbitmq-c/tcp_socket.h"

const int Channel_Buff_Size = 1024;

bool CheckAmqpReply(amqp_rpc_reply_t response, const std::string& err_msg);

class AmqpChannel
{
private:
	uint16_t _id;
	amqp_connection_state_t _conn;
	char* _buff;

public:
	AmqpChannel(uint16_t id, amqp_connection_state_t conn);
	~AmqpChannel();

	int SendMsg(const std::string& msg, const std::string& exchange, const std::string& rotekey);
};

#endif // AMQP_CHANNEL