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

#ifndef JAEGER_STRUCT_COMPILER_STRINGS_H
#define JAEGER_STRUCT_COMPILER_STRINGS_H

#include <cctype>
#include <regex>
#include <string>

namespace jaeger_struct {
namespace compiler {

inline std::string makeIdentifier(const std::string& str)
{
    const std::regex nonAlnumRegex("[^A-Za-z0-9_]+");
    return std::regex_replace(str, nonAlnumRegex, "_");
}

inline std::string capsCase(const std::string& str)
{
    const std::regex groupRegex("[A-Za-z][A-Za-z0-9]+");
    std::string result;
    result.reserve(str.size());
    std::sregex_iterator begin(std::begin(str), std::end(str), groupRegex);
    std::sregex_iterator end;
    for (auto itr = begin; itr != end; ++itr) {
        auto&& match = *itr;
        result += '_';
        for (auto&& ch : match.str()) {
            if (std::islower(ch)) {
                result += std::toupper(ch);
            }
            else {
                result += ch;
            }
        }
    }
    result.shrink_to_fit();
    return result;
}

inline std::string snakeCase(const std::string& str)
{
    auto&& input = capsCase(str);
    std::string result;
    result.reserve(input.size());
    std::transform(std::begin(input),
                   std::end(input),
                   std::back_inserter(result),
                   [](char ch) {
                       if (std::isupper(ch)) {
                           return static_cast<char>(std::tolower(ch));
                       }
                       return ch;
                   });
    return result;
}

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_STRINGS_H
