#include "direct_handlers.hxx"
#include <iostream>

namespace nix {
namespace direct_handlers {


void echo(yami::incoming_message& im)
{
	im.reply(im.get_parameters());
}


} // direct_handlers
} // nix
