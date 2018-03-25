#ifndef V2_ADAPTER_H
#define V2_ADAPTER_H

#include "../v1/ClientAdapter.h"

namespace v2 {

class ClientAdapter : public v1::ClientAdapter
{
public:
    ClientAdapter(rpc::client& client, ErrorCallback callback = nullptr);
    virtual ~ClientAdapter();
};

} // namespace v2

#endif // ADAPTER_H
