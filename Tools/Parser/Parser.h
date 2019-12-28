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

#ifndef EXPLORATIONS_PARSER_H
#define EXPLORATIONS_PARSER_H

#include <ErrorCodes.h>
#include <vector>
#include <string>
#include <glib.h>
#include <algorithm>
#include <TimeConversion.h>
#include <memory>


namespace Parse
{
    typedef std::vector<std::pair<std::string,std::vector<std::string>>> GroupInfo;

    /*!
     * @class Convert
     * Defines possible type conversions from std::string
     * @tparam T Type the string will be converted to
     */
    template <typename T>
    class Convert
    {};

    /*!
     *  @class Parser
     *  @brief Used to parse options from .ini file. See glib key-value file parser reference for details
     *
     *  The structure of .ini file to be parsed must be in format like this:
     *      [GroupName]
     *      keyName=Value
     *  Example:
     *      [Common]
     *      time=3600
     *      [Addition]
     *      Timeouts=600;900
     *      RetryTime=300
     *  Syntax:
     *      "\"  -  escape symbol
     *      ";"  -  values separator
     *      "\\" parses as "\"
     *      "\;" parses as ";"
     *
     *  @warning There are some glib limitations^
     *      1. If there's single "\" in any key, then can't parse.
     *      2. Recognizes empty string at the begin (";text" -> "", "text" ) but not at the end: ";text;" -> "", "text"
     *
     *  @note Possible types to be parsed:
     *      - bool
     *      - std::string
     *      - arithmetic types (int, float, double, long, long double, long long, unsigned long, unsigned long long)
     *      - std::chrono::duration types: supported to parse values from std::chrono::nanoseconds to
     *        std::chrono::duration <int64_t, std::ratio<604800>> (week).
     *        Format is similar to linux date format (see 'man date' for details)
     */
    class Parser
    {
        std::unique_ptr<GKeyFile,  void(*)(GKeyFile*)> _keyFile;

        /*!
         * @defgroup glibErrors
         * @retval GroupNotFound Group wasn't found
         * @retval KeyNotFound Key wasn't found
         * @retval GlibError Another glib error occurred
         */

        /*!
         * @defgroup convertErrors
         * @retval GroupNotFound Group wasn't found
         * @retval KeyNotFound Key wasn't found
         * @retval GlibError Another glib error occurred
         */

        /*!
         * Check occurred error for glib errors
         * @param group_name Group name to get value from
         * @param key Key from group to get value from
         * @param error Error to check
         * @return Tools error code
         * @copydetails glibErrors
         */
        ErrorCode glibErrorCheck(const std::string& group_name, const std::string& key, GError_autoptr error) const;

        /*!
         * Converts multiple strings into needed type
         * @tparam T Type the value will be converted to
         * @param ret Vector of strings to be converted
         * @return Tools error code and vector of converted strings
         * @retval Success
         * @copydetails convertErrors
         */
        template <typename T>
        std::pair<std::vector<T>, ErrorCode> multipleConvert(const std::vector<std::string> &ret) const;

        /*!
         * Parses single string
         * @param group_name Group name to get value from
         * @param key Key name to get value from
         * @return Tools error code and parsed string
         * @retval Success
         * @retval FileNotLoaded Key file is not loaded
         * @copydetails glibErrors
         */
        std::pair<std::string, ErrorCode> getStringFromFile(const std::string& group_name, const std::string& key) const;

        /*!
         * Get value from file
         * @param group_name Group name to get value from
         * @param key Key to get value from
         * @return Tools error code and parsed value(-s)
         * @retval Success
         * @retval FileNotLoaded key_file is not loaded
         * @copydetails glibErrors
         */
        std::pair<std::vector<std::string>, ErrorCode>
        getStringList(const std::string& group_name, const std::string& key) const;


    public:

        /*!
         * Constructor
         */
        Parser();

        /*!
         * Check if key file is opened
         * @return true if not nullptr, false otherwise
         */
        inline bool isOpen() const
        { return _keyFile != nullptr; }

        /*!
         * Close key file
         */
        inline void close()
        { _keyFile = nullptr; }

        /*!
         * Load config file
         * @param file Path to config file to be loaded
         * @return Tools error code
         * @retval Success
         * @retval GlibError Creating new key file failed
         * @retval LoadFailed Error while loading key file
         */
        ErrorCode loadConfigFile(const std::string& file = "Config.ini");

