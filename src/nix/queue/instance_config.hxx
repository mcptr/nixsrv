#ifndef NIX_QUEUE_INSTANCE_CONFIG_HXX
#define NIX_QUEUE_INSTANCE_CONFIG_HXX

#include <string>

namespace nix {
namespace queue {


class InstanceConfig
{
public:
	std::string name;
	size_t size;
};


} // queue
} // nix

#endif
