#pragma once

#include <map>
#include <vector>
#include <cassert>
#include "acquirable.hpp"
#include "disposable.hpp"

namespace arch
{
	/******************************************************************/
	/*                    ------ Definition ------                    */
	/*                          ProtocolType                          */
	/******************************************************************/

	enum ProtocolType : int
	{
		PT_Http = 0,
		PT_WebSocket = 1,
		PT_Arch,

		// < ----------- insert new types here.

		PT_ProtoTypesNum,
		PT_Unknown = -1
	};

	/******************************************************************/
	/*                   ------ Declaration ------                    */
	/*                        IProtocolObject                         */
	/******************************************************************/

	class IProtocolObject
		: public IDisposable
		, public IAcquriable<IProtocolObject>
	{
	public:
		virtual ProtocolType	get_protocol_type() const noexcept = 0;
	};

	/******************************************************************/
	/*                   ------ Declaration ------                    */
	/*                      Protocol HTTP Object                      */
	/******************************************************************/

	enum HttpMethod : int
	{
		HMUnknown,
		HMGET,
		HMHEAD,
		HMPOST,
		HMPUT
	};

	enum HttpVersion : int
	{
		HVUnknown,
		HVV1,
		HVV1_1,
		HVV2
	};

	class ProtocolObjectHttp
		: public IProtocolObject
	{
	public:
		ProtocolObjectHttp(const ProtocolObjectHttp& rhs) = delete;
		ProtocolObjectHttp& operator=(const ProtocolObjectHttp& rhs) = delete;

	public:
		ProtocolObjectHttp();
		ProtocolObjectHttp(ProtocolObjectHttp&& rhs) noexcept;
		virtual ~ProtocolObjectHttp();

		ProtocolObjectHttp& operator=(ProtocolObjectHttp&& rhs) noexcept;

	public:
		virtual ProtocolType	get_protocol_type() const noexcept override	{ return PT_Http; }
		IProtocolObject&		acquire(IProtocolObject& src) noexcept override;
		void					dispose() noexcept override;

	public:
		typedef std::map<std::string, std::string>	header_list_t;

	public:
		HttpMethod			method;
		HttpVersion			version;
		header_list_t		headers;
		std::string			path;
		int					status_code;
		int					content_length;
		std::string			content;
		std::string			status_msg;
	};

	/******************************************************************/
	/*                   ------ Declaration ------                    */
	/*                   Protocol WebSocket Object                    */
	/******************************************************************/

	enum WSOpcode : std::uint8_t
	{
		WSO_Continuation = 0,
		WSO_Text = 1,
		WSO_Binary = 2,
		WSO_Reserved3, WSO_Reserved4, WSO_Reserved5, WSO_Reserved6, WSO_Reserved7,
		WSO_Close = 8,
		WSO_Ping = 9,
		WSO_Pong = 10,
		WSO_ReservedB, WSO_ReservedC, WSO_ReservedD, WSO_WSO_ReservedE, WSO_ReservedF,
	};


	//		   0                   1                   2                   3
	//		   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	//		  +-+-+-+-+-------+-+-------------+-------------------------------+
	//		  |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
	//		  |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
	//		  |N|V|V|V|       |S|             |   (if payload len==126/127)   |
	//		  | |1|2|3|       |K|             |                               |
	//		  +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
	//		  |     Extended payload length continued, if payload len == 127  |
	//		  + - - - - - - - - - - - - - - - +-------------------------------+
	//		  |                               |Masking-key, if MASK set to 1  |
	//		  +-------------------------------+-------------------------------+
	//		  | Masking-key (continued)       |          Payload Data         |
	//		  +-------------------------------- - - - - - - - - - - - - - - - +
	//		  :                     Payload Data continued ...                :
	//		  + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
	//		  |                     Payload Data continued ...                |
	//		  +---------------------------------------------------------------+
	typedef struct WSMsgFrame
	{
	public:
		std::vector<unsigned char>	data;
		std::uint64_t				target_len;
		std::uint8_t				mask_key[4];
		WSOpcode					opcode;
		bool						fin;
		bool						mask;

	public:
		WSMsgFrame();

		void digest_fin_rsv_opcode(std::uint8_t data);
		void digest_mask_payload(std::uint8_t data);
		void decode_data();
		void encode_data();
		void encode_data(unsigned char* dst_data, unsigned short len);
		void gen_mask_key();
		unsigned short gen_header(uint8_t* out) const;
		std::uint8_t gen_fin_rsv_opcode() const;
		std::uint8_t gen_mask_payload() const;
		void gen_ext_payload1(std::uint8_t& low, std::uint8_t& high) const;

		int payloadlen_type() const;
	} msg_frame_t;

