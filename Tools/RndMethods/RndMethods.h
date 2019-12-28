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

#ifndef EXPLORATIONS_RNDMETHODS_H
#define EXPLORATIONS_RNDMETHODS_H

#include <random>
#include <cassert>

#ifndef PRNG_TYPE
#define PRNG_TYPE uint32_t
#endif // PRNG_TYPE

#define RANDOM_VALUE rndMethods::randomGenerator.randomValue
#define RANDOM_DATA rndMethods::randomGenerator.randomData


namespace rndMethods
{
    /*!
     * @class RandomGenerator
     * Used to generate random values,random data vectors or fill provided memory chunk with random values
     */
    class RandomGenerator
    {
        std::mt19937 _generator;
        std::uniform_int_distribution<PRNG_TYPE> _uid;

    public:

        RandomGenerator() noexcept;

        /*!
         * Generates value of template parameter type
         * @note PRNG_TYPE must be convertible to template parameter type
         * @tparam valueType Value type to be generated
         * @return Generated value
         */
        template<typename valueType = uint8_t>
        constexpr valueType randomValue();

        /*!
         * fill provided memory chunk with random values
         * @tparam valueType Pointer to type of values to be generated
         * @param dataLength Provided memory chunk size
         */
        template<typename valueType = uint8_t *>
        void constexpr randomData(valueType pointerToData, size_t dataLength);

        /*!
         * Generate vector of values of template parameter type
         * @tparam valueType Type of values to be generated
         * @param count Values count to be generated
         * @return Vector of generated values
         */
        template<typename valueType = uint8_t>
        std::vector<valueType> randomData(size_t count);
    };

    extern RandomGenerator randomGenerator;
}


template<typename valueType>
constexpr valueType rndMethods::RandomGenerator::randomValue()
{
    static_assert(std::is_convertible_v<PRNG_TYPE, valueType>);
    return static_cast<valueType>(_uid(_generator));
}

template<typename valueType>
void constexpr rndMethods::RandomGenerator::randomData(valueType pointerToData, size_t dataLength)
{
    size_t dataSize = dataLength * sizeof(decltype(*pointerToData));
    auto *buffer = reinterpret_cast<uint8_t *>(pointerToData);
    for (size_t i = 0; i < dataSize; ++i)
        buffer[i] = RANDOM_VALUE<uint8_t>();
}

template<typename valueType>
std::vector<valueType> rndMethods::RandomGenerator::randomData(size_t count)
{
    std::vector<valueType> result(count);
    randomData<valueType *>(result.data(), count);
    return result;
}

#endif //EXPLORATIONS_RNDMETHODS_H