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
project(AdditionalCodeTools)

include_directories(Catch2/single_include/catch2)
include_directories(Kerlog/src)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Tools)
add_subdirectory(Kerlog)