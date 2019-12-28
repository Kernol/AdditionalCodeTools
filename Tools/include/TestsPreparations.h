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

#ifndef EXPLORATIONS_TESTSPREPARATIONS_H
#define EXPLORATIONS_TESTSPREPARATIONS_H

#ifndef NO_CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif // NO_CATCH_CONFIG_MAIN

#include <catch.hpp>

#ifndef PRNG_TYPE
#define PRNG_TYPE uint32_t
#endif // PRNG_TYPE

#if __GNUC_PREREQ(8, 0)
#include <filesystem>
namespace filesystem = std::filesystem;
#else
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#endif // __GNUC_PREREQ(8, 0)

#endif // EXPLORATIONS_TESTSPREPARATIONS_H
