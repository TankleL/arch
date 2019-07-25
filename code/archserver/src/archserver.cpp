#include "archserver-prereq.hpp"
#include "config.hpp"

using namespace std;
using namespace arch;

int main(int argc, char** argv)
{
	config::load_config(argv[1]);

	return 0;
}