// simd_init.h
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

namespace HastyNoise
{
namespace details
{

template<typename _SimdType, typename _Type, size_t _VectorSize>
union uSIMD
{
    _SimdType m;
    _Type a[_VectorSize];
};

template<SIMDType _SIMDType>
struct InitSIMDValuesBase
{
    template<typename _Constants>
    static void init()
    {
        if(_Constants::valueSet)
            return;

        uSIMD<typename SIMD<_SIMDType>::Float, float, SIMD<_SIMDType>::vectorSize()> incF;
        uSIMD<typename SIMD<_SIMDType>::Int, int32_t, SIMD<_SIMDType>::vectorSize()> incI;
        for(size_t i=0; i<SIMD<_SIMDType>::vectorSize(); i++)
        {
            incF.a[i]=float(i);
            incI.a[i]=int32_t(i);
        }
        _Constants::numf_incremental=incF.m;
        _Constants::numi_incremental=incI.m;

        _Constants::numf_0=SIMD<_SIMDType>::zeroFloat();
        _Constants::numf_1=SIMD<_SIMDType>::set(1.0f);
		_Constants::numf_neg1=SIMD<_SIMDType>::set(-1.0f);
        _Constants::numf_2=SIMD<_SIMDType>::set(2.0f);
        _Constants::numf_6=SIMD<_SIMDType>::set(6.0f);
        _Constants::numf_10=SIMD<_SIMDType>::set(10.0f);
        _Constants::numf_15=SIMD<_SIMDType>::set(15.0f);
        _Constants::numf_32=SIMD<_SIMDType>::set(32.0f);
        _Constants::numf_999999=SIMD<_SIMDType>::set(999999.0f);

        _Constants::numf_0_5=SIMD<_SIMDType>::set(0.5f);
        _Constants::numf_0_6=SIMD<_SIMDType>::set(0.6f);
        _Constants::numf_15_5=SIMD<_SIMDType>::set(15.5f);
        _Constants::numf_511_5=SIMD<_SIMDType>::set(511.5f);
		_Constants::numf_32768_5=SIMD<_SIMDType>::set(32768.5f);

        _Constants::numf_F3=SIMD<_SIMDType>::set(1.f/3.f);
        _Constants::numf_G3=SIMD<_SIMDType>::set(1.f/6.f);
		_Constants::numf_R3=SIMD<_SIMDType>::set(2.f/3.f);
        _Constants::numf_G33=SIMD<_SIMDType>::set((3.f/6.f)-1.f);
        _Constants::numf_hash2Float=SIMD<_SIMDType>::set(1.f/2147483648.f);
        _Constants::numf_vectorSize=SIMD<_SIMDType>::set((float)SIMD<_SIMDType>::vectorSize());
        _Constants::numf_cubicBounding=SIMD<_SIMDType>::set(1.f/(1.5f*1.5f*1.5f));

        _Constants::numi_1=SIMD<_SIMDType>::set(1);
        _Constants::numi_2=SIMD<_SIMDType>::set(2);
        _Constants::numi_255=SIMD<_SIMDType>::set(255);
        _Constants::numi_60493=SIMD<_SIMDType>::set(60493);
        _Constants::numi_0x7fffffff=SIMD<_SIMDType>::set(0x7fffffff);

        _Constants::numi_xPrime=SIMD<_SIMDType>::set(1619);
        _Constants::numi_yPrime=SIMD<_SIMDType>::set(31337);
        _Constants::numi_zPrime=SIMD<_SIMDType>::set(6971);
        _Constants::numi_bit5Mask=SIMD<_SIMDType>::set(31);
        _Constants::numi_bit10Mask=SIMD<_SIMDType>::set(1023);
        _Constants::numi_vectorSize=SIMD<_SIMDType>::set((int)SIMD<_SIMDType>::vectorSize());

        _Constants::numi_0xffffffff=SIMD<_SIMDType>::set(-1);

        _Constants::valueSet=true;
    }
};

template<SIMDType _SIMDType>
struct InitSIMDValues:InitSIMDValuesBase<_SIMDType>
{
    typedef Constants<typename SIMD<_SIMDType>::Float, typename SIMD<_SIMDType>::Int, _SIMDType> _Constants;

    static void _()
    {
        if(_Constants::valueSet)
            return;

        _Constants::numi_8=SIMD<_SIMDType>::set(8);
        _Constants::numi_12=SIMD<_SIMDType>::set(12);
        _Constants::numi_13=SIMD<_SIMDType>::set(13);

        InitSIMDValuesBase<_SIMDType>::template init<_Constants>();
    }
};


}//namespace details
}//namespace HastyNoise
