#ifdef fastNoise_sse41_EXPORTS
#define fastNoise_EXPORTS
#endif
#include "FastNoiseSIMD.h"

#include "simd_constants.inl"
#include "internal_none.inl"
#include "internal_sse2.inl"
#include "internal_sse41.inl"
#include "FastNoiseSIMD_internal.h"

#include "simd_init.inl"
#include "FastNoiseSIMD_internal.inl"

namespace FastNoise
{
namespace details
{

template class FASTNOISE_EXPORT NoiseSIMD<SIMDType::SSE4_1>;
//template struct Constants<typename SIMD<SIMDType::SSE4_1>::Float, typename SIMD<SIMDType::SSE4_1>::Int, SIMDType::SSE4_1>;

}//namespace details
}//namespace FastNoiseSIMD
