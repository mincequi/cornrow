#include "Util.h"

#include <cmath>
#include <complex>

namespace common
{

bool computeBiQuad(int r, const Filter& f, BiQuad* biquad)
{
    switch (f.type) {
    case FilterType::Peak: {
        double A = pow(10, f.g/40.0);
        double w0 = 2*M_PI*f.f/r;
        double alpha = sin(w0)*0.5/f.q;

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
    case FilterType::LowPass: {
        double w0 = 2*M_PI*f.f/r;
        double alpha = sin(w0)*0.5/f.q;
        double a0    = 1.0 + alpha;

        biquad->b1 = ( 1.0 - cos(w0) ) / a0;
        biquad->b0 = biquad->b1 * 0.5;
        biquad->b2 = biquad->b0;
        biquad->a1 = (-2.0 * cos(w0)) / a0;
        biquad->a2 = ( 1.0 - alpha  ) / a0;
        break;
    }
    case FilterType::HighPass: {
        double w0 = 2*M_PI*f.f/r;
        double alpha = sin(w0)*0.5/f.q;
        double a0    = 1.0 + alpha;

        biquad->b1 = -( 1.0 + cos(w0) ) / a0;
        biquad->b0 = biquad->b1 * -0.5;
        biquad->b2 = biquad->b0;
        biquad->a1 = (-2.0 * cos(w0)) / a0;
        biquad->a2 = ( 1.0 - alpha  ) / a0;
        break;
    }
    case FilterType::LowShelf: {
        double A = pow(10, f.g/40.0);
        double w0 = 2*M_PI*f.f/r;
        double cos_w0 = cos(w0);
        double alpha = sin(w0)*0.5/f.q;
        double alpha2 = 2*sqrt(A)*alpha;
        double a0 = (A+1) + (A-1)*cos_w0 + alpha2;

        biquad->b0 =  (  A*( (A+1) - (A-1)*cos_w0 + alpha2) )/a0;
        biquad->b1 =  (2*A*( (A-1) - (A+1)*cos_w0         ) )/a0;
        biquad->b2 =  (  A*( (A+1) - (A-1)*cos_w0 - alpha2) )/a0;
        biquad->a1 =  ( -2*( (A-1) + (A+1)*cos_w0         ) )/a0;
        biquad->a2 =  (      (A+1) + (A-1)*cos_w0 - alpha2  )/a0;
        break;
    }
    case FilterType::HighShelf: {
        double A = pow(10, f.g/40.0);
        double w0 = 2*M_PI*f.f/r;
        double cos_w0 = cos(w0);
        double alpha = sin(w0)*0.5/f.q;
        double alpha2 = 2*sqrt(A)*alpha;
        double a0 = (A+1) - (A-1)*cos_w0 + alpha2;

        biquad->b0 =  (  A*( (A+1) + (A-1)*cos_w0 + alpha2) )/a0;
        biquad->b1 = (-2*A*( (A-1) + (A+1)*cos_w0         ) )/a0;
        biquad->b2 =  (  A*( (A+1) + (A-1)*cos_w0 - alpha2) )/a0;
        biquad->a1 =  (  2*( (A-1) - (A+1)*cos_w0         ) )/a0;
        biquad->a2 =  (      (A+1) - (A-1)*cos_w0 - alpha2  )/a0;
        break;
    }
    case FilterType::AllPass: {
        double w0 = 2*M_PI*f.f/r;
        double cos_w0 = cos(w0);
        double alpha = sin(w0)*0.5/f.q;
        double a0 = 1 + alpha;

        biquad->b0 = (1 - alpha)/a0;
        biquad->b1 = (-2*cos_w0)/a0;
        biquad->b2 = 1.0;
        biquad->a1 = biquad->b1;
        biquad->a2 = biquad->b0;
        break;
    }

    case FilterType::Invalid:
    case FilterType::CrossoverLr2:
    case FilterType::CrossoverLr4:
    case FilterType::Subwoofer:
    case FilterType::Loudness:
    //case FilterType::Max:
        return false;
    }

    return true;
}

} // namespace common
