/*
* Tests for Simd Library (http://simd.sourceforge.net).
*
* Copyright (c) 2011-2016 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Test/TestUtils.h"
#include "Test/TestPerformance.h"
#include "Test/TestData.h"

namespace Test
{
    namespace
    {
        struct Func1
        {
            typedef void (*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height, 
                uint8_t saturation, uint8_t boost, uint8_t * dx, size_t dxStride, uint8_t * dy, size_t dyStride);

            FuncPtr func;
            String description;

            Func1(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, uint8_t saturation, uint8_t boost, View &  dx, View & dy) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, saturation, boost, dx.data, dx.stride, dy.data, dy.stride);
            }
        };
    }
#define FUNC1(function) Func1(function, #function)

    bool TextureBoostedSaturatedGradientAutoTest(int width, int height, int saturation, int boost, const Func1 & f1, const Func1 & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "] <" << saturation << ", " << boost << ">.");

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);

        View dx1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dy1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dx2(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dy2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, saturation, boost, dx1, dy1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, saturation, boost, dx2, dy2));

        result = result && Compare(dx1, dx2, 0, true, 32, 0, "dx");
        result = result && Compare(dy1, dy2, 0, true, 32, 0, "dy");

        return result;
    }

    bool TextureBoostedSaturatedGradientAutoTest(int width, int height, const Func1 & f1, const Func1 & f2)
    {
        bool result = true;

        result = result && TextureBoostedSaturatedGradientAutoTest(width, height, 32, 3, f1, f2);
        result = result && TextureBoostedSaturatedGradientAutoTest(width, height, 16, 4, f1, f2);
        result = result && TextureBoostedSaturatedGradientAutoTest(width, height, 16, 5, f1, f2);

        return result;
    }

    bool TextureBoostedSaturatedGradientAutoTest(const Func1 & f1, const Func1 & f2)
    {
        bool result = true;

        result = result && TextureBoostedSaturatedGradientAutoTest(W, H, f1, f2);
        result = result && TextureBoostedSaturatedGradientAutoTest(W + O, H - O, f1, f2);
        result = result && TextureBoostedSaturatedGradientAutoTest(W - O, H + O, f1, f2);

        return result;
    }


    namespace
    {
        struct Func2
        {
            typedef void (*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height, 
                uint8_t boost, uint8_t * dst, size_t dstStride);

            FuncPtr func;
            String description;

            Func2(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, uint8_t boost, View &  dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, boost, dst.data, dst.stride);
            }
        };
    }
#define FUNC2(function) Func2(function, #function)

    bool TextureBoostedUvAutoTest(int width, int height, int boost, const Func2 & f1, const Func2 & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "] <" << boost << ">.");

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);

        View dst1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dst2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, boost, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, boost, dst2));

        result = result && Compare(dst1, dst2, 0, true, 32, 0);

        return result;
    }

    bool TextureBoostedUvAutoTest(int width, int height, const Func2 & f1, const Func2 & f2)
    {
        bool result = true;

        result = result && TextureBoostedUvAutoTest(width, height, 3, f1, f2);
        result = result && TextureBoostedUvAutoTest(width, height, 4, f1, f2);
        result = result && TextureBoostedUvAutoTest(width, height, 5, f1, f2);

        return result;
    }

    bool TextureBoostedUvAutoTest(const Func2 & f1, const Func2 & f2)
    {
        bool result = true;

        result = result && TextureBoostedUvAutoTest(W, H, f1, f2);
        result = result && TextureBoostedUvAutoTest(W + O, H - O, f1, f2);
        result = result && TextureBoostedUvAutoTest(W - O, H + O, f1, f2);

        return result;
    }


    namespace
    {
        struct Func3
        {
            typedef void (*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height, 
                const uint8_t * lo, size_t loStride, const uint8_t * hi, size_t hiStride, int64_t * sum);

            FuncPtr func;
            String description;

            Func3(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, const View & lo, const View & hi, int64_t * sum) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, lo.data, lo.stride, hi.data, hi.stride, sum);
            }
        };
    }
#define FUNC3(function) Func3(function, #function)

    bool TextureGetDifferenceSumAutoTest(int width, int height, const Func3 & f1, const Func3 & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);
        View lo(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(lo);
        View hi(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(hi);

        int64_t s1, s2;

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, lo, hi, &s1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, lo, hi, &s2));

        TEST_CHECK_VALUE(s);

        return result;
    }

    bool TextureGetDifferenceSumAutoTest(const Func3 & f1, const Func3 & f2)
    {
        bool result = true;

        result = result && TextureGetDifferenceSumAutoTest(W, H, f1, f2);
        result = result && TextureGetDifferenceSumAutoTest(W + O, H - O, f1, f2);
        result = result && TextureGetDifferenceSumAutoTest(W - O, H + O, f1, f2);

        return result;
    }


    namespace
    {
        struct Func4
        {
            typedef void (*FuncPtr)(const uint8_t * src, size_t srcStride, size_t width, size_t height, 
                int shift, uint8_t * dst, size_t dstStride);

            FuncPtr func;
            String description;

            Func4(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, int shift, View &  dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, shift, dst.data, dst.stride);
            }
        };
    }
#define FUNC4(function) Func4(function, #function)

    bool TexturePerformCompensationAutoTest(int width, int height, int shift, const Func4 & f1, const Func4 & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "] <" << shift << ">.");

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);

        View dst1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dst2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, shift, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, shift, dst2));

        result = result && Compare(dst1, dst2, 0, true, 32, 0);

        return result;
    }

    bool TexturePerformCompensationAutoTest(int width, int height, const Func4 & f1, const Func4 & f2)
    {
        bool result = true;

        result = result && TexturePerformCompensationAutoTest(width, height, 17, f1, f2);
        result = result && TexturePerformCompensationAutoTest(width, height, 3, f1, f2);
        result = result && TexturePerformCompensationAutoTest(width, height, 0, f1, f2);
        result = result && TexturePerformCompensationAutoTest(width, height, -4, f1, f2);
        result = result && TexturePerformCompensationAutoTest(width, height, -33, f1, f2);

        return result;
    }

    bool TexturePerformCompensationAutoTest(const Func4 & f1, const Func4 & f2)
    {
        bool result = true;

        result = result && TexturePerformCompensationAutoTest(W, H, f1, f2);
        result = result && TexturePerformCompensationAutoTest(W + O, H - O, f1, f2);
        result = result && TexturePerformCompensationAutoTest(W - O, H + O, f1, f2);

        return result;
    }



}
