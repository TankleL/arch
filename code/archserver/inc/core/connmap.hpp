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

	public:
		typedef Connection<StreamT>	conn_t;

	public:
		ConnMap()
			: _top_conn_id(0)
		{}

		~ConnMap()
		{}

	public:
		std::weak_ptr<conn_t> new_connection()
		{
			std::unique_lock<std::mutex>	lock;
			conn_id_t id = (_top_conn_id++) % max_conn_id;

			const auto& old_conn = _conns.find(id);
			if (old_conn == _conns.cend())
			{
				std::shared_ptr<conn_t> conn =
					std::make_shared<conn_t>(id);
				_conns.insert({ id, conn });

				return conn;
			}

			throw ArchException_ItemAlreadyExist();
		}

		void delete_connection(conn_id_t id)
		{
			_conns.erase(id);
		}

	private:
		conn_id_t	_top_conn_id;
		std::mutex	_top_conn_id_mtx;
		std::unordered_map<
			conn_id_t,
			std::shared_ptr<conn_t>> _conns;
	};

}

