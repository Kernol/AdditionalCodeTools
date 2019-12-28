// AdditionalCodeTools. Support tools for main code.
// Copyright (C) 2019 Evgeny Zaytsev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <RndMethods.h>
#include <TestsPreparations.h>
#include <iostream>

#define REPEAT_COUNT 1000


TEST_CASE("RndMethods")
{
    SECTION("RandomValue")
    {
        SECTION("DefaultTemplateParameter")
        {
            for (auto i = 0; i < REPEAT_COUNT; ++i)
            {
                auto rndValue = RANDOM_VALUE<>();
                REQUIRE(std::is_same_v<decltype(rndValue), uint8_t>);

            }
        }

        SECTION("CustomTemplateParameter")
        {
            for (auto i = 0; i < REPEAT_COUNT; ++i)
            {
                auto rndValue_uint16 = RANDOM_VALUE<uint16_t>();
                REQUIRE(std::is_same_v<decltype(rndValue_uint16), uint16_t>);
                auto rndValue_uchar = RANDOM_VALUE<unsigned char>();
                REQUIRE(std::is_same_v<decltype(rndValue_uchar), unsigned char>);
            }
        }
    }

    SECTION("RandomData")
    {
        SECTION("GetVectorOfGeneratedValues")
        {
            for (auto i = 0; i < REPEAT_COUNT; ++i)
            {
                auto vectorSize = RANDOM_VALUE<size_t>() % 16 + 4;
                std::vector<uint16_t> result;
                REQUIRE_NOTHROW(result = RANDOM_DATA<uint16_t>(vectorSize));
                REQUIRE(result.size() == vectorSize);
                REQUIRE(std::is_same_v<decltype(result)::value_type, uint16_t>);
            }
        }

        SECTION("FillDataChunk")
        {
            for (auto i = 0; i < REPEAT_COUNT; ++i)
            {
                auto dataSize = RANDOM_VALUE<size_t>() % 16 + 4;
                std::unique_ptr<uint16_t[]> data(new uint16_t[dataSize]);
                REQUIRE_NOTHROW(RANDOM_DATA(data.get(), dataSize));
                REQUIRE(std::is_same_v<std::remove_reference<decltype(*data.get())>::type, uint16_t>);
            }
        }
    }
}