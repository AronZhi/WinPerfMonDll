#include "pch.h"
#include "AmqpChannel.h"

bool CheckAmqpReply(amqp_rpc_reply_t response, const std::string& err_msg)
{
	return response.reply_type == AMQP_RESPONSE_NORMAL;
}

AmqpChannel::AmqpChannel(uint16_t id, amqp_connection_state_t conn)
	: _id(id)
	, _conn(conn)
	, _confirm_mode(false)
{
	amqp_channel_open(_conn, id);
	_buff = new char[Channel_Buff_Size];
}

AmqpChannel::~AmqpChannel()
{
	if (_buff)
		delete _buff;
	amqp_channel_close(_conn, _id, AMQP_REPLY_SUCCESS);
}

void AmqpChannel::enableConfirmMode()
{
	if (!_confirm_mode)
	{
		amqp_confirm_select(_conn, _id);
		_confirm_mode = true;
	}
}

int AmqpChannel::SendMsg(const std::string& msg, const std::string& exchange, const std::string& rotekey)
{
	memset(_buff, 0, Channel_Buff_Size);
	memcpy(_buff, msg.c_str(), msg.size());
	amqp_bytes_t message_bytes;
	message_bytes.len = msg.size();
	message_bytes.bytes = _buff;
	if (AMQP_STATUS_OK != amqp_basic_publish(_conn, _id, amqp_cstring_bytes(exchange.c_str()),
		amqp_cstring_bytes(rotekey.c_str()), 0, 0, NULL, message_bytes))
		return -1;
	if (_confirm_mode)
	{
		amqp_frame_t frame;
		amqp_rpc_reply_t ret;
		if (AMQP_STATUS_OK != amqp_simple_wait_frame(_conn, &frame))
			return -2;
		if (AMQP_FRAME_METHOD == frame.frame_type)
		{
			switch (frame.payload.method.id)
			{
			case AMQP_BASIC_ACK_METHOD:
				break;
			case AMQP_BASIC_NACK_METHOD:
				return -3;
			default:
				return -4;
			}
		}
	}

	return 0;
}