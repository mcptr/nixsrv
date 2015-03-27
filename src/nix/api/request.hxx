#ifndef NIX_API_REQUEST_HXX
#define NIX_API_REQUEST_HXX

#include <yami4-cpp/yami.h>

#include "nix/core/request.hxx"

namespace nix {
namespace api {

// FIXME: solve "dangling" yami (ifdefs on compilation time?)
typedef nix::core::Request<yami::incoming_message> Request_t;


} // api
} // nix

#endif
