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

#ifndef EXPLORATIONS_TIMECONVERSION_H
#define EXPLORATIONS_TIMECONVERSION_H

#include <chrono>
#include <string>
#include <stdexcept>
#include <utils.h>

namespace timeConversion
{
    /*!
     * @class TimeConverter Converts std::string to std::chrono::duration type
     * @note Parsed string must be like linux date format (see 'man date' for details)
     */
    class TimeConverter
    {
        /*!
         * Check for division without remainder
         */
        template<typename T, typename U>
        static bool checkTimeDivision(T *numerator, U denominator)
        {
            static_assert(std::is_integral_v <T> && std::is_integral_v <U> , "Types must be integral");
            static_assert(std::is_unsigned_v <T> && std::is_unsigned_v <U> , "Types must be unsigned");
            assert(numerator != nullptr);
            assert(denominator > 0);

            if (denominator == 1)
                return true;
            if (*numerator % denominator != 0)
                return false;
            *numerator = T(*numerator / denominator);
            return true;
        }


        template<typename FromT, typename ToT>
        static void convert(size_t& size, const std::string& sizeString)
        {
            static_assert(is_duration_v <FromT> && is_duration_v <ToT> , "Types must be durations");
            static_assert(FromT::period::num > 0 && FromT::period::den > 0 &&
                          ToT::period::num > 0 && ToT::period::den > 0);
            static_assert(Utils::divideAndRoundUp<std::uintmax_t>(std::numeric_limits<decltype(FromT::period::num)>::max(),
                                                ToT::period::den) >= FromT::period::num);
            static_assert(Utils::divideAndRoundUp<std::uintmax_t>(std::numeric_limits<decltype(FromT::period::den)>::max(),
                                                ToT::period::num) >= FromT::period::den);

            constexpr uint64_t diff1 = FromT::period::num * ToT::period::den;
            constexpr uint64_t diff2 = FromT::period::den * ToT::period::num;
            if(!Utils::checkMultiplyOverflow(&size, diff1))
                throw std::overflow_error("Size " + sizeString + " is too large");
            if(!checkTimeDivision(&size, diff2))
                throw std::runtime_error("Cannot divide without remainder: " + sizeString);
        }

        template<typename _Tp>
        struct is_duration : std::false_type { };

        template<typename _Rep, typename _Period>
        struct is_duration<std::chrono::duration<_Rep, _Period>> : std::true_type { };

        template<typename T>
        inline static const bool is_duration_v = is_duration<T>::value;

    public:
        typedef std::chrono::nanoseconds  nanoseconds;
        typedef std::chrono::microseconds microseconds;
        typedef std::chrono::milliseconds milliseconds;
        typedef std::chrono::seconds      seconds;
        typedef std::chrono::minutes      minutes;
        typedef std::chrono::hours        hours;
        typedef std::chrono::duration <int64_t, std::ratio<86400>>  days;
        typedef std::chrono::duration <int64_t, std::ratio<604800>> weeks;

        /*!
         * Converts std::string to std::chrono::duration if possible
         * @note String time format must be the same as linux date format (see 'man date' for details)
         * @tparam T Type the string will be converted to (must be std::chrono::duration)
         * @param sizeString String to be converted
         * @return std::chrono::duration value converted from string
         */
        template<typename T>
        static T stringToTime(const std::string &sizeString)
        {
            static_assert(is_duration_v<T>, "Type must be duration");

            char *endPtr = nullptr;
            errno = 0;
            auto uMax = strtoumax(sizeString.c_str(), &endPtr, 10);
            if (uMax == 0 && sizeString.c_str() == endPtr)
                throw std::runtime_error("Size " + sizeString + " does not contain integers");
            if (uMax == UINTMAX_MAX && errno == ERANGE)
                throw std::overflow_error("Size " + sizeString + " is out of range");
            if constexpr (sizeof(uMax) > sizeof(T))
            {
                if (uMax > std::numeric_limits<T>::max())
                    throw std::overflow_error("Size " + sizeString + " is too large");
            }

            auto size = uint64_t(uMax);
            switch (*endPtr)
            {
                case 'w':
                {
                    convert<weeks, T>(size, sizeString);
                    break;
                }
                case 'd':
                {
                    convert<days, T>(size, sizeString);
                    break;
                }
                case 'H':
                {
                    convert<hours, T>(size, sizeString);
                    break;
                }
                case 'M':
                {
                    convert<minutes, T>(size, sizeString);
                    break;
                }
                case 'N':
                {
                    convert<nanoseconds, T>(size, sizeString);
                    break;
                }
                case 'S':
                    [[fallthrough]];
                default:
                {
                    convert<seconds, T>(size, sizeString);
                    break;
                }
            }
            return T(size);
        }
    };
}

#endif //EXPLORATIONS_TIMECONVERSION_H
