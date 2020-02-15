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
			ProtocolType default_protocol = PT_Unknown) noexcept
			: _id(id)
			, _iapp_protocol(default_protocol)
			, _oapp_protocol(default_protocol)
			, _closing(false)
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

		void set_closing(bool closing) noexcept
		{
			_closing = closing;
		}

		bool get_closing() const noexcept
		{
			return _closing;
		}

		void set_iapp_protocol(const ProtocolType& aptype) noexcept
		{
			_iapp_protocol = aptype;
		}

		ProtocolType get_iapp_protocol() const noexcept
		{
			return _iapp_protocol;
		}

		void set_oapp_protocol(const ProtocolType& aptype) noexcept
		{
			_oapp_protocol = aptype;
		}

		ProtocolType get_oapp_protocol() const noexcept
		{
			return _oapp_protocol;
		}

		void set_app_protocol_data(std::unique_ptr<IProtocolData>&& protocol_data) noexcept
		{
			_app_prot_data = std::move(protocol_data);
		}

		IProtocolData* get_app_protocol_data() const noexcept
		{
			return _app_prot_data.get();
		}

	private:
		std::unique_ptr<StreamT>		_stream;
		std::unique_ptr<IProtocolData>	_app_prot_data;
		conn_id_t		_id;
		ProtocolType	_iapp_protocol;
		ProtocolType	_oapp_protocol;
		bool			_closing;
	};

}


