#pragma once

#include "archserver-prereq.hpp"
#include "protocols.hpp"
#include "protocolproc.hpp"

namespace arch
{
	enum ParsingPhase : int
	{
		PPStart = 0,
		PPAGAIN,
		PPEnd,

		PPMethod,
		PPPath,
		PPVersion,
		PPHeader_Key,
		PPHeader_Value,
		PPContent,
			
		PPError
	};

	class Internal_ProtoObjectHttp;
	class ProtoProcHttp : public IProtocolProc
	{
	public:
		virtual ProtoProcRet proc_istrm(IProtocolObject& dest, const uv_buf_t* uvbuffer, ssize_t uvreadlen) override;
		virtual bool proc_ostrm(std::string& obuffer, const IProtocolObject& src) override;
		virtual bool proc_check_switch(ProtocolType& dest_proto, const IProtocolObject& obj) override;

	protected:
		static ParsingPhase _proc_istrm_byte(Internal_ProtoObjectHttp& obj, byte_ptr read_buf, byte_ptr p, index_t idx);
		static bool _proc_istrm_cmp(Internal_ProtoObjectHttp& obj, byte_ptr read_buf, index_t beg, byte_ptr pattern, index_t len);
		static void _proc_istrm_cpy(std::string& dst, Internal_ProtoObjectHttp& obj, byte_ptr read_buf, index_t beg, index_t len);
	};



	class Internal_ProtoObjectHttp 
		: public ProtocolObjectHttp
	{
		friend ProtoProcHttp;
	public:
		Internal_ProtoObjectHttp(const Internal_ProtoObjectHttp&) = delete;
		Internal_ProtoObjectHttp& operator=(const Internal_ProtoObjectHttp& rhs) = delete;

	public:
		Internal_ProtoObjectHttp();
		Internal_ProtoObjectHttp(Internal_ProtoObjectHttp&& rhs) noexcept;
		virtual ~Internal_ProtoObjectHttp();

		Internal_ProtoObjectHttp& operator=(Internal_ProtoObjectHttp&& rhs) noexcept;

	public:
		virtual IProtocolObject& acquire(IProtocolObject& src) noexcept override;
		virtual void dispose() noexcept override;

	private:
		typedef std::vector<byte_t>					buffer_t;
	
	private:
		buffer_t				_cache;

		std::string				_cached_str;
		index_t					_commit_pos;
		int						_parsing_phase;
	};

}


