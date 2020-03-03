#pragma once

#include "pre-req.hpp"
#include "connection.hpp"
#include "ex-general.hpp"

namespace core
{

	template<class StreamT>
	class ConnMap
	{
		UNCOPYABLE(ConnMap);
		UNMOVABLE(ConnMap);

	public:
		typedef Connection<StreamT>	conn_t;

	public:
		ConnMap()
			: _top_conn_id(0)
		{}

		~ConnMap()
		{}

	public:
		conn_t* new_connection(archproto::ProtocolType default_procotol)
		{
			std::unique_lock<std::mutex>	lock;
			conn_id_t id = (++_top_conn_id) % max_conn_id;

			const auto& old_conn = _conns.find(id);
			if (old_conn == _conns.cend())
			{
				std::unique_ptr<conn_t> conn =
					std::make_unique<conn_t>(id, default_procotol);
				conn_t* retval = conn.get();
				_conns.insert({ id, std::move(conn) });
				return retval;
			}

			throw ArchException_ItemAlreadyExist();
		}

		void delete_connection(conn_id_t id)
		{
			_conns.erase(id);
		}

		conn_t* get_connection(conn_id_t id)
		{
			const auto& conn = _conns.find(id);
			if (conn != _conns.cend())
			{
				return conn->second.get();
			}
			else
			{
				return nullptr;
			}
		}

	private:
		conn_id_t	_top_conn_id;
		std::mutex	_top_conn_id_mtx;
		std::unordered_map<
			conn_id_t,
			std::unique_ptr<conn_t>> _conns;
	};

}

