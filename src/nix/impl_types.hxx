#ifndef NIX_IMPL_TYPES_HXX
#define NIX_IMPL_TYPES_HXX

#include "transport/yami.hxx"


namespace nix {
namespace impl {


// default transport implementaion
typedef nix::transport::YAMIServer ServerTransport_t;
typedef nix::transport::YAMIClient ClientTransport_t;
typedef nix::transport::YAMIRequest Request_t;


} // impl
} // nix


#endif
