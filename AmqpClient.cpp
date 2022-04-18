#include "pch.h"
#include "AmqpClient.h"

int AmqpClient::Login(const std::string& hostIp, int port, const std::string& account,
	const std::string& password, const std::string& vhost)
{
	_conn = amqp_new_connection();
	if (!_conn)
		return -1;
	amqp_socket_t* socket = amqp_tcp_socket_new(_conn);
	if (!socket)
		return -2;
	if (AMQP_STATUS_OK != amqp_socket_open(socket, hostIp.c_str(), port))
		return -3;
	if (!CheckAmqpReply(amqp_login(_conn, vhost.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, account.c_str(), password.c_str()),
		"amqp_login"))
		return -4;
	return 0;
}

int AmqpClient::Logout()
{
	if (_conn)
	{
		for (std::map<uint16_t, AmqpChannel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
		{
			delete it->second;
			it->second = NULL;
		}
		_channels.clear();
		if (!CheckAmqpReply(amqp_connection_close(_conn, AMQP_REPLY_SUCCESS), "amqp_connection_close"))
			return -1;
		if (AMQP_STATUS_OK != amqp_destroy_connection(_conn))
			return -2;
		_conn = NULL;
	}
}

AmqpChannel* AmqpClient::Channel(uint16_t id)
{
	std::map<uint16_t, AmqpChannel*>::iterator it = _channels.find(id);
	if (it == _channels.end())
	{
		AmqpChannel* p = new AmqpChannel(id, _conn);
		_channels[id] = p;
		return p;
	}
	else
	{
		return it->second;
	}
}
