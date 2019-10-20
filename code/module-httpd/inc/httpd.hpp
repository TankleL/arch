#pragma once

#include "libam.hpp"

class ModuleHTTPD : public libam::IArchModule
{
public:
	virtual int		Init() override;
	virtual void	Uninit() override;
	virtual	void	service_processor(arch::ArchMessage& onode, const arch::ArchMessage& inode) override;
};