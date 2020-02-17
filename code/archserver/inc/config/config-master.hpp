#pragma once

#include "pre-req.hpp"

namespace config
{

	struct MasterConfig
	{
		enum SockProtocol : int
		{
			SP_Unknown = 0,
			SP_TCP,
			SP_UDP
		};

		typedef struct _listen
		{
			_listen()
				: ipaddr("0.0.0.0")
				, sockproto(SP_Unknown)
				, backlog(0)
				, port(0)
			{}

			std::string		ipaddr;
			SockProtocol	sockproto;
			int				backlog;
			uint16_t		port;
		} listen_t;

		typedef struct _service_inst
		{
			uint16_t	inst_id;
		} service_inst_t;

		typedef struct _service
		{
			std::string name;
			std::string path;
			std::string workingdir;
			std::string	assembly;
			uint32_t	svc_id;
			std::vector<service_inst_t> insts;
		} service_t;


		static listen_t			listen;
		static std::vector<service_t>	services;

	};
}


