#pragma once

#include "pre-req.hpp"
#include "protocol.hpp"

namespace core
{

	typedef uint16_t	conn_id_t;
	static const uint16_t max_conn_id = (uint16_t)-1;

	template<class StreamT>
	class Connection
	{
	public:
		Connection(
			conn_id_t id = 0,
			archproto::ProtocolType default_proto= PT_Unknown) noexcept
			: _id(id)
			, _tempproto(default_proto)
		{}

		void set_id(conn_id_t id) noexcept
		{
			_id = id;
		}

		conn_id_t get_id() const noexcept
		{
			return _id;
		}

		void set_stream(std::unique_ptr<StreamT>&& stream) noexcept
		{
			_stream = std::move(stream);
		}

		StreamT* get_stream() const noexcept
		{
			return _stream.get();
		}

		archproto::ProtocolType get_tempproto() const noexcept
		{
			return _tempproto;
		}

		void set_tempproto(const archproto::ProtocolType& proto) noexcept
		{
			_tempproto = proto;
		}

		void set_tempdata(std::unique_ptr<archproto::IProtocolData>&& tempdata) noexcept
		{
			_tempdata = std::move(tempdata);
		}

		bool has_tempdata() const noexcept
		{
			return _tempdata != nullptr;
		}

		archproto::IProtocolData& get_tempdata() const noexcept
		{
			return *(_tempdata.get());
		}

		std::unique_ptr<archproto::IProtocolData>&& acquire_tempdata() noexcept
		{
			return std::move(_tempdata);
		}

	private:
		std::unique_ptr<StreamT>					_stream;
		std::unique_ptr<archproto::IProtocolData>	_tempdata;
		archproto::ProtocolType						_tempproto;
		conn_id_t									_id;
	};

}


