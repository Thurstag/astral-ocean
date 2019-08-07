// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/vulkan/engine/settings.h>
#include <gtest/gtest.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

#include "../helpers/tests.h"

namespace ao::test {
    class _EngineSettings : public vulkan::EngineSettings {
       public:
        _EngineSettings() = default;
        virtual ~_EngineSettings() = default;

        std::map<std::string, std::pair<std::pair<size_t, std::string>, void*>>& Values() {
            return this->values;
        }

        std::map<std::string, std::string>& StrValues() {
            return this->str_values;
        }
    };

    TEST(EngineSettings, Default) {
        ao::core::Logger::Init();
        vulkan::EngineSettings s;

        ASSERT_FALSE(s.exists("test"));
        ASSERT_TRUE(s.get<bool>("test", std::make_optional(true)));
        ASSERT_TRUE(s.exists("test"));
    }

    TEST(EngineSettings, Update) {
        vulkan::EngineSettings s;
        s.get<bool>("test") = false;

        ASSERT_FALSE(s.get<bool>("test"));

        s.get<bool>("test") = true;
        ASSERT_TRUE(s.get<bool>("test"));
    }

    TEST(EngineSettings, UpdateByRef) {
        vulkan::EngineSettings s;
        s.get<bool>("test") = false;

        bool& ref = s.get<bool>("test");
        ASSERT_FALSE(ref);
        ref = true;

        ASSERT_TRUE(s.get<bool>("test"));
    }

    TEST(EngineSettings, WrongCast) {
        // Create a new output for logger
        std::stringbuf ss;
        std::ostream os(&ss);
        boost::log::add_console_log(os, boost::log::keywords::format = "%Message%", boost::log::keywords::auto_flush = true);

        vulkan::EngineSettings s;
        s.get<bool>("test") = false;

        s.get<uint32_t>("test");

        // Assert log
        ASSERT_TRUE(ss.str().find("Cast") != std::string::npos);
    }

    TEST(EngineSettings, EmplaceString) {
        _EngineSettings s;

        ASSERT_TRUE(s.Values().empty());
        ASSERT_TRUE(s.StrValues().empty());

        s.get<std::string>("key") = std::string("value");

        ASSERT_TRUE(s.Values().empty());
        ASSERT_TRUE(s.exists("key"));
        ASSERT_FALSE(s.StrValues().empty());
    }

    TEST(EngineSettings, SameKey) {
        _EngineSettings s;

        ASSERT_TRUE(s.Values().empty());
        ASSERT_TRUE(s.StrValues().empty());

        s.get<std::string>("key") = std::string("value");
        s.get<int>("key") = 1;

        ASSERT_FALSE(s.Values().empty());
        ASSERT_FALSE(s.StrValues().empty());
        ASSERT_TRUE(s.exists("key"));

        ASSERT_STREQ("value", s.get<std::string>("key").c_str());
        ASSERT_EQ(1, s.get<int>("key"));
    }
}  // namespace ao::test
