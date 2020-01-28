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

#include "Parser.h"
#include <kerlog.h>

Parse::Parser Parse::defaultParser;


Parse::Parser::Parser(): _keyFile(nullptr, [](GKeyFile* ptr){ if (ptr != nullptr)g_key_file_free(ptr); }) {}

Parse::ErrorCode Parse::Parser::loadConfigFile(const std::string& file)
{
    KERLOG_DEBUG("Loading key file with name/path" + file + "' @ " + std::to_string((uint64_t) this));
    decltype(_keyFile) keyFile(g_key_file_new(), [](GKeyFile* ptr)
                {
                    if (ptr != nullptr)
                    g_key_file_free(ptr);
                });
    if(!keyFile)
    {   //GCOV_EXCL_START
        KERLOG_ERROR("Error creating new key file: " + std::to_string(GlibError));
        return GlibError;
        //GCOV_EXCL_STOP
    }
    g_autoptr(GError) glibError = nullptr;
    if (!g_key_file_load_from_file(keyFile.get(), file.c_str(), G_KEY_FILE_NONE, &glibError))
    {
        KERLOG_ERROR("Error loading key file: " + std::string(glibError->message));
        return LoadFailed;
    }
    _keyFile = std::move(keyFile);
    KERLOG_DEBUG("Key file with name/path '" + file + "' loaded. Returning Success");
    return Success;
}


