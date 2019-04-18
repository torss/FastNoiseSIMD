#ifdef fastNoise_sse2_EXPORTS
#define fastNoise_EXPORTS
#endif
#include "FastNoiseSIMD.h"

#include "simd_constants.inl"
#include "internal_none.inl"
#include "internal_sse2.inl"
#include "FastNoiseSIMD_internal.h"

#include "simd_init.inl"
#include "FastNoiseSIMD_internal.inl"

namespace FastNoise
{
namespace details
{

template class FASTNOISE_EXPORT NoiseSIMD<SIMDType::SSE2>;

}//namespace details
}//namespace FastNoiseSIMD
