#include "ClientAdapter.h"

#include <cmath>
#include <iostream>
#include <sstream>

#include <rpc/client.h>
#include <rpc/rpc_error.h>
#include <rpc/server.h>

#include "Types.h"
#include "common/IControllable.h"

namespace v2 {

#define FUNC(code) std::string(1, static_cast<uint8_t>(code))

ClientAdapter::ClientAdapter(rpc::client& client, ErrorCallback callback)
    : v1::ClientAdapter(client, callback)
{
}

ClientAdapter::~ClientAdapter()
{
}

#undef FUNC

} // namespace v2
