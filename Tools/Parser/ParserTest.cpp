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
#include <Parser.h>

#define MAX_NUM(type) std::numeric_limits<type>::max()
#define MAX_NUM1(type) std::numeric_limits<type>::max() - 1u
#define SMAX_NUM(type) std::to_string(MAX_NUM(type))
#define SMAX_NUM1(type) std::to_string(MAX_NUM1(type))
#define MULTI config.parseMultipleOptions
#define SINGLE config.parseSingleOption

TEST_CASE("ParserTest")
{
    SECTION("LoadNonExistingFile", "[Parse]")
    {
        Parse::Parser config;
        REQUIRE(config.loadConfigFile("set") == Parse::LoadFailed);
    }

    SECTION("ParseOptionsWhileKeyFileIsNullptr", "[Parse]")
    {
        Parse::Parser config;
        REQUIRE(SINGLE<std::string>("Common", "field1").second == Parse::FileNotLoaded);
        REQUIRE(MULTI<std::string>("Common", "field1").second == Parse::FileNotLoaded);
        REQUIRE(config.parseGroup("Common").second == Parse::FileNotLoaded);
    }

    SECTION("Constructor", "[Parse]")
    {
        Parse::Parser config;
        REQUIRE(!config.isOpen());
    }

    SECTION("ConstructExistingFile_DefaultConstructor", "[Parse]")
    {
        std::fstream file;
        bool flag = false;
        file.open("Parse.ini");
        if (!file.is_open()) {
            file.close();
            file.open("Parse.ini", std::fstream::app);
            flag = true;
        }
        file.close();
        Parse::Parser config;
        if(flag)
            remove("Parse.ini");
    }

    SECTION("ParsingFile", "[Parse]")
    {
        WHEN("File and Parse configect are created")
        {
            std::fstream file;
            std::string shortFileName = "Parse";
            std::string fileName = shortFileName + ".ini";
            std::vector<std::pair<std::string, std::string>> group_options;
            std::string value;
            size_t iniFileNumber = 0;
            INFO("Editing filepath");
            file.open(fileName);
            while (file.is_open())
            {
                ++iniFileNumber;
                file.close();
                fileName = shortFileName + std::to_string(iniFileNumber) + "TEST.ini";
                file.open(fileName);
            }
            file.close();
            INFO("Editing filepath completed");

            file.open(fileName, std::fstream::app);
            INFO("Editing file content");
            file <<
                 "[Common]\n"                     /// "->" means "parses by parseMultipleOptions<std::string> method as"
                 "field1=field1Value\n"                     // -> "field1Value"
                 "field2=;\n"                               // -> ""
                 "field3=field3Value1;field3Value2\n"       // -> "field3Value1", "field3Value2"
                 "field4=field4Value;\n"                    // -> "field4Value"
                 "field5=field5Value1\\;field5Value2\n"     // -> "field5Value1;field5Value2"
                 "field6=field6Value1\\\\field6Value2\n"    // "field6Value1\\field6Value2"   -> "field6Value1\field6Value2"
                 "field7=field7Value1\\\\;field7Value2\n"   // "field7Value1\\;field7Value2"  -> "field7Value1\", "field7Value2"
                 "field8=field8Value1\\\\\\;field8Value2\n" // "field8Value1\\\;field8Value2" -> "field8Value1\;field8Value2"
                 "field9=;\\\\;\\\\\\;\\\\\n"               // ";\\;\\\;\\" -> "", "\", "\;\"
                 "field10=\\\\\n"                           // "\\" -> "\"
                 "field11=;\n"                              // -> ""
                 "field12=;;\n"                             // -> "", ""
                 "field13=;\\\\;;\\\\\\;\n"                 // ";\\;;\\\;" -> "", "\", "", "\;"

                 "[float]\n"
                 "test=1.23;5.23;0\n"
                 "errtest=546842151212454121512165132165416548212146\n"
                 "incorrect=abc\n"

                 "[int]\n"
                 "incorrect=abc\n"

                 "[bool]\n"
                 "incorrect=abc\n"

                 "[ConvertTypesTest]\n"
                 "stringType=stringContainment\n"
                 "boolType1=0\n"
                 "boolType2=1\n"
                 "boolType3=false\n"
                 "boolType4=true\n"
                 "boolTypeCase=TrUe\n"
                 "intType="              + SMAX_NUM(int) + "\n"                <<
                 "floatType="            + SMAX_NUM(float) + "\n"              <<
                 "doubleType="           + SMAX_NUM(double) + "\n"             <<
                 "longdoubleType="       + SMAX_NUM(long double) + "\n"        <<
                 "longType="             + SMAX_NUM(long) + "\n"               <<
                 "longlongType="         + SMAX_NUM(long long) + "\n"          <<
                 "unsignedlongType="     + SMAX_NUM(unsigned long) + "\n"      <<
                 "unsignedlonglongType=" + SMAX_NUM(unsigned long long) + "\n" <<
                 "stringTypeMultiple=stringContainment1;stringContainment2\n" <<
                 "intTypeMultiple="              + SMAX_NUM(int)                + ";" + SMAX_NUM1(int)   + "\n" <<
                 "floatTypeMultiple="            + SMAX_NUM(float)              + ";" + SMAX_NUM1(float) + "\n" <<
                 "doubleTypeMultiple="           + SMAX_NUM(double)             + ";" + SMAX_NUM1(double) + "\n" <<
                 "longdoubleTypeMultiple="       + SMAX_NUM(long double)        + ";" + SMAX_NUM1(long double) + "\n" <<
                 "longTypeMultiple="             + SMAX_NUM(long)               + ";" + SMAX_NUM1(long) + "\n" <<
                 "longlongTypeMultiple="         + SMAX_NUM(long long)          + ";" + SMAX_NUM1(long long) + "\n" <<
                 "unsignedlongTypeMultiple="     + SMAX_NUM(unsigned long)      + ";" + SMAX_NUM1(unsigned long) + "\n" <<
                 "unsignedlonglongTypeMultiple=" + SMAX_NUM(unsigned long long) + ";" + SMAX_NUM1(unsigned long long)+"\n"<<
                 "boolTypeMultiple=true;0;false;1\n"
                 "boolTypeMultipleCaseSensitive=TRuE;0;fAlSe;1\n"

                 "[CommonIncorrect]\n"
                 "field14=\\\n"

                 "[TimeConversion]\n"
                 "nanoseconds=1000000000N\n" // == 1S
                 "seconds=60S\n"
                 "minutes=60M\n"
                 "hours=24H\n"
                 "days=7d\n"
                 "weeks=1w";

            file.close();
            INFO("Editing file content completed");

            Parse::Parser config;
            config.loadConfigFile(fileName);
            config.loadConfigFile(fileName); //Check for correct free of existing _keyFile

            THEN("WrongParse")
            {
                std::pair<Parse::GroupInfo, Parse::ErrorCode > parseGroupRes;
                REQUIRE((parseGroupRes = config.parseGroup("Comon")).second == Parse::GroupNotFound);
                REQUIRE((parseGroupRes = config.parseGroup("CommonIncorrect")).second == Parse::GlibError);
                auto multipleOptionerr = MULTI<float>("float", "errtest");
                REQUIRE(multipleOptionerr.second == Parse::OutOfRange);
                auto multipleOptionIncorrectFloat = MULTI<float>("float", "incorrect");
                REQUIRE(multipleOptionIncorrectFloat.second == Parse::IncorrectFileContainment);
                auto multipleOptionIncorrectInt = MULTI<int>("int", "incorrect");
                REQUIRE(multipleOptionIncorrectInt.second == Parse::IncorrectFileContainment);
                auto multipleOptionIncorrectBool = MULTI<bool>("bool", "incorrect");
                REQUIRE(multipleOptionIncorrectBool.second == Parse::IncorrectFileContainment);
                auto singleOptionGroupNotFound = SINGLE<std::string>("Comon", "field1");
                REQUIRE(singleOptionGroupNotFound.second == Parse::GroupNotFound);
                auto singleOptionKeyNotFound = SINGLE<std::string>("Common", "fieldd");
                REQUIRE(singleOptionKeyNotFound.second == Parse::KeyNotFound);
            }

            THEN("ParseGroup_Common")
            {
                std::pair<Parse::GroupInfo, Parse::ErrorCode> parseGroupRes;

                REQUIRE((parseGroupRes = config.parseGroup("Common")).second == Parse::Success);
                REQUIRE(parseGroupRes.first.size() == 13);

                auto singleOption = SINGLE<std::string>("Common", "field1");
                REQUIRE(singleOption.second == Parse::Success);
                REQUIRE(singleOption.first == "field1Value");
                auto multipleOption = MULTI<std::string>("Common", "field1");
                REQUIRE(multipleOption.second == Parse::Success);
                REQUIRE(multipleOption.first == parseGroupRes.first[0].second);
                REQUIRE(multipleOption.first.size() == 1);
                REQUIRE(multipleOption.first[0] == "field1Value");

                auto singleOption2 = SINGLE<std::string>("Common", "field2");
                REQUIRE(singleOption2.second == Parse::Success);
                REQUIRE(singleOption2.first == ";");
                auto multipleOption2 = MULTI<std::string>("Common", "field2");
                REQUIRE(multipleOption2.second == Parse::Success);
                REQUIRE(multipleOption2.first == parseGroupRes.first[1].second);
                REQUIRE(multipleOption2.first.size() == 1);
                REQUIRE(multipleOption2.first[0].empty());

                auto singleOption3 = SINGLE<std::string>("Common", "field3");
                REQUIRE(singleOption3.second == Parse::Success);
                REQUIRE(singleOption3.first == "field3Value1;field3Value2");
                auto multipleOption3 = MULTI<std::string>("Common", "field3");
                REQUIRE(multipleOption3.second == Parse::Success);
                REQUIRE(multipleOption3.first == parseGroupRes.first[2].second);
                REQUIRE(multipleOption3.first.size() == 2);
                REQUIRE(multipleOption3.first[0] == "field3Value1");
                REQUIRE(multipleOption3.first[1] == "field3Value2");

                auto singleOption4 = SINGLE<std::string>("Common", "field4");
                REQUIRE(singleOption4.second == Parse::Success);
                REQUIRE(singleOption4.first == "field4Value;");
                auto multipleOption4 = MULTI<std::string>("Common", "field4");
                REQUIRE(multipleOption4.second == Parse::Success);
                REQUIRE(multipleOption4.first == parseGroupRes.first[3].second);
                REQUIRE(multipleOption4.first.size() == 1);
                REQUIRE(multipleOption4.first[0] == "field4Value");

                auto singleOption5 = SINGLE<std::string>("Common", "field5");
                REQUIRE(singleOption5.second == Parse::GlibError);
                auto multipleOption5 = MULTI<std::string>("Common", "field5");
                REQUIRE(multipleOption5.second == Parse::Success);
                REQUIRE(multipleOption5.first == parseGroupRes.first[4].second);
                REQUIRE(multipleOption5.first.size() == 1);
                REQUIRE(multipleOption5.first[0] == "field5Value1;field5Value2");

                auto singleOption6 = SINGLE<std::string>("Common", "field6");
                REQUIRE(singleOption6.second == Parse::Success);
                REQUIRE(singleOption6.first == "field6Value1\\field6Value2");
                auto multipleOption6 = MULTI<std::string>("Common", "field6");
                REQUIRE(multipleOption6.second == Parse::Success);
                REQUIRE(multipleOption6.first == parseGroupRes.first[5].second);
                REQUIRE(multipleOption6.first.size() == 1);
                REQUIRE(multipleOption6.first[0] == "field6Value1\\field6Value2");

                auto singleOption7 = SINGLE<std::string>("Common", "field7");
                REQUIRE(singleOption7.second == Parse::Success);
                REQUIRE(singleOption7.first == "field7Value1\\;field7Value2");
                auto multipleOption7 = MULTI<std::string>("Common", "field7");
                REQUIRE(multipleOption7.second == Parse::Success);
                REQUIRE(multipleOption7.first == parseGroupRes.first[6].second);
                REQUIRE(multipleOption7.first.size() == 2);
                REQUIRE(multipleOption7.first[0] == "field7Value1\\");
                REQUIRE(multipleOption7.first[1] == "field7Value2");

                auto singleOption8 = SINGLE<std::string>("Common", "field8");
                REQUIRE(singleOption8.second == Parse::GlibError);
                auto multipleOption8 = MULTI<std::string>("Common", "field8");
                REQUIRE(multipleOption8.second == Parse::Success);
                REQUIRE(multipleOption8.first == parseGroupRes.first[7].second);
                REQUIRE(multipleOption8.first.size() == 1);
                REQUIRE(multipleOption8.first[0] == "field8Value1\\;field8Value2");

                auto singleOption9 = SINGLE<std::string>("Common", "field9");
                REQUIRE(singleOption9.second == Parse::GlibError);
                auto multipleOption9 = MULTI<std::string>("Common", "field9");
                REQUIRE(multipleOption9.second == Parse::Success);
                REQUIRE(multipleOption9.first == parseGroupRes.first[8].second);
                REQUIRE(multipleOption9.first.size() == 3);
                REQUIRE(multipleOption9.first[0].empty());
                REQUIRE(multipleOption9.first[1] == "\\");
                REQUIRE(multipleOption9.first[2] == "\\;\\");

                auto singleOption10 = SINGLE<std::string>("Common", "field10");
                REQUIRE(singleOption10.second == Parse::Success);
                REQUIRE(singleOption10.first == "\\");
                auto multipleOption10 = MULTI<std::string>("Common", "field10");
                REQUIRE(multipleOption10.second == Parse::Success);
                REQUIRE(multipleOption10.first == parseGroupRes.first[9].second);
                REQUIRE(multipleOption10.first.size() == 1);
                REQUIRE(multipleOption10.first[0] == "\\");

                auto singleOption11 = SINGLE<std::string>("Common", "field11");
                REQUIRE(singleOption11.second == Parse::Success);
                REQUIRE(singleOption11.first == ";");
                auto multipleOption11 = MULTI<std::string>("Common", "field11");
                REQUIRE(multipleOption11.second == Parse::Success);
                REQUIRE(multipleOption11.first == parseGroupRes.first[10].second);
                REQUIRE(multipleOption11.first.size() == 1);
                REQUIRE(multipleOption11.first[0].empty());

                auto singleOption12 = SINGLE<std::string>("Common", "field12");
                REQUIRE(singleOption12.second == Parse::Success);
                REQUIRE(singleOption12.first == ";;");
                auto multipleOption12 = MULTI<std::string>("Common", "field12");
                REQUIRE(multipleOption12.second == Parse::Success);
                REQUIRE(multipleOption12.first == parseGroupRes.first[11].second);
                REQUIRE(multipleOption12.first.size() == 2);
                REQUIRE(multipleOption12.first[0].empty());
                REQUIRE(multipleOption12.first[1].empty());

                auto singleOption13 = SINGLE<std::string>("Common", "field13");
                REQUIRE(singleOption13.second == Parse::GlibError);
                auto multipleOption13 = MULTI<std::string>("Common", "field13");
                REQUIRE(multipleOption13.second == Parse::Success);
                REQUIRE(multipleOption13.first == parseGroupRes.first[12].second);
                REQUIRE(multipleOption13.first.size() == 4);
                REQUIRE(multipleOption13.first[0].empty());
                REQUIRE(multipleOption13.first[1] == "\\");
                REQUIRE(multipleOption13.first[2].empty());
                REQUIRE(multipleOption13.first[3] == "\\;");
            }

            THEN("ConvertCommonTypes")
            {
                SECTION("ParseSingleValue")
                {
                    INFO("Converting types single");
                    auto singleString = SINGLE<std::string>("ConvertTypesTest", "stringType");
                    REQUIRE(singleString.second == Parse::Success);
                    REQUIRE(singleString.first == "stringContainment");
                    auto singleInt = SINGLE<int>("ConvertTypesTest", "intType");
                    REQUIRE(singleInt.second == Parse::Success);
                    REQUIRE(singleInt.first == MAX_NUM(int));
                    auto singleFloat = SINGLE<float>("ConvertTypesTest", "floatType");
                    REQUIRE(singleFloat.second == Parse::Success);
                    REQUIRE(singleFloat.first == MAX_NUM(float));
                    auto singleDouble = SINGLE<double>("ConvertTypesTest", "doubleType");
                    REQUIRE(singleDouble.second == Parse::Success);
                    REQUIRE(singleDouble.first == MAX_NUM(double));
                    auto singleLongDouble = SINGLE<long double>("ConvertTypesTest", "longdoubleType");
                    REQUIRE(singleLongDouble.second == Parse::Success);
                    REQUIRE(singleLongDouble.first == MAX_NUM(long double));
                    auto singleLong = SINGLE<long>("ConvertTypesTest", "longType");
                    REQUIRE(singleLong.second == Parse::Success);
                    REQUIRE(singleLong.first == MAX_NUM(long));
                    auto singleLongLong = SINGLE<long long>("ConvertTypesTest", "longlongType");
                    REQUIRE(singleLongLong.second == Parse::Success);
                    REQUIRE(singleLongLong.first == MAX_NUM(long long));
                    auto singleUnsignedLong = SINGLE<unsigned long>("ConvertTypesTest", "unsignedlongType");
                    REQUIRE(singleUnsignedLong.second == Parse::Success);
                    REQUIRE(singleUnsignedLong.first == MAX_NUM(unsigned long));
                    auto singleUnsignedLongLong = SINGLE<unsigned long long>("ConvertTypesTest", "unsignedlonglongType");
                    REQUIRE(singleUnsignedLongLong.second == Parse::Success);
                    REQUIRE(singleUnsignedLongLong.first == MAX_NUM(unsigned long long));
                    auto singleBool1 = SINGLE<bool>("ConvertTypesTest", "boolType1");
                    REQUIRE(singleBool1.second == Parse::Success);
                    REQUIRE(!singleBool1.first);
                    auto singleBool2 = SINGLE<bool>("ConvertTypesTest", "boolType2");
                    REQUIRE(singleBool2.second == Parse::Success);
                    REQUIRE(singleBool2.first);
                    auto singleBool3 = SINGLE<bool>("ConvertTypesTest", "boolType3");
                    REQUIRE(singleBool3.second == Parse::Success);
                    REQUIRE(!singleBool3.first);
                    auto singleBool4 = SINGLE<bool>("ConvertTypesTest", "boolType4");
                    REQUIRE(singleBool4.second == Parse::Success);
                    REQUIRE(singleBool4.first);
                    auto singleBoolCase = SINGLE<bool>("ConvertTypesTest", "boolTypeCase");
                    REQUIRE(singleBoolCase.second == Parse::Success);
                    REQUIRE(singleBoolCase.first);
                }

                SECTION("ParseMultipleValues")
                {
                    auto multipleOption0 = MULTI<float>("float", "test");
                    REQUIRE(multipleOption0.second == Parse::Success);
                    REQUIRE(multipleOption0.first.size() == 3);
                    REQUIRE(multipleOption0.first[0] == float(1.23));
                    REQUIRE(multipleOption0.first[1] == float(5.23));
                    REQUIRE(multipleOption0.first[2] == float(0));

                    auto multipleString = MULTI<std::string>("ConvertTypesTest", "stringTypeMultiple");
                    REQUIRE(multipleString.second == Parse::Success);
                    REQUIRE(multipleString.first.size() == 2);
                    REQUIRE(multipleString.first[0] == "stringContainment1");
                    REQUIRE(multipleString.first[1] == "stringContainment2");
                    auto multipleInt = MULTI<int>("ConvertTypesTest", "intTypeMultiple");
                    REQUIRE(multipleInt.second == Parse::Success);
                    REQUIRE(multipleInt.first.size() == 2);
                    REQUIRE(multipleInt.first[0] == MAX_NUM(int));
                    REQUIRE(multipleInt.first[1] == MAX_NUM1(int));
                    auto multipleFloat = MULTI<float>("ConvertTypesTest", "floatTypeMultiple");
                    REQUIRE(multipleFloat.second == Parse::Success);
                    REQUIRE(multipleFloat.first.size() == 2);
                    REQUIRE(multipleFloat.first[0] == MAX_NUM(float));
                    REQUIRE(multipleFloat.first[1] == MAX_NUM1(float));
                    auto multipleDouble = MULTI<double>("ConvertTypesTest", "doubleTypeMultiple");
                    REQUIRE(multipleDouble.second == Parse::Success);
                    REQUIRE(multipleDouble.first.size() == 2);
                    REQUIRE(multipleDouble.first[0] == MAX_NUM(double));
                    REQUIRE(multipleDouble.first[1] == MAX_NUM1(double));
                    auto multipleLongDouble = MULTI<long double>("ConvertTypesTest", "longdoubleTypeMultiple");
                    REQUIRE(multipleLongDouble.second == Parse::Success);
                    REQUIRE(multipleLongDouble.first.size() == 2);
                    REQUIRE(multipleLongDouble.first[0] == MAX_NUM(long double));
                    REQUIRE(multipleLongDouble.first[1] == MAX_NUM1(long double));
                    auto multipleLong = MULTI<long>("ConvertTypesTest", "longTypeMultiple");
                    REQUIRE(multipleLong.second == Parse::Success);
                    REQUIRE(multipleLong.first.size() == 2);
                    REQUIRE(multipleLong.first[0] == MAX_NUM(long));
                    REQUIRE(multipleLong.first[1] == MAX_NUM1(long));
                    auto multipleLongLong = MULTI<long long>("ConvertTypesTest", "longlongTypeMultiple");
                    REQUIRE(multipleLongLong.second == Parse::Success);
                    REQUIRE(multipleLongLong.first.size() == 2);
                    REQUIRE(multipleLongLong.first[0] == MAX_NUM(long long));
                    REQUIRE(multipleLongLong.first[1] == MAX_NUM1(long long));
                    auto multipleUnsignedLong = MULTI<unsigned long>("ConvertTypesTest", "unsignedlongTypeMultiple");
                    REQUIRE(multipleUnsignedLong.second == Parse::Success);
                    REQUIRE(multipleUnsignedLong.first.size() == 2);
                    REQUIRE(multipleUnsignedLong.first[0] == MAX_NUM(unsigned long));
                    REQUIRE(multipleUnsignedLong.first[1] == MAX_NUM1(unsigned long));
                    auto multipleUnsignedLongLong = MULTI<unsigned long long>(
                            "ConvertTypesTest", "unsignedlonglongTypeMultiple");
                    REQUIRE(multipleUnsignedLongLong.second == Parse::Success);
                    REQUIRE(multipleUnsignedLongLong.first.size() == 2);
                    REQUIRE(multipleUnsignedLongLong.first[0] == MAX_NUM(unsigned long long));
                    REQUIRE(multipleUnsignedLongLong.first[1] == MAX_NUM1(unsigned long long));
                    auto multipleBool = MULTI<bool>("ConvertTypesTest", "boolTypeMultiple");
                    REQUIRE(multipleBool.second == Parse::Success);
                    REQUIRE(multipleBool.first.size() == 4);
                    REQUIRE(multipleBool.first[0]);
                    REQUIRE(!multipleBool.first[1]);
                    REQUIRE(!multipleBool.first[2]);
                    REQUIRE(multipleBool.first[3]);
                    auto multipleBoolCaseSensitive = MULTI<bool>("ConvertTypesTest", "boolTypeMultipleCaseSensitive");
                    REQUIRE(multipleBoolCaseSensitive.second == Parse::Success);
                    REQUIRE(multipleBoolCaseSensitive.first.size() == 4);
                    REQUIRE(multipleBoolCaseSensitive.first[0]);
                    REQUIRE(!multipleBoolCaseSensitive.first[1]);
                    REQUIRE(!multipleBoolCaseSensitive.first[2]);
                    REQUIRE(multipleBoolCaseSensitive.first[3]);
                }
            }


            SECTION("Parse durations")
            {
                auto singleDurationNano = SINGLE<std::chrono::seconds>("TimeConversion", "nanoseconds");
                REQUIRE(singleDurationNano.second == Parse::Success);
                REQUIRE(singleDurationNano.first == std::chrono::seconds(1));
                auto singleDurationSeconds = SINGLE<std::chrono::minutes>("TimeConversion", "seconds");
                REQUIRE(singleDurationSeconds.second == Parse::Success);
                REQUIRE(singleDurationSeconds.first == timeConversion::TimeConverter::minutes(1));
                auto singleDurationMinutes = SINGLE<std::chrono::hours>("TimeConversion", "minutes");
                REQUIRE(singleDurationMinutes.second == Parse::Success);
                REQUIRE(singleDurationMinutes.first == timeConversion::TimeConverter::hours(1));
                auto singleDurationHours = SINGLE<timeConversion::TimeConverter::days>("TimeConversion", "hours");
                REQUIRE(singleDurationHours.second == Parse::Success);
                REQUIRE(singleDurationHours.first == timeConversion::TimeConverter::days(1));
                auto singleDurationDays = SINGLE<timeConversion::TimeConverter::weeks>("TimeConversion", "days");
                REQUIRE(singleDurationDays.second == Parse::Success);
                REQUIRE(singleDurationDays.first == timeConversion::TimeConverter::weeks(1));
                auto singleDurationWeeks = SINGLE<timeConversion::TimeConverter::days>("TimeConversion", "weeks");
                REQUIRE(singleDurationWeeks.second == Parse::Success);
                REQUIRE(singleDurationWeeks.first == timeConversion::TimeConverter::days(7));
            }

            config.close();
            INFO("Removing file");
            remove(fileName.c_str());
            INFO("GetVariousInfoFromExistingFile Succeeded");
        }
    }
}