        /*!
         * Get vector of keys and values
         * @param group_name Group name to get keys and values from
         * @return Tools error code and vector of <key, value>
         * @retval Success
         * @retval FileNotLoaded _keyFile is not loaded
         * @copydetails glibErrors
         */
        std::pair<GroupInfo, ErrorCode> parseGroup(const std::string& group_name) const;

        /*!
         * Parse single option from key in group. Doesn't look for separators (;), but if there's "\;" in file, then fails
         * @tparam T Type to be parsed
         * @param group_name Group name to get value from
         * @param key Key to get value from
         * @return Tools error code and parsed value
         * @retval Success
         * @retval FileNotLoaded Config file isn't loaded
         * @copydetails glibErrors
         * @copydetails convertErrors
         */
        template <typename T>
        std::pair<T, ErrorCode> parseSingleOption(const std::string& group_name, const std::string& key) const;

        /*!
         * Parse multiple options from key in group. Looks for separators
         * @tparam T Type to be parsed
         * @param group_name Group name to get value from
         * @param key Key to get value from
         * @return Tools error code and parsed value
         * @retval Success
         * @copydetails glibErrors
         * @copydetails convertErrors
         */
        template <typename T>
        std::pair<std::vector<T>, ErrorCode>
        parseMultipleOptions(const std::string& group_name, const std::string& key) const;
    };

    extern Parser defaultParser;
}


template <typename T>
std::pair<std::vector<T>, Parse::ErrorCode> Parse::Parser::multipleConvert(const std::vector<std::string> &ret) const
{
    std::vector<T> res;
    res.reserve(ret.size());
    errno = 0;
    for(auto &str: ret)
    {
        auto resConvert = Convert<T>()(str);
        if (resConvert.second == Success)
            res.emplace_back(std::move(resConvert.first));
        else
            return {{}, resConvert.second};
    }
    return {res, Success};
}

template <typename T>
std::pair<T, Parse::ErrorCode> Parse::Parser::parseSingleOption(const std::string& group_name, const std::string& key) const
{
    auto ret = getStringFromFile(group_name, key);
    if(ret.second == Success)
    {
        errno = 0;
        return Convert<T>()(std::move(ret.first));
    }
    else
        return {{}, ret.second};
}

template <typename T>
std::pair<std::vector<T>, Parse::ErrorCode>
Parse::Parser::parseMultipleOptions(const std::string& group_name, const std::string& key) const
{
    auto str = getStringList(group_name, key);
    if(str.second == Success)
    {
        auto convertRes = multipleConvert<T>(str.first);
        if(convertRes.second != Success)
            return {{}, convertRes.second};
        return {std::move(convertRes.first), Success};
    }
    else
        return {{}, str.second};
}

template<>
class Parse::Convert<std::string>
{
public:
    std::pair<std::string, ErrorCode> operator()(const std::string &str) const;
};

template <>
class Parse::Convert<float>
{
public:
    std::pair<float, ErrorCode> operator()(const std::string &str) const;
};

template <>
class Parse::Convert<double>
{
public:
    std::pair<double, ErrorCode> operator()(const std::string &str) const;
};

template <>
class Parse::Convert<int>
{
public:
    std::pair<int, ErrorCode> operator()(const std::string &str) const;
};

template <>
class Parse::Convert<long double>
{
public:
    std::pair<long double, ErrorCode> operator()(const std::string &str) const;
};

template <>
class Parse::Convert<long>
{
public:
    std::pair<long, ErrorCode> operator()(const std::string &str) const;
};

template <>
class Parse::Convert<long long>
{
public:
    std::pair<long long, ErrorCode> operator()(const std::string &str) const;
};

template <>
class Parse::Convert<unsigned long>
{
public:
    std::pair<unsigned long, ErrorCode> operator()(const std::string &str) const;
};

template <>
class Parse::Convert<unsigned long long>
{
public:
    std::pair<unsigned long long, ErrorCode> operator()(const std::string &str) const;
};

template <>
class Parse::Convert<bool>
{
public:
    std::pair<bool, ErrorCode> operator()(const std::string &str) const;
};

template <typename Rep, typename RatioT>
class Parse::Convert<std::chrono::duration<Rep, RatioT>>
{
public:
    std::pair<std::chrono::duration<Rep, RatioT>, ErrorCode> operator()(const std::string &str) const
    {
        return {timeConversion::TimeConverter::stringToTime<std::chrono::duration<Rep, RatioT>>(str), Success};
    }
};

template <>
class Parse::Convert<char>
{
public:
    std::pair<char, ErrorCode> operator()(const std::string &str) const;
};

#endif //EXPLORATIONS_PARSER_H