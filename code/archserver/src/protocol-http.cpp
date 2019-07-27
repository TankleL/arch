#include "protocol-http.hpp"
#include "utilities.hpp"

using namespace std;
using namespace arch;

const int ProtoProcHttp_MaxContentLength = BYTES_SIZE_64K;


ProtoProcRet ProtoProcHttp::proc_istrm(IProtocolObject& dest, const uv_buf_t* uvbuffer, ssize_t uvreadlen)
{
	ProtoProcRet retval = PPR_AGAIN;

	Internal_ProtoObjectHttp& obj = static_cast<Internal_ProtoObjectHttp&>(dest);
	obj._commit_pos = 0;

	const byte_ptr read_buf = (byte_ptr)(uvbuffer->base);

	const index_t cache_size = (index_t)obj._cache.size();
	if (cache_size > 0)
	{
		byte_ptr p = obj._cache.data();
		for (index_t i = 0; i < cache_size && retval >= 0; ++i, ++p)
		{
			ParsingPhase status = _proc_istrm_byte(obj, read_buf, p, i);
			if (PPEnd != status)
			{
				retval = PPError != status ? PPR_AGAIN : PPR_ERROR;
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
			ParsingPhase status = _proc_istrm_byte(obj, read_buf, p, i + cache_size);
			if (PPEnd != status)
			{
				retval = PPError != status? PPR_AGAIN : PPR_ERROR;
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
		Internal_ProtoObjectHttp::buffer_t& cache = obj._cache;
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

	if (PPR_PULSE == retval)
	{

	}
	
	
	return retval;
}

ParsingPhase ProtoProcHttp::_proc_istrm_byte(Internal_ProtoObjectHttp& obj, byte_ptr read_buf, byte_ptr p, index_t idx)
{
	ParsingPhase retval = PPAGAIN;

	byte_t ch = *p;

	switch (obj._parsing_phase)
	{
	case PPStart:
		if (ch != ' ')
		{
			obj._parsing_phase = PPMethod;
			obj._commit_pos = idx;
		}
		break;

	case PPMethod:
		if (ch == ' ')
		{
			const index_t word_len = idx - obj._commit_pos;

			switch (word_len)
			{
			case 3:
				if(_proc_istrm_cmp(obj, read_buf, obj._commit_pos, (byte_ptr)"GET", 3))
				{
					obj.method = HMGET;
					obj._parsing_phase = PPPath;
					obj._commit_pos = idx;
				}
				else
				{
					retval = PPError;
				}
				break;

			case 4:
				if (_proc_istrm_cmp(obj, read_buf, obj._commit_pos, (byte_ptr)"POST", 4))
				{
					obj.method = HMPOST;
					obj._parsing_phase = PPPath;
					obj._commit_pos = idx;
				}
				else
				{
					retval = PPError;
				}
				break;
			}
		}
		break;

	case PPPath:
		if (ch == ' ')
		{
			if (idx > obj._commit_pos + 1)
			{
				const index_t len = idx - obj._commit_pos - 1;
				_proc_istrm_cpy(obj.path, obj, read_buf, obj._commit_pos + 1, len);
				
				obj._parsing_phase = PPVersion;
				obj._commit_pos = idx;
			}
			else
			{
				retval = PPError;
			}
		}
		break;

	case PPVersion:
		if (ch == '\n')
		{
			if (_proc_istrm_cmp(obj, read_buf, idx - 1, (byte_ptr)"\r", 1))
			{
				// TODO: parse the version

				obj._commit_pos = idx;
				obj._parsing_phase = PPHeader_Key;
			}
			else
			{
				retval = PPError;
			}
		}
		break;
		
	case PPHeader_Key:
		if (ch == ':')
		{
			if (idx > obj._commit_pos + 1)
			{
				const index_t len = idx - obj._commit_pos - 1;
				obj._cached_str.clear();
				_proc_istrm_cpy(obj._cached_str, obj, read_buf, obj._commit_pos + 1, len);

				obj._commit_pos = idx;
				obj._parsing_phase = PPHeader_Value;
			}
			else
			{
				retval = PPError;
			}
		}
		else if (ch == '\n' && idx > 0 && _proc_istrm_cmp(obj, read_buf, idx - 1, (byte_ptr)"\r", 1))
		{
			if (obj._commit_pos + 1 == idx - 1)
			{
				obj._commit_pos = idx;
				if (obj.content_length > 0)
				{
					obj._parsing_phase = PPContent;
				}
				else
				{
					obj._parsing_phase = PPStart;
					retval = PPEnd;
				}
			}
			else
			{
				retval = PPError;
			}
		}
		break;

	case PPHeader_Value:
		if (ch == '\n' && idx > 0 && _proc_istrm_cmp(obj, read_buf, idx - 1, (byte_ptr)"\r", 1))
		{
			if (idx > obj._commit_pos + 1)
			{
				const index_t len = idx - obj._commit_pos - 1;

				if (len > 1)
				{
					std::string value;
					_proc_istrm_cpy(value, obj, read_buf, obj._commit_pos + 1, len - 1);

					utils::trim(obj._cached_str);
					utils::trim(value);

					obj.headers.insert(std::make_pair(obj._cached_str, value));

					// detect content length
					if (obj.content_length == 0 && obj._cached_str == "Content-Length")
					{
						int clen = atoi(value.c_str());
						if (clen <= ProtoProcHttp_MaxContentLength)
						{
							obj.content_length = clen;
						}
						else
						{
							retval = PPError;
						}
					}

					obj._cached_str.clear();
				}
				else if (len == 2)
				{
					obj.headers.insert(std::make_pair(obj._cached_str, ""));
				}
				else
				{
					assert(false);
					retval = PPError;
				}

				obj._commit_pos = idx;
				obj._parsing_phase = PPHeader_Key;
			}
			else
			{
				retval = PPError;
			}
		}
		break;

	case PPContent:
		// TODO: implement this phase.
		assert(false);
		retval = PPError;
		break;

	default:
		//assert(false);
		// report error.
		retval = PPError;
		break;
	}

	return retval;
}

bool ProtoProcHttp::_proc_istrm_cmp(Internal_ProtoObjectHttp& obj, byte_ptr read_buf, index_t beg, byte_ptr pattern, index_t len)
{
	bool retval = true;

	const index_t cache_size = (index_t)obj._cache.size();
	index_t end = beg + len;

	if (beg >= cache_size)
	{
		const byte_ptr base = read_buf + beg - cache_size;
		retval = memcmp(base, pattern, len) == 0;
	}
	else if (beg < cache_size && end > cache_size)
	{
		index_t left_len = cache_size - beg;
		const byte_ptr left_base = obj._cache.data() + beg;
		retval = memcmp(left_base, pattern, left_len) == 0;

		if (retval)
		{
			index_t right_len = end - cache_size;
			const byte_ptr right_base = read_buf + beg - cache_size;
			retval = memcmp(right_base, pattern + left_len, right_len) == 0;
		}
	}
	else if (end <= cache_size)
	{
		const byte_ptr base = obj._cache.data() + beg;
		retval = memcmp(base, pattern, len) == 0;
	}
	else
	{
		assert(false);
	}

	return retval;
}

void ProtoProcHttp::_proc_istrm_cpy(std::string& dst, Internal_ProtoObjectHttp& obj, byte_ptr read_buf, index_t beg, index_t len)
{
	dst.clear();
	const index_t cache_size = (index_t)obj._cache.size();
	index_t end = beg + len;

	if (beg >= cache_size)
	{
		const byte_ptr base = read_buf + beg - cache_size;
		dst.assign((const char*)base, len);
	}
	else if (beg < cache_size && end > cache_size)
	{
		index_t left_len = cache_size - beg;
		const byte_ptr left_base = obj._cache.data() + beg;
		dst.assign((const char*)left_base, left_len);

		index_t right_len = end - cache_size;
		const byte_ptr right_base = read_buf + beg - cache_size;
		dst.append((const char*)right_base, right_len);
	}
	else if (end <= cache_size)
	{
		const byte_ptr base = obj._cache.data() + beg;
		dst.assign((const char*)base, len);
	}
	else
	{
		assert(false);
	}
}

bool ProtoProcHttp::proc_ostrm(std::string& obuffer, const IProtocolObject& src)
{
	const ProtocolObjectHttp& obj = static_cast<const ProtocolObjectHttp&>(src);

	char ch_buf[16];
	memset(ch_buf, 0, sizeof(ch_buf));

	obuffer.append("HTTP/1.1 ");
	obuffer.append(_itoa(obj.status_code, ch_buf, 10));
	obuffer.append(" OK\r\n");


	obuffer.append("\r\n");

	return true;
}

bool ProtoProcHttp::proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj)
{
	bool retval = false;
	const Internal_ProtoObjectHttp& sobj = static_cast<const Internal_ProtoObjectHttp&>(obj);

	const auto seg_connection = sobj.headers.find("Connection");
	if (sobj.headers.end() != seg_connection && "Upgrade" == seg_connection->second)
	{
		const auto seg_upgrade = sobj.headers.find("Upgrade");
		if (sobj.headers.end() != seg_upgrade)
		{
			if (seg_upgrade->second == "websocket")
			{
				const auto seg_wsa = sobj.headers.find("Sec-WebSocket-Accept");
				if (sobj.headers.end() != seg_wsa && seg_wsa->second.length() > 0)
				{
					retval = true;
					dest_proto = PT_WebSocket;
				}
			}
		}
	}

	return retval;
}

Internal_ProtoObjectHttp::Internal_ProtoObjectHttp()
	: _commit_pos(0)
	, _parsing_phase(ParsingPhase::PPStart)
{}

Internal_ProtoObjectHttp::Internal_ProtoObjectHttp(Internal_ProtoObjectHttp&& rhs) noexcept
	: ProtocolObjectHttp(std::move(rhs))
	, _cache(std::move(rhs._cache))
	, _cached_str(std::move(rhs._cached_str))
	, _commit_pos(rhs._commit_pos)
	, _parsing_phase(rhs._parsing_phase)
{}

Internal_ProtoObjectHttp& Internal_ProtoObjectHttp::operator=(Internal_ProtoObjectHttp&& rhs) noexcept
{
	ProtocolObjectHttp::operator=(std::move(rhs));
	_cache = std::move(rhs._cache);
	_cached_str = std::move(rhs._cached_str);
	_commit_pos = rhs._commit_pos;
	_parsing_phase = rhs._parsing_phase;
	return *this;
}

Internal_ProtoObjectHttp::~Internal_ProtoObjectHttp()
{}

IProtocolObject& Internal_ProtoObjectHttp::acquire(IProtocolObject& src) noexcept
{
#ifdef _DEBUG
	Internal_ProtoObjectHttp& obj = dynamic_cast<Internal_ProtoObjectHttp&>(src);
	assert(typeid(obj) == typeid(Internal_ProtoObjectHttp));
#else
	ProtoObjectHttp& obj = static_cast<ProtoObjectHttp&>(src);
#endif // _DEBUG

	ProtocolObjectHttp::acquire(src);
	_cache = std::move(obj._cache);
	return *this;
}

void Internal_ProtoObjectHttp::dispose() noexcept
{
	delete this;
}

