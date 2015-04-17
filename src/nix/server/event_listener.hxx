#ifndef NIX_SERVER_EVENT_LISTENER_HXX
#define NIX_SERVER_EVENT_LISTENER_HXX

#include <yami4-cpp/yami.h>
#include <string>

namespace nix {
namespace server {


class EventListener : public yami::event_callback
{
public:
    virtual void agent_created();
    virtual void agent_closed();
    virtual void incoming_connection_open(const char * target);
    virtual void outgoing_connection_open(const char * target);
    virtual void connection_closed(const char * target);
    virtual void connection_error(const char * target);
    virtual void object_registered(const char * name);
    virtual void object_unregistered(const char * name);
    virtual void message_sent(const char * target, std::size_t size);
    virtual void message_received(const char * target, std::size_t size);

private:

};


} // server
} // nix

#endif
