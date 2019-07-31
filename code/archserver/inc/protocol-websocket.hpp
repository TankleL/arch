#pragma once

#include "archserver-prereq.hpp"
#include "protocols.hpp"
#include "protocolproc.hpp"

namespace arch
{

	enum WSParsingPhase : int
	{
		WSPP_Start = 0,
		WSPP_Again,

		WSPP_FinRsvsOpcode,
		WSPP_MaskPayloadLen,
		WSPP_ExtPayloadLen16,
		WSPP_ExtPayloadLen64,
		WSPP_MaskingKey,
		WSPP_Payload,

		WSPP_Ping,
		WSPP_Pong,
		WSPP_End,
		WSPP_Error
	};

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

	class Internal_ProtoObjectWebSocket;
	class ProtoProcWebSocket : public IProtocolProc
	{
	public:
		virtual ProtoProcRet proc_istrm(IProtocolObject& dest, const uv_buf_t* uvbuffer, ssize_t uvreadlen) override;
		virtual bool proc_ostrm(std::string& obuffer, const IProtocolObject& src) override;
		virtual bool proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj) override;

	protected:
		static WSParsingPhase _proc_istrm_byte(Internal_ProtoObjectWebSocket& obj, byte_ptr read_buf, byte_ptr p, index_t idx);
		
	};
		
	class Internal_ProtoObjectWebSocket
		: public ProtocolObjectWebSocket
	{
		friend ProtoProcWebSocket;

	private:
		typedef std::vector<byte_t>				buffer_t;
		typedef std::vector<WSMsgFrame>			framelist_t;

	private:
		framelist_t				_frames;
		buffer_t				_cache;

		index_t					_commit_pos;
		WSParsingPhase			_parsing_phase;
	};

}


