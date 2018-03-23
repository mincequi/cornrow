#include "Util.h"

#include <cmath>
#include <complex>

bool computeBiQuad(int r, Type t, float f, float g, float q, BiQuad* biquad)
{
    switch (t) {
    case Type::Peak: {
        double A = pow(10, g/40.0);
        double w0 = 2*M_PI*f/r;
        double alpha = sin(w0)*0.5/q;

        double alpha1 = alpha*A;
        double alpha2 = alpha/A;
        double a0     = 1.0 + alpha2;

        biquad->b0 = ( 1.0 + alpha1 ) / a0;
        biquad->b1 = (-2.0 * cos(w0)) / a0;
        biquad->b2 = ( 1.0 - alpha1 ) / a0;
        biquad->a1 = biquad->b1;
        biquad->a2 = ( 1.0 - alpha2 ) / a0;
        break;
    }
    case Type::LowPass: {
        double w0 = 2*M_PI*f/r;
        double alpha = sin(w0)*0.5/q;
        double a0    = 1.0 + alpha;

        biquad->b1 = ( 1.0 - cos(w0) ) / a0;
        biquad->b0 = biquad->b1 * 0.5;
        biquad->b2 = biquad->b0;
        biquad->a1 = (-2.0 * cos(w0)) / a0;
        biquad->a2 = ( 1.0 - alpha  ) / a0;
        break;
    }
    case Type::HighPass: {
        double w0 = 2*M_PI*f/r;
        double alpha = sin(w0)*0.5/q;
        double a0    = 1.0 + alpha;

        biquad->b1 = -( 1.0 + cos(w0) ) / a0;
        biquad->b0 = biquad->b1 * -0.5;
        biquad->b2 = biquad->b0;
        biquad->a1 = (-2.0 * cos(w0)) / a0;
        biquad->a2 = ( 1.0 - alpha  ) / a0;
        break;
    }
    case Type::Invalid:
    case Type::Max:
        return false;
    }

    return true;
}

bool computeResponse(Type t, float f, float g, float q, const std::vector<float>& freqs, std::vector<float>* mags , std::vector<float>* phases)
{
    BiQuad biquad;
    if (!computeBiQuad(48000, t, f, g, q, &biquad)) return false;

    mags->resize(freqs.size());
    phases->resize(freqs.size());

    for (size_t i = 0; i < freqs.size(); ++i) {
        double w = 2.0*M_PI*freqs.at(i)/48000;
        std::complex<double> z(cos(w), sin(w));
        std::complex<double> numerator = biquad.b0 + (biquad.b1 + biquad.b2*z)*z;
        std::complex<double> denominator = 1.0 + (biquad.a1 + biquad.a2*z)*z;
        std::complex<double> response = numerator / denominator;
        mags->at(i)     = 20.0*log10(abs(response));
        phases->at(i)   = (180.0/M_PI)*atan2(response.imag(), response.real());
    }

    return true;
}
