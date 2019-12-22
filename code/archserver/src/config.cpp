#include "config.hpp"
#include "tinyxml2.h"
#include "protocols.hpp"

using namespace std;
using namespace arch;
using namespace tinyxml2;

std::unordered_map<std::string, std::string>		arch::config::global_keyvalues;
arch::config::server_phase_t						arch::config::server_phase;
arch::config::module_phase_t						arch::config::module_phase;

namespace arch
{
	namespace config
	{
		class _ConfigXmlVisitor : public XMLVisitor
		{
		public:
			/// Visit an element.
			virtual bool VisitEnter(const XMLElement& element, const XMLAttribute* firstAttribute) override 
			{
				bool retval = true;

				_push_visitphase(element.Name());

				switch (_current_visitphase())
				{
				case VP_None:
					break;

				case VP_ArchServer_Config:
					break;

				case VP_Global_KeyValue:
					break;

				case VP_ServerPhase:
					retval = _parse_server_phase(element);
					break;

				case VP_ModulePhase:
					retval = _parse_module_phase(element);
					break;

				default:
					break;
				}

				return retval;
			}
			/// Visit an element.
			virtual bool VisitExit(const XMLElement& element) override 
			{
				_pop_visitphase(element.Name());
				return true;
			}

		public:
			_ConfigXmlVisitor()
			{}

		private:
			enum VisitPhase
			{
				VP_None,
				VP_ArchServer_Config,
				VP_Global_KeyValue,
				VP_ServerPhase,
				VP_ModulePhase
			};

		private:
			VisitPhase _current_visitphase()
			{
				if (_vp_stack.size() > 0)
				{
					return _vp_stack.top();
				}
				else
				{
					return VP_None;
				}
			}

			void _push_visitphase(const char* element_name)
			{
				if (strcmp(element_name, "ArchServer_Config") == 0)
				{
					_vp_stack.push(VP_ArchServer_Config);
				}
				else if (strcmp(element_name, "Global_KeyValue") == 0)
				{
					_vp_stack.push(VP_Global_KeyValue);
				}
				else if (strcmp(element_name, "ServerPhase") == 0)
				{
					_vp_stack.push(VP_ServerPhase);
				}
				else if (strcmp(element_name, "ModulePhase") == 0)
				{
					_vp_stack.push(VP_ModulePhase);
				}
			}

			void _pop_visitphase(const char* element_name)
			{
				if (strcmp(element_name, "ArchServer_Config") == 0 ||
					strcmp(element_name, "Global_KeyValue") == 0 ||
					strcmp(element_name, "ServerPhase") == 0 ||
					strcmp(element_name, "ModulePhase") == 0)
				{
					if (_vp_stack.size() > 0)
					{
						_vp_stack.pop();
					}
					else
					{
						assert(false);
					}
				}
			}

			bool _parse_server_phase(const XMLElement& element)
			{
				const char* name = element.Name();

				if (strcmp(name, "IPAddress") == 0)
				{
					server_phase.ipaddr = element.GetText();
				}
				else if (strcmp(name, "Port") == 0)
				{
					server_phase.port = element.IntText();
				}
				else if (strcmp(name, "Backlog") == 0)
				{ 
					server_phase.backlog = element.IntText();
				}
				else if (strcmp(name, "CoreNumber") == 0)
				{
					server_phase.core_num = element.IntText();
				}
				else if (strcmp(name, "Max_Living_Connections") == 0)
				{
					server_phase.max_living_conn = element.IntText();
				}
				else if (strcmp(name, "Max_ArchMessage_Queue_Length") == 0)
				{
					server_phase.max_archmsg_queue_len = element.IntText();
				}
				else if (strcmp(name, "Protocol") == 0)
				{
					server_phase.protocol = protocol_name_to_val(element.GetText());
				}

				return true;
			}

			bool _parse_module_phase(const XMLElement& element)
			{
				const char* name = element.Name();

				if (strcmp(name, "RootPath") == 0)
				{
					module_phase.root_path = element.GetText();
#if defined(_DEBUG) || defined(DEBUG)
					module_phase.root_path = std::regex_replace(
						module_phase.root_path,
						std::regex("\\$\\{TEST_ROOT_PATH\\}"),
						TEST_PATH_RES_MODULES_ROOT);
#endif
				}
				else if (strcmp(name, "Module") == 0)
				{
					module_t mdl;
					
					const XMLElement* ele_name = element.FirstChildElement("Name");
					if (ele_name)	mdl.name = ele_name->GetText();
					else			return false;

					const XMLElement* ele_binname = element.FirstChildElement("BinName");
					if (ele_name)	mdl.bin_name = ele_binname->GetText();
					else			return false;

					const XMLElement* ele_binding_proto = element.FirstChildElement("Binding_Protocoal");
					if (ele_binding_proto)	mdl.proto_type = protocol_name_to_val(ele_binding_proto->GetText());
					else					return false;

					module_phase.modules[mdl.proto_type] = mdl;
				}

				return true;
			}

		private:
			std::stack<VisitPhase>	_vp_stack;
		};
	}
}

bool arch::config::load_config(const std::string& configfile)
{
	bool retval = true;

	XMLDocument	doc;
	doc.LoadFile(configfile.c_str());
	
	_ConfigXmlVisitor xv;
	retval = doc.Accept(&xv);

	return retval;
}

int arch::config::protocol_name_to_val(const std::string& pname)
{
	static std::map<std::string, int> map_name2val(
		{ 
			{"http", PT_Http},
			{"websocket", PT_WebSocket},
			{"arch", PT_Arch}
		});

	int res = PT_Http;

	const auto& val = map_name2val.find(pname);
	if (val != map_name2val.end())
	{
		res = val->second;
	}
	else
	{
		throw std::runtime_error("Protocol not found.");
	}
	
	return res;
}
