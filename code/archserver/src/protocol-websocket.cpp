#include "protocol-websocket.hpp"

using namespace std;
using namespace arch;

#define _MAKE_UINT16(u_high, u_low) ((u_high << 8) | u_low)
#define _MAKE_UINT32(u_4, u_3, u_2, u_1)	\
		((u_4 << 24) |	\
		 (u_3 << 16) |	\
		 (u_2 << 8)  |	\
		 (u_1)			\
		)

ProtoProcRet ProtoProcWebSocket::proc_istrm(IProtocolObject& dest, const uv_buf_t* uvbuffer, ssize_t uvreadlen)
{
	ProtoProcRet retval = PPR_AGAIN;

	Internal_ProtoObjectWebSocket& obj = static_cast<Internal_ProtoObjectWebSocket&>(dest);
	obj._commit_pos = 0;

	const byte_ptr read_buf = (byte_ptr)(uvbuffer->base);

	const index_t cache_size = (index_t)obj._cache.size();
	if (cache_size > 0)
	{
		byte_ptr p = obj._cache.data();
		for (index_t i = 0; i < cache_size && retval >= 0; ++i, ++p)
		{
			WSParsingPhase status = _proc_istrm_byte(obj, read_buf, p, i);
			if (WSPP_End != status)
			{
				retval = WSPP_Error != status ? PPR_AGAIN : PPR_ERROR;
			}
			else
			{
				retval = PPR_PULSE;
			}
		}
	}

	const index_t read_size = (index_t)uvreadlen;
	if (read_size > 0 && retval >= 0)
	{
		byte_ptr p = (byte_ptr)(uvbuffer->base);
		for (index_t i = 0; i < read_size && retval >= 0; ++i, ++p)
		{
			WSParsingPhase status = _proc_istrm_byte(obj, read_buf, p, i + cache_size);
			if (WSPP_End != status)
			{
				retval = WSPP_Error != status ? PPR_AGAIN : PPR_ERROR;
			}
			else
			{
				retval = PPR_PULSE;
			}
		}
	}

	const index_t total_size = cache_size + read_size;
	if (retval >= 0 && total_size > 0 && obj._commit_pos < total_size - 1)
	{
		Internal_ProtoObjectWebSocket::buffer_t& cache = obj._cache;
		index_t idx = obj._commit_pos;
		if (idx < cache_size)
		{
			const index_t len = cache_size - idx;
			for (index_t i = 0; i < len; ++i)
			{
				cache[i] = cache[idx + i];
			}

			idx += len;
		}

		if (cache_size > idx)
		{
			cache.resize(cache_size - idx);
		}
		else
		{
			cache.clear();
		}

		if (idx < total_size)
		{
			cache.insert(cache.end(), read_buf + idx - cache_size, read_buf + total_size - cache_size);
		}
	}

	return retval;
}

bool ProtoProcWebSocket::proc_ostrm(std::string& obuffer, const IProtocolObject& src)
{
	bool retval = true;

	return retval;
}

bool ProtoProcWebSocket::proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj)
{
	return false;
}

WSParsingPhase ProtoProcWebSocket::_proc_istrm_byte(Internal_ProtoObjectWebSocket& obj, byte_ptr read_buf, byte_ptr p, index_t idx)
{
	WSParsingPhase retval = WSPP_Again;

	byte_t ch = *p;

	switch (obj._parsing_phase)
	{
	case WSPP_Start:
	case WSPP_FinRsvsOpcode:
		{
			WSMsgFrame empty_frame;
			obj._frames.push_back(empty_frame);
		}
		
		obj._frames.back().digest_fin_rsv_opcode(ch);

		if ((obj._frames.size() == 1 && obj._frames.back().opcode == WSO_Binary) ||
			(obj._frames.size() == 1 && obj._frames.back().opcode == WSO_Text) ||
			(obj._frames.size() > 1 && obj._frames.back().opcode == WSO_Continuation))
		{
			obj._parsing_phase = WSPP_MaskPayloadLen;
			obj._commit_pos = idx;
		}
		else if (obj._frames.back().opcode == WSO_Ping)
		{
			obj._parsing_phase = WSPP_Ping;
			obj._commit_pos = idx;
		}
		else
		{
			retval = WSPP_Error;	// error, close connection, log as a warning.
		}

		break;
		
	case WSPP_MaskPayloadLen:
		obj._frames.back().digest_mask_payload(ch);
		switch (obj._frames.back().payloadlen_type())
		{
		case 0:
			if (obj._frames.back().mask)
				obj._parsing_phase = WSPP_MaskingKey;
			else
				obj._parsing_phase = WSPP_Payload;
			obj._commit_pos = idx;
			break;

		case 16:
			obj._parsing_phase = WSPP_ExtPayloadLen16;
			obj._commit_pos = idx;
			break;

		case 64:
			obj._parsing_phase = WSPP_ExtPayloadLen64;
			obj._commit_pos = idx;
			break;

		default:
			retval = WSPP_Error;
		}
		break;

	case WSPP_ExtPayloadLen16:
		if (obj._cache.size() < 1)
		{
			obj._cache.push_back(ch);
		}
		else
		{
			obj._cache.push_back(ch);
			obj._frames.back().target_len = _MAKE_UINT16(
				obj._cache[0],
				obj._cache[1]
			);
			obj._cache.clear();

			if (obj._frames.back().mask)
				obj._parsing_phase = WSPP_MaskingKey;
			else
				obj._parsing_phase = WSPP_Payload;
		}
		obj._commit_pos = idx;
		break;

	case WSPP_ExtPayloadLen64:
		// Sorry, we DO NOT support the payload whose length is longer than 65536.
		assert(false);
		retval = WSPP_Error;		// error, close connection, log as a fatal error.
		break;

	case WSPP_MaskingKey:
		if (obj._cache.size() < 3)
		{
			obj._cache.push_back(ch);
		}
		else
		{
			obj._cache.push_back(ch);
			memcpy(obj._frames.back().mask_key,
				obj._cache.data(),
				4);
			obj._cache.clear();
			obj._parsing_phase = WSPP_Payload;
		}
		obj._commit_pos = idx;
		break;

	case WSPP_Payload:
		if (obj._frames.back().data.size() < obj._frames.back().target_len - 1)
		{
			obj._frames.back().data.push_back(ch);
			obj._commit_pos = idx;
		}
		else
		{
			obj._frames.back().data.push_back(ch);
			obj._commit_pos = idx;

			if (obj._frames.back().mask)
				obj._frames.back().decode_data();

			if (obj._frames.back().fin)
			{
				obj._parsing_phase = WSPP_Start;
				retval = WSPP_End;
			}
		}
		break;

	default:
		retval = WSPP_Error;
	}

	return retval;
}

