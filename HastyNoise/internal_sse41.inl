// internal_sse41.inl
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifdef HN_COMPILE_SSE41
#include <smmintrin.h>
#endif

namespace HastyNoise
{
namespace details
{

#ifdef HN_COMPILE_SSE41

template<>
struct SIMD<SIMDType::SSE4_1>:SIMD<SIMDType::SSE2>
{
    typedef typename SIMD<SIMDType::SSE2>::Float Float;
    typedef typename SIMD<SIMDType::SSE2>::Int Int;
    typedef typename SIMD<SIMDType::SSE2>::Mask Mask;

    static constexpr size_t const level() { return (size_t)SIMDType::SSE4_1; }

    static Float floor(Float a) { return _mm_floor_ps(a); }
    static Float blend(Float a, Float b, Mask mask) { return _mm_blendv_ps(a, b, cast(mask)); }
    static Int mul(Int a, Int b) { return _mm_mullo_epi32(a, b); }
};

#endif

}//namespace details
}//namespace HastyNoise
