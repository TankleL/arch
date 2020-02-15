#pragma once

#include "pre-req.hpp"
#include "protocol-queue.hpp"
#include "uv.h"

namespace core
{

	class PipeClient
	{
		UNCOPYABLE(PipeClient);
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
			ProtocolQueue& inque,
			ProtocolQueue& outque);
		~PipeClient();

	private:
		void _workthread();

	private:
		std::thread		_thread;
		ProtocolQueue&	_inque;
		ProtocolQueue&	_outque;

		pipe_t			_pipe_handle;
		uv_loop_t		_uvloop;
	};


}






