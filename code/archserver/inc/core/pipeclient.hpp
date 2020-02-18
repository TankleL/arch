#pragma once

#include "pre-req.hpp"
#include "protocol-queue.hpp"
#include "uv.h"

namespace core
{

	class PipeClient
	{
		UNCOPYABLE(PipeClient);
		UNMOVABLE(PipeClient);
	public:
		typedef struct _pipe_t : public uv_pipe_t
		{
			_pipe_t(PipeClient& pipeclient)
				: pipcli(pipeclient)
			{}

			PipeClient& pipcli;
		} pipe_t;

	public:
		PipeClient(
			const std::shared_ptr<ProtocolQueue>& inque,
			const std::shared_ptr<ProtocolQueue>& outque);
		~PipeClient();

	private:
		void _workthread();

	private:
		std::thread		_thread;
		std::shared_ptr<ProtocolQueue>	_inque;
		std::shared_ptr<ProtocolQueue>	_outque;

		pipe_t			_pipe_handle;
		uv_loop_t		_uvloop;
	};


}