	class ProtocolObjectWebSocket
		: public IProtocolObject
	{
	public:
		ProtocolObjectWebSocket(const ProtocolObjectWebSocket& rhs) = delete;
		ProtocolObjectWebSocket& operator=(const ProtocolObjectWebSocket& rhs) = delete;

	public:
		ProtocolObjectWebSocket();
		ProtocolObjectWebSocket(ProtocolObjectWebSocket&& rhs) noexcept;
		virtual ~ProtocolObjectWebSocket();

		ProtocolObjectWebSocket& operator=(ProtocolObjectWebSocket&& rhs) noexcept;

	public:
		virtual ProtocolType	get_protocol_type() const noexcept override { return PT_WebSocket; }
		IProtocolObject&		acquire(IProtocolObject& src) noexcept override;
		void					dispose() noexcept override;

	public:
		typedef std::vector<WSMsgFrame>			framelist_t;

	public:
		framelist_t				_ioframes;
		std::uint32_t			_header_offset;
	};


	/******************************************************************/
	/*                   ------ Declaration ------                    */
	/*                      Protocol Arch Object                      */
	/******************************************************************/
	enum ArchProtocolVersion : int
	{
		APV_Unknown,
		APV_0_1		// arch protocol 0.1
	};

	class ProtocolObjectArch
		: public IProtocolObject
	{
	public:
		ProtocolObjectArch(const ProtocolObjectArch& rhs) = delete;
		ProtocolObjectArch& operator=(const ProtocolObjectArch& rhs) = delete;

	public:
		ProtocolObjectArch();
		ProtocolObjectArch(ProtocolObjectArch&& rhs) noexcept;
		virtual ~ProtocolObjectArch();

		ProtocolObjectArch& operator=(ProtocolObjectArch&& rhs) noexcept;

	public:
		virtual ProtocolType	get_protocol_type() const noexcept override { return PT_Arch; }
		IProtocolObject&		acquire(IProtocolObject& src) noexcept override;
		void					dispose() noexcept override;

	public:
		uint32_t				version;
		std::vector<uint8_t>	data;
	};

	/******************************************************************/
	/*                    ------ Definition ------                    */
	/*                       Protocol HTTP Object                     */
	/******************************************************************/
	inline ProtocolObjectHttp::ProtocolObjectHttp()
		: method(HMUnknown)
		, version(HVUnknown)
		, status_code(0)
		, content_length(0)
	{}

	inline ProtocolObjectHttp::ProtocolObjectHttp(ProtocolObjectHttp&& rhs) noexcept
		: method(rhs.method)
		, version(rhs.version)
		, headers(std::move(rhs.headers))
		, path(std::move(rhs.path))
		, status_code(rhs.status_code)
		, content_length(rhs.content_length)
		, content(std::move(rhs.content))
		, status_msg(std::move(rhs.status_msg))
	{}

	inline ProtocolObjectHttp::~ProtocolObjectHttp()
	{}

	inline ProtocolObjectHttp& ProtocolObjectHttp::operator=(ProtocolObjectHttp&& rhs) noexcept
	{
		method = rhs.method;
		version = rhs.version;
		headers = std::move(rhs.headers);
		path = std::move(rhs.path);
		status_code = rhs.status_code;
		content_length = rhs.content_length;
		content = std::move(rhs.content);
		status_msg = std::move(rhs.status_msg);

		return *this;
	}

	inline IProtocolObject& ProtocolObjectHttp::acquire(IProtocolObject& src) noexcept
	{
		ProtocolObjectHttp& obj = static_cast<ProtocolObjectHttp&>(src);
		method = obj.method;
		version = obj.version;
		status_code = obj.status_code;
		content_length = obj.content_length;

		headers = std::move(obj.headers);
		path = std::move(obj.path);
		content = std::move(obj.content);
		status_msg = std::move(obj.status_msg);

		return *this;
	}

	inline void ProtocolObjectHttp::dispose() noexcept
	{
		delete this;
	}



	/******************************************************************/
	/*                    ------ Definition ------                    */
	/*                   Protocol WebSocket Object                    */
	/******************************************************************/

	inline ProtocolObjectWebSocket::ProtocolObjectWebSocket()
		: _header_offset(4)
	{}

	inline ProtocolObjectWebSocket::ProtocolObjectWebSocket(ProtocolObjectWebSocket&& rhs) noexcept
		: _header_offset(rhs._header_offset)
	{}

	inline ProtocolObjectWebSocket::~ProtocolObjectWebSocket()
	{}

	inline ProtocolObjectWebSocket& ProtocolObjectWebSocket::operator=(ProtocolObjectWebSocket&& rhs) noexcept
	{
		_header_offset = rhs._header_offset;
		return *this;
	}

	inline IProtocolObject& ProtocolObjectWebSocket::acquire(IProtocolObject& src) noexcept
	{
		ProtocolObjectWebSocket& obj = static_cast<ProtocolObjectWebSocket&>(src);
		_header_offset = obj._header_offset;
		return *this;
	}

