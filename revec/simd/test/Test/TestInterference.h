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
			typedef void (*FuncPtr)(uint8_t * statistic, size_t stride, size_t width, size_t height, uint8_t value, int16_t saturation);

			FuncPtr func;
			String description;

			Func1(const FuncPtr & f, const String & d) : func(f), description(d) {}

			void Call(const View & statisticSrc, View & statisticDst, uint8_t value, int16_t saturation) const
			{
				Simd::Copy(statisticSrc, statisticDst);
				TEST_PERFORMANCE_TEST(description);
				func(statisticDst.data, statisticDst.stride, statisticDst.width, statisticDst.height, value, saturation);
			}
		};

        struct Func2
        {
            typedef void (*FuncPtr)(uint8_t * statistic, size_t statisticStride, size_t width, size_t height, 
                uint8_t value, int16_t saturation, const uint8_t * mask, size_t maskStride, uint8_t index);

            FuncPtr func;
            String description; 

            Func2(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & statisticSrc, View & statisticDst, uint8_t value, int16_t saturation, const View & mask, uint8_t index) const
            {
                Simd::Copy(statisticSrc, statisticDst);
                TEST_PERFORMANCE_TEST(description);
                func(statisticDst.data, statisticDst.stride, statisticDst.width, statisticDst.height, 
                    value, saturation, mask.data, mask.stride, index);
            }
        };
	}

#define FUNC1(function) Func1(function, std::string(#function))

#define FUNC2(function) Func2(function, std::string(#function))

	bool InterferenceChangeAutoTest(int width, int height, const Func1 & f1, const Func1 & f2)
	{
		bool result = true;

		TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

		View statisticSrc(width, height, View::Int16, NULL, TEST_ALIGN(width));
		FillRandom(statisticSrc, 0, 64);

        uint8_t value = 3;
        int16_t saturation = 8888;

		View statisticDst1(width, height, View::Int16, NULL, TEST_ALIGN(width));
		View statisticDst2(width, height, View::Int16, NULL, TEST_ALIGN(width));

		TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(statisticSrc, statisticDst1, value, saturation));

		TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(statisticSrc, statisticDst2, value, saturation));

		result = result && Compare(statisticDst1, statisticDst2, 0, true, 32, 0);

		return result;
	}

    bool InterferenceChangeAutoTest(const Func1 & f1, const Func1 & f2)
    {
        bool result = true;

        result = result && InterferenceChangeAutoTest(W, H, f1, f2);
        result = result && InterferenceChangeAutoTest(W + O, H - O, f1, f2);
        result = result && InterferenceChangeAutoTest(W - O, H + O, f1, f2);

        return result;
    }


    bool InterferenceChangeMaskedAutoTest(int width, int height, const Func2 & f1, const Func2 & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View statisticSrc(width, height, View::Int16, NULL, TEST_ALIGN(width));
        FillRandom(statisticSrc, 0, 64);

        uint8_t value = 3, index = 11;
        int16_t saturation = 8888;

        View mask(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandomMask(mask, index);

        View statisticDst1(width, height, View::Int16, NULL, TEST_ALIGN(width));
        View statisticDst2(width, height, View::Int16, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(statisticSrc, statisticDst1, value, saturation, mask, index));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(statisticSrc, statisticDst2, value, saturation, mask, index));

        result = result && Compare(statisticDst1, statisticDst2, 0, true, 32, 0);

        return result;
    }

    bool InterferenceChangeMaskedAutoTest(const Func2 & f1, const Func2 & f2)
    {
        bool result = true;

        result = result && InterferenceChangeMaskedAutoTest(W, H, f1, f2);
        result = result && InterferenceChangeMaskedAutoTest(W + O, H - O, f1, f2);
        result = result && InterferenceChangeMaskedAutoTest(W - O, H + O, f1, f2);

        return result;
    }

 
}
