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

ProtoProcRet ProtoProcWebSocket::proc_istrm(IProtocolObject& dest, cbyte_ptr readbuf, ssize_t toreadlen, ssize_t& procbytes)
{
	procbytes = 0;

	Internal_ProtoObjectWebSocket& obj = static_cast<Internal_ProtoObjectWebSocket&>(dest);
	obj._commit_pos = 0;

	const index_t cache_size = (index_t)obj._cache.size();
	if (cache_size > 0)
	{
		cbyte_ptr p = obj._cache.data();
		for (index_t i = 0; i < cache_size; ++i, ++p)
		{
			WSParsingPhase status = _proc_istrm_byte(obj, readbuf, p, i);
			
			if (WSPP_End == status && WSPP_Error != status)
			{
				return PPR_PULSE;
			}
			else if(WSPP_Error == status)
			{
				return PPR_ERROR;
			}
		}
	}

	const index_t read_size = (index_t)toreadlen;
	if (read_size > 0)
	{
		const byte_t* p = readbuf;
		for (index_t i = 0; i < read_size; ++i, ++p)
		{
			++procbytes;
			WSParsingPhase status = _proc_istrm_byte(obj, readbuf, p, i + cache_size);
			if (WSPP_End == status && WSPP_Error != status)
			{
				return PPR_PULSE;
			}
			else if (WSPP_Error == status)
			{
				return PPR_ERROR;
			}
		}
	}

	const index_t total_size = cache_size + read_size;
	if (total_size > 0 && obj._commit_pos < total_size - 1)
	{
		Internal_ProtoObjectWebSocket::buffer_t& cache = obj._cache;
		index_t idx = obj._commit_pos;
		if (idx < cache_size)
		{
			const index_t len = cache_size - idx;
			for (index_t i = 0; i < len; ++i, ++procbytes)
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
			for (const byte_t* p = readbuf + idx; idx < total_size; ++idx, ++procbytes)
			{
				cache.push_back(*p);
			}
		}
	}

	return PPR_AGAIN;
}

bool ProtoProcWebSocket::proc_ostrm(std::string& obuffer, const IProtocolObject& src)
{
	bool retval = true;

#if defined(DEBUG) || defined(_DEBUG)
	const ProtocolObjectWebSocket& obj = dynamic_cast<const ProtocolObjectWebSocket&>(src);
	// throw bad_cast if errored.
#else
	const ProtocolObjectWebSocket& obj = static_cast<const ProtocolObjectWebSocket&>(src);
#endif

	if (obj._ioframes.size() == 1)
	{ // currently, we only support responding with 1 frame.
		std::uint8_t header[20];
		unsigned short header_len = obj._ioframes.front().gen_header(header);

		obuffer.append((const char*)header, header_len);
		obuffer.append(obj._ioframes.front().data.begin(), obj._ioframes.front().data.end());
	}

	return retval;
}

bool ProtoProcWebSocket::proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj)
{
	return false;
}

WSParsingPhase ProtoProcWebSocket::_proc_istrm_byte(Internal_ProtoObjectWebSocket& obj, cbyte_ptr read_buf, cbyte_ptr p, index_t idx)
{
	WSParsingPhase retval = WSPP_Again;

	byte_t ch = *p;

	switch (obj._parsing_phase)
	{
	case WSPP_Start:
	case WSPP_FinRsvsOpcode:
		{
			WSMsgFrame empty_frame;
			obj._ioframes.push_back(empty_frame);
		}
		
		obj._ioframes.back().digest_fin_rsv_opcode(ch);

		if ((obj._ioframes.size() == 1 && obj._ioframes.back().opcode == WSO_Binary) ||
			(obj._ioframes.size() == 1 && obj._ioframes.back().opcode == WSO_Text) ||
			(obj._ioframes.size() > 1 && obj._ioframes.back().opcode == WSO_Continuation))
		{
			obj._parsing_phase = WSPP_MaskPayloadLen;
			obj._commit_pos = idx;
		}
		else if (obj._ioframes.back().opcode == WSO_Ping)
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
		obj._ioframes.back().digest_mask_payload(ch);
		switch (obj._ioframes.back().payloadlen_type())
		{
		case 0:
			if (obj._ioframes.back().mask)
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
			obj._ioframes.back().target_len = _MAKE_UINT16(
				obj._cache[0],
				obj._cache[1]
			);
			obj._cache.clear();

			if (obj._ioframes.back().mask)
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
			memcpy(obj._ioframes.back().mask_key,
				obj._cache.data(),
				4);
			obj._cache.clear();
			obj._parsing_phase = WSPP_Payload;
		}
		obj._commit_pos = idx;
		break;

	case WSPP_Payload:
		if (obj._ioframes.back().data.size() < obj._ioframes.back().target_len - 1)
		{
			obj._ioframes.back().data.push_back(ch);
			obj._commit_pos = idx;
		}
		else
		{
			obj._ioframes.back().data.push_back(ch);
			obj._commit_pos = idx;

			if (obj._ioframes.back().mask)
				obj._ioframes.back().decode_data();

			if (obj._ioframes.back().fin)
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

