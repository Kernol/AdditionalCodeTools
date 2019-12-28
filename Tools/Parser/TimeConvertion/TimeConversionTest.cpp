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

#include <TestsPreparations.h>
#include <TimeConversion.h>

#define DURATION timeConversion::TimeConverter
#define STR_TO_TIME DURATION::stringToTime


TEST_CASE("TimeConversionTest")
{
    SECTION("From nanoseconds")
    {
        REQUIRE(STR_TO_TIME<DURATION::microseconds>("1000N") == DURATION::microseconds(1));
        REQUIRE(STR_TO_TIME<DURATION::milliseconds>("1000000N") == DURATION::milliseconds(1));
        REQUIRE(STR_TO_TIME<DURATION::seconds>("1000000000N") == DURATION::seconds(1));
        REQUIRE(STR_TO_TIME<DURATION::minutes>("60000000000N") == DURATION::minutes(1));
        REQUIRE(STR_TO_TIME<DURATION::hours>("3600000000000N") == DURATION::hours(1));
        REQUIRE(STR_TO_TIME<DURATION::days>("86400000000000N") == DURATION::days(1));
    }

    SECTION("From seconds")
    {
        REQUIRE(STR_TO_TIME<DURATION::milliseconds>("1S") == DURATION::milliseconds(1000));
        REQUIRE(STR_TO_TIME<DURATION::milliseconds>("1") == DURATION::milliseconds(1000));
        REQUIRE(STR_TO_TIME<DURATION::microseconds>("1S") == DURATION::microseconds(1000000));
        REQUIRE(STR_TO_TIME<DURATION::microseconds>("1") == DURATION::microseconds(1000000));
        REQUIRE(STR_TO_TIME<DURATION::nanoseconds>("1S") == DURATION::nanoseconds(1000000000));
        REQUIRE(STR_TO_TIME<DURATION::nanoseconds>("1") == DURATION::nanoseconds(1000000000));
        REQUIRE(STR_TO_TIME<DURATION::minutes>("60S") == DURATION::minutes(1));
        REQUIRE(STR_TO_TIME<DURATION::minutes>("60") == DURATION::minutes(1));
        REQUIRE(STR_TO_TIME<DURATION::hours>("3600S") == DURATION::hours(1));
        REQUIRE(STR_TO_TIME<DURATION::hours>("3600") == DURATION::hours(1));
        REQUIRE(STR_TO_TIME<DURATION::days>("86400S") == DURATION::days(1));
        REQUIRE(STR_TO_TIME<DURATION::days>("86400") == DURATION::days(1));
    }

    SECTION("From minutes")
    {
        REQUIRE(STR_TO_TIME<DURATION::days>("1440M") == DURATION::days(1));
        REQUIRE(STR_TO_TIME<DURATION::hours>("60M") == DURATION::hours(1));
        REQUIRE(STR_TO_TIME<DURATION::seconds>("1M") == DURATION::seconds(60));
        REQUIRE(STR_TO_TIME<DURATION::milliseconds>("1M") == DURATION::milliseconds(60000));
        REQUIRE(STR_TO_TIME<DURATION::microseconds>("1M") == DURATION::microseconds(60000000));
        REQUIRE(STR_TO_TIME<DURATION::nanoseconds>("1M") == DURATION::nanoseconds(60000000000));
    }

    SECTION("From hours")
    {
        REQUIRE(STR_TO_TIME<DURATION::days>("24H") == DURATION::days(1));
        REQUIRE(STR_TO_TIME<DURATION::minutes>("1H") == DURATION::minutes(60));
        REQUIRE(STR_TO_TIME<DURATION::seconds>("1H") == DURATION::seconds(3600));
        REQUIRE(STR_TO_TIME<DURATION::milliseconds>("1H") == DURATION::milliseconds(3600000));
        REQUIRE(STR_TO_TIME<DURATION::microseconds>("1H") == DURATION::microseconds(3600000000));
        REQUIRE(STR_TO_TIME<DURATION::nanoseconds>("1H") == DURATION::nanoseconds(3600000000000));
    }

    SECTION("From days")
    {
        REQUIRE(STR_TO_TIME<DURATION::hours>("1d") == DURATION::hours(24));
        REQUIRE(STR_TO_TIME<DURATION::minutes>("1d") == DURATION::minutes(1440));
        REQUIRE(STR_TO_TIME<DURATION::seconds>("1d") == DURATION::seconds(86400));
        REQUIRE(STR_TO_TIME<DURATION::milliseconds>("1d") == DURATION::milliseconds(86400000));
        REQUIRE(STR_TO_TIME<DURATION::microseconds>("1d") == DURATION::microseconds(86400000000));
        REQUIRE(STR_TO_TIME<DURATION::nanoseconds>("1d") == DURATION::nanoseconds(86400000000000));
    }

    SECTION("From weeks")
    {
        REQUIRE(STR_TO_TIME<DURATION::days>("1w") == DURATION::days(7));
        REQUIRE(STR_TO_TIME<DURATION::hours>("1w") == DURATION::hours(168));
        REQUIRE(STR_TO_TIME<DURATION::minutes>("1w") == DURATION::minutes(10080));
        REQUIRE(STR_TO_TIME<DURATION::seconds>("1w") == DURATION::seconds(604800));
        REQUIRE(STR_TO_TIME<DURATION::milliseconds>("1w") == DURATION::milliseconds(604800000));
        REQUIRE(STR_TO_TIME<DURATION::microseconds>("1w") == DURATION::microseconds(604800000000));
        REQUIRE(STR_TO_TIME<DURATION::nanoseconds>("1w") == DURATION::nanoseconds(604800000000000));
    }

    SECTION("Zero conversions")
    {
        REQUIRE(STR_TO_TIME<DURATION::microseconds>("0N") == DURATION::microseconds(0));
        REQUIRE(STR_TO_TIME<DURATION::microseconds>("0S") == DURATION::microseconds(0));
        REQUIRE(STR_TO_TIME<DURATION::milliseconds>("0M") == DURATION::milliseconds(0));
        REQUIRE(STR_TO_TIME<DURATION::seconds>("0H") == DURATION::seconds(0));
        REQUIRE(STR_TO_TIME<DURATION::minutes>("0d") == DURATION::minutes(0));
        REQUIRE(STR_TO_TIME<DURATION::hours>("0w") == DURATION::hours(0));
        REQUIRE(STR_TO_TIME<DURATION::days>("0S") == DURATION::days(0));
        REQUIRE(STR_TO_TIME<DURATION::weeks>("0M") == DURATION::weeks(0));
        REQUIRE(STR_TO_TIME<DURATION::seconds>("0") == DURATION::seconds(0));
    }

    SECTION("ExceptionWasThrown")
    {
        SECTION("WrongCast")
        {
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::microseconds>("1N"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::milliseconds>("1N"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::seconds>("1N"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::minutes>("1N"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::hours>("1N"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::days>("1N"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::weeks>("1N"), std::runtime_error);

            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::minutes>("1S"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::hours>("1S"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::days>("1S"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::weeks>("1S"), std::runtime_error);

            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::hours>("1M"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::days>("1M"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::weeks>("1M"), std::runtime_error);

            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::days>("1H"), std::runtime_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::weeks>("1H"), std::runtime_error);

            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::weeks>("1d"), std::runtime_error);

            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::weeks>("ad"), std::runtime_error);
        }

        SECTION("Overflow")
        {
            std::string str = std::to_string(std::numeric_limits<uint64_t>::max());
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::seconds>(str + "0N"), std::overflow_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::seconds>(str + "0S"), std::overflow_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::seconds>(str + "0M"), std::overflow_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::seconds>(str + "0H"), std::overflow_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::seconds>(str + "0d"), std::overflow_error);
            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::seconds>(str + "0w"), std::overflow_error);

            REQUIRE_THROWS_AS(STR_TO_TIME<DURATION::seconds>(str + "w"), std::overflow_error);
        }
    }
}