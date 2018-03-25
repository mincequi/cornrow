#ifndef SERVERADAPTER_H
#define SERVERADAPTER_H

#include "../v1/ServerAdapter.h"
#include "Converter.h"

namespace v2 {

class ServerAdapter : public v1::ServerAdapter
{
public:
    ServerAdapter(rpc::server& server, IControllable& controllable);

private:
    Converter m_converter;
};

} // namespace v2

#endif // SERVERADAPTER_H
