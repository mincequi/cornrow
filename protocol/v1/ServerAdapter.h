#ifndef V1_SERVERADAPTER_H
#define V1_SERVERADAPTER_H

#include "Converter.h"

class IControllable;
namespace rpc {
class server;
}

namespace v1 {

class ServerAdapter
{
public:
    ServerAdapter(rpc::server& server, IControllable& controllable);
    virtual ~ServerAdapter();

private:
    Converter m_converter;
};

} // namespace v1

#endif // V1_SERVERADAPTER_H
