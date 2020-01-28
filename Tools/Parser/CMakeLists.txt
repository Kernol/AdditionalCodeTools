## AdditionalCodeTools. Support tools for main code.
## Copyright (C) 2019 Evgeny Zaytsev
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)
project(Tools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include(FindPkgConfig)
pkg_check_modules(GLIB glib-2.0 REQUIRED)

include_directories(TimeConvertion)
add_subdirectory(TimeConvertion)

add_library(Parser Parser.cpp Parser.h ./TimeConvertion/TimeConversion.h)
target_include_directories(Parser PUBLIC ${GLIB_INCLUDE_DIRS} TimeConvertion ../include ../../Kerlog/src)
target_link_libraries(Parser ${GLIB_LIBRARIES} Kerlog)

enable_testing()
add_executable(ParserTest ParserTest.cpp)
target_link_libraries(ParserTest Parser)
add_test(ParserTest ParserTest)