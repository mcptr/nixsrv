#ifndef NIX_API_REQUEST_HXX
#define NIX_API_REQUEST_HXX

#include <yami4-cpp/yami.h>

#include "nix/request.hxx"
#include "nix/response.hxx"
#include "nix/module.hxx"

namespace nix {
namespace api {

// FIXME: solve exposing yami (ifdefs on compilation time?)
typedef nix::Request<yami::incoming_message> Request_t;
typedef nix::Response Response_t;

} // api
} // nix

#endif
