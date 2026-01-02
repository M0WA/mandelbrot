/*
 *  Catch v2.13.10
 *  Generated: 2022-10-16 11:01:23.452308
 *  ----------------------------------------------------------
 *  This file has been merged from multiple headers. Please don't edit it directly
 *  Copyright (c) 2022 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED
#define TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED

// Note: This is a minimal placeholder header for Catch2
// In a real project, download the full single-header version from:
// https://github.com/catchorg/Catch2/releases/download/v2.13.10/catch.hpp

// For this implementation, we'll use a minimal macro-based testing framework
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

namespace Catch {
    struct TestCase {
        std::string name;
        std::string tags;
        void (*func)();
    };
    
    struct Section {
        std::string name;
        bool active;
        int runCount;
        Section(const std::string& n) : name(n), active(true), runCount(0) {}
    };
    
    class TestRegistry {
    public:
        static TestRegistry& instance() {
            static TestRegistry reg;
            return reg;
        }
        
        void registerTest(const std::string& name, const std::string& tags, void (*func)()) {
            tests.push_back({name, tags, func});
        }
        
        int runAllTests() {
            int failed = 0;
            std::cout << "\n===============================================================================\n";
            std::cout << "Running tests...\n";
            std::cout << "===============================================================================\n";
            for (auto& test : tests) {
                std::cout << "\nTest case: " << test.name << "\n";
                try {
                    currentSections.clear();
                    test.func();
                    std::cout << "  ✓ PASSED\n";
                } catch (const std::exception& e) {
                    std::cout << "  ✗ FAILED: " << e.what() << "\n";
                    failed++;
                }
            }
            std::cout << "\n===============================================================================\n";
            std::cout << "Summary: " << tests.size() << " test cases, " << (tests.size() - failed) 
                      << " passed, " << failed << " failed\n";
            std::cout << "===============================================================================\n";
            return failed;
        }
        
        bool enterSection(const std::string& name) {
            // Simple section handling - just track what section we're in
            for (auto& sec : currentSections) {
                if (sec.name == name) {
                    if (sec.runCount == 0) {
                        sec.runCount++;
                        return true;
                    }
                    return false;
                }
            }
            currentSections.push_back(Section(name));
            std::cout << "  Section: " << name << "\n";
            currentSections.back().runCount++;
            return true;
        }
        
    private:
        std::vector<TestCase> tests;
        std::vector<Section> currentSections;
    };
    
    class AssertionException : public std::exception {
        std::string msg;
    public:
        AssertionException(const std::string& m) : msg(m) {}
        const char* what() const noexcept override { return msg.c_str(); }
    };
}

#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) name##line
#define INTERNAL_CATCH_UNIQUE_NAME_LINE( name, line ) INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line )
#define INTERNAL_CATCH_UNIQUE_NAME( name ) INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __LINE__ )

#define TEST_CASE(name, tags) \
    static void INTERNAL_CATCH_UNIQUE_NAME(test_function)(); \
    namespace { \
        struct INTERNAL_CATCH_UNIQUE_NAME(TestRegistrar) { \
            INTERNAL_CATCH_UNIQUE_NAME(TestRegistrar)() { \
                Catch::TestRegistry::instance().registerTest(name, tags, INTERNAL_CATCH_UNIQUE_NAME(test_function)); \
            } \
        } INTERNAL_CATCH_UNIQUE_NAME(registrar); \
    } \
    static void INTERNAL_CATCH_UNIQUE_NAME(test_function)()

#define SECTION(name) \
    if (Catch::TestRegistry::instance().enterSection(name))

#define REQUIRE(expr) \
    do { \
        if (!(expr)) { \
            std::ostringstream oss; \
            oss << "REQUIRE failed: " << #expr << " at " << __FILE__ << ":" << __LINE__; \
            throw Catch::AssertionException(oss.str()); \
        } \
    } while(0)

#ifdef CATCH_CONFIG_MAIN
int main(int argc, char* argv[]) {
    return Catch::TestRegistry::instance().runAllTests();
}
#endif

#endif // TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED
