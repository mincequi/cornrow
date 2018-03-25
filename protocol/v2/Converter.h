#ifndef V2_CONVERTER_H
#define V2_CONVERTER_H

#include "../v1/Converter.h"

namespace v2 {

class Converter : public v1::Converter
{
public:
    Converter();
    virtual ~Converter();

    virtual uint8_t freqToProto(float f) override;
    virtual float freqFromProto(uint8_t i) override;
};

} // namespace v2

#endif // V2_CONVERTER_H

