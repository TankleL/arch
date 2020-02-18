#include "pipeclient.hpp"

using namespace core;

core::PipeClient::PipeClient(
			const std::shared_ptr<ProtocolQueue>& inque,
			const std::shared_ptr<ProtocolQueue>& outque)
	: _thread(std::bind(&PipeClient::_workthread, this))
	, _inque(inque)
	, _outque(outque)
	, _pipe_handle(*this)
	, _uvloop({})
{}

core::PipeClient::~PipeClient()
{}

void core::PipeClient::_workthread()
{
	uv_loop_init(&_uvloop);

	uv_pipe_init(&_uvloop, (uv_pipe_t*)& _pipe_handle, 1);


	uv_run(&_uvloop, UV_RUN_DEFAULT);
}