	inline void ProtocolObjectWebSocket::dispose() noexcept
	{
		delete this;
	}


	inline WSMsgFrame::WSMsgFrame()
		: target_len(0)
		, mask_key{ 0 }
		, opcode(WSO_Close)
		, fin(false)
		, mask(false)
	{}

	inline void WSMsgFrame::digest_fin_rsv_opcode(std::uint8_t data)
	{
		fin = (data >> 7) == 1;
		opcode = (WSOpcode)(data & 0x0f);
	}

	inline void WSMsgFrame::digest_mask_payload(std::uint8_t data)
	{
		mask = (data >> 7) == 1;
		target_len = (data & 0x7f);
	}

	inline std::uint8_t WSMsgFrame::gen_fin_rsv_opcode() const
	{
		return (fin << 7) | opcode;
	}

	inline std::uint8_t WSMsgFrame::gen_mask_payload() const
	{
		if (target_len < 0x7e)
		{
			return (((int)mask) << 7) | ((std::uint8_t)target_len);
		}
		else if (target_len >= 0x7e)
		{
			return (((int)mask) << 7) | 0x7e;
		}
		else
		{
			assert(false);
			return (((int)mask) << 7);
		}
	}

	inline void WSMsgFrame::gen_ext_payload1(std::uint8_t & low, std::uint8_t & high) const
	{
		low = ((std::uint16_t)target_len) & 0xff00;
		high = ((std::uint16_t)target_len) & 0x00ff;
	}

	inline unsigned short WSMsgFrame::gen_header(uint8_t * out) const
	{
		int idx = 0;

		out[idx++] = gen_fin_rsv_opcode();
		out[idx++] = gen_mask_payload();

		if (target_len >= 0x7e && target_len <= 0xffff)
		{
			gen_ext_payload1(out[idx++], out[idx++]);
		}
		else if (target_len > 0xffff)
		{
			assert(false);
			// Sorry, we DO NOT support the payload whose length is longer than 65536.
		}

		if (mask)
		{
			memcpy(out + idx, mask_key, 4);
			idx += 4;
		}

		return idx;
	}

	inline int WSMsgFrame::payloadlen_type() const
	{
		if (target_len < 0x7e)
		{
			return 0;	// normal type, no need to read ext payload len.
		}
		else if (target_len == 0x7e)
		{
			return 16;	// read next 16bits and interpret as the ext payload len.
		}
		else
		{
			return 64; // read next 64bits and interpret as the ext payload len.
		}
	}

	inline void WSMsgFrame::decode_data()
	{
		for (size_t i = 0; i < data.size(); ++i)
		{
			data[i] = data[i] ^ mask_key[i % 4];
		}
	}

	inline void WSMsgFrame::encode_data()
	{
		for (size_t i = 0; i < data.size(); ++i)
		{
			data[i] = data[i] ^ mask_key[i % 4];
		}
	}

	inline void WSMsgFrame::encode_data(unsigned char* dst_data, unsigned short len)
	{
		for (int i = 0; i < len; ++i)
		{
			dst_data[i] = dst_data[i] ^ mask_key[i % 4];
		}
	}

	inline void WSMsgFrame::gen_mask_key()
	{
		static std::uint8_t mk0 = 0x00;
		static std::uint8_t mk1 = 0x3f;
		static std::uint8_t mk2 = 0x7f;
		static std::uint8_t mk3 = 0xff;

		mask_key[0] = ++mk0;
		mask_key[1] = ++mk1;
		mask_key[2] = ++mk2;
		mask_key[3] = ++mk3;
	}

	/******************************************************************/
	/*                    ------ Definition ------                    */
	/*                      Protocol Arch Object                      */
	/******************************************************************/
	inline ProtocolObjectArch::ProtocolObjectArch()
		: version(APV_Unknown)
	{}

	inline ProtocolObjectArch::ProtocolObjectArch(ProtocolObjectArch&& rhs) noexcept
		: version(rhs.version)
		, data(std::move(rhs.data))
	{
		rhs.version = APV_Unknown;
	}

	inline ProtocolObjectArch::~ProtocolObjectArch()
	{}

	inline ProtocolObjectArch& ProtocolObjectArch::operator=(ProtocolObjectArch&& rhs) noexcept
	{
		version = rhs.version;
		data = std::move(rhs.data);

		rhs.version = APV_Unknown;
		return *this;
	}

	inline IProtocolObject& ProtocolObjectArch::acquire(IProtocolObject& src) noexcept
	{
		ProtocolObjectArch& srcobj = static_cast<ProtocolObjectArch&>(src);
		version = srcobj.version;
		data = std::move(srcobj.data);

		srcobj.version = APV_Unknown;

		return *this;
	}

	inline void ProtocolObjectArch::dispose() noexcept
	{
		delete this;
	}
}