Parse::ErrorCode
Parse::Parser::glibErrorCheck(const std::string& group_name, const std::string& key, GError_autoptr error) const
{
    if (g_error_matches(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_GROUP_NOT_FOUND))
    {
        KERLOG_ERROR("Group '" + group_name + "' doesn't exist in key file: " + std::string(error->message)
              + ".Returning value: GroupNotFound");
        return GroupNotFound;
    }
    else if (g_error_matches(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    {
        KERLOG_ERROR("Key '" + key + "' doesn't exist in key file: " + std::string(error->message)
              + ".Returning value: KeyNotFound");
        return KeyNotFound;
    }
    else
    {
        KERLOG_ERROR("Glib error occurred while parsing an option. Glib error message: " + std::string(error->message)
              + ".Returning value: GlibError");
        return GlibError;
    }
}


std::pair<std::string, Parse::ErrorCode>
Parse::Parser::getStringFromFile(const std::string& group_name, const std::string& key) const
{
    KERLOG_DEBUG("Parsing single key string " + key + " from group " + group_name + " @ " + std::to_string((uint64_t) this));
    if(!_keyFile)
    {
        KERLOG_ERROR("_keyFile is not loaded. Returning FileNotLoaded");
        return {"", FileNotLoaded};
    }
    g_autoptr(GError) error = nullptr;
    std::unique_ptr<char, void(*)(char*)> value(g_key_file_get_string(_keyFile.get(), group_name.c_str(), key.c_str(), &error),
                                                [](char* str) { free(str); } );
    if(error != nullptr)
        return {"", glibErrorCheck(group_name, key, error)};
    KERLOG_DEBUG("Parsing key strings " + key + " from group " + group_name + " completed. Returning Success");
    return {value.get(), Success};
}


std::pair<std::vector<std::string>, Parse::ErrorCode>
Parse::Parser::getStringList(const std::string& group_name, const std::string& key) const
{
    KERLOG_DEBUG("Parsing key strings " + key + " from group " + group_name + " @ " + std::to_string((uint64_t) this));
    if(!_keyFile)
    {
        KERLOG_ERROR("_keyFile is not loaded. Returning FileNotLoaded");
        return {{}, FileNotLoaded};
    }
    gsize size = 0;
    g_autoptr(GError) error = nullptr;
    std::unique_ptr<gchar*, void(*)(gchar**)> value(g_key_file_get_string_list(_keyFile.get(), group_name.c_str(),
            key.c_str(), &size, &error), [](gchar** ptr)
            {
                if (ptr != nullptr)
                    g_strfreev(ptr);
            });
    if(error != nullptr)
        return {{}, glibErrorCheck(group_name, key, error)};
    std::vector<std::string> res(value.get(), value.get() + size);
    KERLOG_DEBUG("Parsing key strings " + key + " from group " + group_name + " completed. Returning Success");
    return {res, Success};
}


std::pair<Parse::GroupInfo, Parse::ErrorCode> Parse::Parser::parseGroup(const std::string& group_name) const
{
    KERLOG_DEBUG("Parsing group '" + group_name + "' @ " + std::to_string((uint64_t) this));
    if(!_keyFile)
    {
        KERLOG_ERROR("_keyFile is not loaded. Returning FileNotLoaded");
        return {{}, FileNotLoaded};
    }
    g_autoptr(GError) error = nullptr;
    std::unique_ptr<gchar*,  void(*)(gchar**)> g_keys(g_key_file_get_keys(_keyFile.get(), group_name.c_str(), nullptr,
                                               &error), [](gchar** ptr){ if (ptr != nullptr) g_strfreev(ptr);});
    if(error != nullptr && g_error_matches(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_GROUP_NOT_FOUND))
    {
        KERLOG_ERROR("Error finding group '" + group_name + "' in key file: " + std::string(error->message)
              + "Returning value: GroupNotFound");
        return {{}, GroupNotFound};
    }
    GroupInfo groupInfo;
    for(int i = 0; g_keys.get()[i]; i++)
    {
        std::pair<std::vector<std::string>, Parse::ErrorCode> value;
        if((value = getStringList(group_name, g_keys.get()[i])).second == Success)
            groupInfo.emplace_back(g_keys.get()[i], std::move(value.first));
        else
        {
            KERLOG_ERROR("Couldn't parse an option. Returning value: error code from parseOption method");
            return {{}, value.second};
        }
    }
    KERLOG_DEBUG("Parsing group '" + group_name + "' completed. Returning value: Success, keys vector");
    return {groupInfo, Success};
}


Parse::ErrorCode convertCheck(const std::string &str, char *pend)
{
    if(pend != str)
    {
        if(errno != ERANGE)
            return Parse::Success;
        else
            return Parse::OutOfRange;
    }
    else
        return Parse::IncorrectFileContainment;
}


template <class T>
std::pair<T, Parse::ErrorCode> convertCommonType(const char *str)
{
    static_assert(std::is_same_v<T, float>       || std::is_same_v<T, double>        ||
                  std::is_same_v<T, long double> || std::is_same_v<T, long>          ||
                  std::is_same_v<T, long long>   || std::is_same_v<T, unsigned long> ||
                  std::is_same_v<T, unsigned long long>);
    char *pend;
    T ret;
    if constexpr (std::is_same_v<T, float>)
        ret = std::strtof(str, &pend);
    else if constexpr (std::is_same_v<T, double>)
        ret = std::strtod(str, &pend);
    else if constexpr (std::is_same_v<T, long double>)
        ret = std::strtold(str, &pend);
    else if constexpr (std::is_same_v<T, long>)
        ret = std::strtol(str, &pend, 10);
    else if constexpr (std::is_same_v<T, long long>)
        ret = std::strtoll(str, &pend, 10);
    else if constexpr (std::is_same_v<T, unsigned long>)
        ret = std::strtoul(str, &pend, 10);
    else if constexpr (std::is_same_v<T, unsigned long long>)
        ret = std::strtoull(str, &pend, 10);
    Parse::ErrorCode convertError = convertCheck(str, pend);
    if (convertError == Parse::Success)
        return {ret, Parse::Success};
    else
        return {{}, convertError};
}

std::pair<std::string, Parse::ErrorCode> Parse::Convert<std::string>::operator()(const std::string &str) const
{
    return {str, Success};
}

std::pair<int, Parse::ErrorCode> Parse::Convert<int>::operator()(const std::string &str) const
{
    auto res = convertCommonType<double>(str.c_str());
    if (res.second == Success)
        return {static_cast<int>(res.first), Success};
    else
        return {{}, res.second};
}

std::pair<float, Parse::ErrorCode> Parse::Convert<float>::operator()(const std::string &str) const
{
    return convertCommonType<float>(str.c_str());
}

std::pair<double, Parse::ErrorCode> Parse::Convert<double>::operator()(const std::string &str) const
{
    return convertCommonType<double>(str.c_str());
}

std::pair<long double, Parse::ErrorCode> Parse::Convert<long double>::operator()(const std::string &str) const
{
    return convertCommonType<long double>(str.c_str());
}

std::pair<long, Parse::ErrorCode> Parse::Convert<long>::operator()(const std::string &str) const
{
    return convertCommonType<long>(str.c_str());
}

std::pair<long long, Parse::ErrorCode> Parse::Convert<long long>::operator()(const std::string &str) const
{
    return convertCommonType<long long>(str.c_str());
}

std::pair<unsigned long, Parse::ErrorCode> Parse::Convert<unsigned long>::operator()(const std::string &str) const
{
    return convertCommonType<unsigned long>(str.c_str());
}

std::pair<unsigned long long, Parse::ErrorCode> Parse::Convert<unsigned long long>::operator()(const std::string &str) const
{
    return convertCommonType<unsigned long long>(str.c_str());
}

std::pair<bool, Parse::ErrorCode> Parse::Convert<bool>::operator()(const std::string &str) const
{
    std::string res = str;
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    if(res == "false" || res == "0")
        return {false, Success};
    else if(res == "true" || res == "1")
        return {true, Success};
    else
        return {{}, IncorrectFileContainment};
}

std::pair<char, Parse::ErrorCode> Parse::Convert<char>::operator()(const std::string &str) const
{
    if(str.size() == 1)
        return {str[0], Success};
    if(str.size() == 2 && str[0] == '\\')
    {
        if(str[1] == 'n')
            return {'\n', Success};
        /// Insert new escape sequences if needed
    }
    if(str.empty())
        return {' ', Success};
    return {{}, IncorrectFileContainment};
}