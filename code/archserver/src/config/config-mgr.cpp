#include "config-mgr.hpp"
#include "config-master.hpp"
#include "tinyxml2.h"
#include "ex-general.hpp"

using namespace config;
using namespace tinyxml2;

void config::ConfigMgr::load_configs(const std::string& configdir)
{
	_load_master_config(configdir + "/config-master.xml");
}

void config::ConfigMgr::_load_master_config(const std::string& filename)
{
	XMLDocument	doc;
	if (XML_SUCCESS == doc.LoadFile(filename.c_str()))
	{
		try
		{
			const auto& xroot = doc.RootElement();
			const auto& xlisten = xroot->LastChildElement("Listen");
			if (xlisten)
				_load_master_cfg_listen(xlisten);

			const auto& xsvcs = xroot->LastChildElement("Services");
			if (xsvcs)
			{
				auto xsvc = xsvcs->FirstChildElement();
				while (xsvc)
				{
					if (strcmp(xsvc->Name(), "Service") == 0)
					{
						_load_master_cfg_service(xsvc);
					}
					xsvc = xsvc->NextSiblingElement();
				}
			}
		}
		catch (std::exception ex)
		{
			//TODO: handle exceptions
			throw ex;
		}
	}
	else
	{
		throw ArchException_LoadConfigFailed();
	}
}

void config::ConfigMgr::_load_master_cfg_listen(const void* xelement)
{
	const XMLElement& xlisten = *static_cast<const XMLElement*>(xelement);

	MasterConfig::listen_t& listen = MasterConfig::listen;

	{
		const auto& xsockproto = xlisten.LastChildElement("SockProtocol");
		if (strcmp(xsockproto->GetText(), "TCP") == 0)
		{
			listen.sockproto = MasterConfig::SP_TCP;
		}
		else if (strcmp(xsockproto->GetText(), "UDP") == 0)
		{
			listen.sockproto = MasterConfig::SP_UDP;
		}
	}

	{
		const auto& xipaddr = xlisten.LastChildElement("IPAddress");
		listen.ipaddr = xipaddr->GetText();
	}

	{
		const auto& xport = xlisten.LastChildElement("Port");
		listen.port = (uint16_t)xport->IntText();
	}

	{
		const auto& xbacklog = xlisten.LastChildElement("Backlog");
		listen.backlog = xbacklog->IntText();
	}
}

void config::ConfigMgr::_load_master_cfg_service(const void* xelement)
{
	const XMLElement& xsvcs = *static_cast<const XMLElement*>(xelement);

	MasterConfig::service_t service;

	{
		const auto& xname = xsvcs.LastChildElement("Name");
		service.name = xname->GetText();
	}

	{
		const auto& xpath = xsvcs.LastChildElement("Path");
		_parse_raw_path(service.path, xpath->GetText());
	}
	
	{
		const auto& xwd = xsvcs.LastChildElement("WorkingDir");
		_parse_raw_path(service.workingdir, xwd->GetText());
	}

	{
		const auto& xasm = xsvcs.LastChildElement("Assembly");
		service.assembly = xasm->GetText();
	}

	{
		const auto& xid = xsvcs.LastChildElement("ServiceID");
		service.svc_id = xid->UnsignedText();
	}

	{
		const auto& xinsts = xsvcs.LastChildElement("Instances");
		auto xinst = xinsts->FirstChildElement();
		while (xinst)
		{
			if (strcmp(xinst->Name(), "Instance") == 0)
			{
				_load_master_cfg_service_inst(&service.insts, xinst);
			}
			xinst = xinst->NextSiblingElement();
		}
	}

	MasterConfig::services.push_back(service);
}

void config::ConfigMgr::_load_master_cfg_service_inst(void* insts, const void* xelement)
{
	std::vector<MasterConfig::service_inst_t>& svc_insts =
		*static_cast<std::vector<MasterConfig::service_inst_t>*>(insts);
	const XMLElement& xinst = *static_cast<const XMLElement*>(xelement);

	MasterConfig::service_inst_t	svc_inst;
	
	{
		const auto& xid = xinst.LastChildElement("InstanceID");
		svc_inst.inst_id = (uint16_t)xid->IntText();
	}

	svc_insts.push_back(svc_inst);
}

void config::ConfigMgr::_parse_raw_path(std::string& res, const std::string& raw)
{
	std::regex reg_server_root("\\$\\{SERVER_ROOT\\}");
	res = std::regex_replace(raw, reg_server_root, MasterConfig::server_root);
}




