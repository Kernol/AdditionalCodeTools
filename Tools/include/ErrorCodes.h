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

#ifndef EXPLORATIONS_ERRORCODES_H
#define EXPLORATIONS_ERRORCODES_H

/*!
*  Set of errors returned by methods
*/
namespace Parse
{
    enum ErrorCode
    {
        Success = 0,                  ///< No errors occurred
        LoadFailed = 1,               ///< Config file wasn't loaded correctly
        GroupNotFound = 2,            ///< Group wasn't found in key file
        KeyNotFound = 3,              ///< Key wasn't found in key file
        GlibError = 4,                ///< Glib error occurred
        FileNotLoaded = 5,            ///< Config file isn't loaded
        IncorrectFileContainment = 6, ///< File consists element(-s) which can't be parsed
        OutOfRange = 7                ///< Tried to parse value which is larger than type can contain
    };
}

namespace CommonErrors
{
    enum ErrorCode
    {
        Success = 0,                  ///< No errors occurred
    };
}

#endif //EXPLORATIONS_ERRORCODES_H
