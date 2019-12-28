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

#ifndef EXPLORATIONS_UTILS_H
#define EXPLORATIONS_UTILS_H

#include <cmath>
#include <cinttypes>


namespace Utils
{
    /*!
     * Divide and round result up
     * @tparam T Type of variables
     * @param a Numerator
     * @param b Denominator
     * @return Division result
     */
    template<typename T>
    constexpr T divideAndRoundUp(T a, T b)
    {
        static_assert(std::is_arithmetic_v<T>, "Values must be arithmetic");
        if constexpr (!std::is_unsigned_v<T>) //todo: check in debug
        {
            if (a < 0 && b < 0)
                return divideAndRoundUp(std::abs(a), std::abs(b));
            else if (a < 0 || b < 0)
                return -divideAndRoundUp(std::abs(a), std::abs(b));
        }
        return a == 0 ? 0 : ((a - 1) / b) + 1;
    }


    template<typename T>
    constexpr auto abs(T val) -> std::enable_if_t<std::is_unsigned_v<T>, T>
    {
        return val;
    }

    template<typename T>
    constexpr auto abs(T val) -> std::enable_if_t<!std::is_unsigned_v<T>, T>
    {
        return std::abs(val);
    }


    /*!
     * Check multiplication for overflow
     * @tparam T Type of result and first multiplier
     * @tparam U Type of the second multiplier
     * @param value Pointer to value to be multiplied
     * @param multiplier Multiplier
     * @retval true Success, multiplied
     * @retval false Overflow will be occurred, not multiplied
     */
    template<typename T, typename U>
    bool checkMultiplyOverflow(T *value, U multiplier)
    {
        static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<U>, "Types should be arithmetic");
        assert(value != nullptr);

        if (multiplier == 1)
            return true;
        if (std::is_unsigned_v<T> && multiplier < 0)
            return false;
        if constexpr (std::is_unsigned_v<T> && std::is_unsigned_v<U>)
        {
            if (divideAndRoundUp<std::uintmax_t>(std::numeric_limits<T>::max(), multiplier) <= *value)
                return false;
        }
        else
        {
            if (std::is_unsigned_v<T> &&
                abs(divideAndRoundUp<uintmax_t>(std::numeric_limits<T>::max(), multiplier)) <= uintmax_t(abs(*value)))
                return false;
            else if (abs(divideAndRoundUp<intmax_t>(std::numeric_limits<T>::max(), multiplier)) <=
                     intmax_t(abs(*value)))
                return false;
        }
        *value = T(*value * multiplier);
        return true;
    }
}

#endif //EXPLORATIONS_UTILS_H
