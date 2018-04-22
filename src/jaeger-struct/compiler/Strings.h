/*
 * Copyright (c) 2018 Uber Technologies, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JAEGER_STRUCT_COMPILER_STRINGS
#define JAEGER_STRUCT_COMPILER_STRINGS

#include <cassert>
#include <cctype>
#include <functional>
#include <iostream>
#include <string>

namespace jaeger_struct {
namespace compiler {

enum class StringCase { kCapsCase, kSnakeCase };

inline std::string stringCasing(const std::string str, StringCase casing)
{
    std::string result;
    std::function<bool(char)> predicate;
    std::function<char(char)> transform;
    switch (casing) {
    case StringCase::kCapsCase:
        predicate = [](char ch) { return std::islower(ch); };
        transform = [](char ch) { return std::toupper(ch); };
        break;
    default:
        assert(casing == StringCase::kSnakeCase);
        predicate = [](char ch) { return std::isupper(ch); };
        transform = [](char ch) { return std::tolower(ch); };
        break;
    }

    for (auto ch : str) {
        if (predicate(ch)) {
            result += ch;
        }
        else {
            result += transform(ch);
        }
    }
    return result;
}

inline std::string makeIdentifier(const std::string& str)
{
    enum class State { kDefault, kSeparator };

    std::string result;
    auto state = State::kDefault;
    for (auto&& ch : str) {
        switch (state) {
        case State::kDefault:
            if (std::isalnum(ch)) {
                result += ch;
            }
            else {
                state = State::kSeparator;
            }
            break;
        default:
            assert(state == State::kSeparator);
            if (std::isalnum(ch)) {
                result += '_';
                result += ch;
                state = State::kDefault;
            }
            break;
        }
    }
    return result;
}

inline std::string capsCase(const std::string& str)
{
    return stringCasing(makeIdentifier(str), StringCase::kCapsCase);
}

inline std::string snakeCase(const std::string& str)
{
    return stringCasing(makeIdentifier(str), StringCase::kSnakeCase);
}

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_STRINGS
