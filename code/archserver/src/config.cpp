#include "config.hpp"
#include "tinyxml2.h"

using namespace std;
using namespace arch;

bool arch::config::load_config(const std::string& configfile)
{
	bool retval = true;

	tinyxml2::XMLDocument	doc;
	doc.LoadFile(configfile.c_str());
	

	return retval;
}
