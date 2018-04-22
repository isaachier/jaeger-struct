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

#include <jaeger-struct/compiler/Strings.h>

#include <gtest/gtest.h>

namespace jaeger_struct {
namespace compiler {

TEST(Strings, testCapsCase)
{
    const char* testCases[] = { "capsCase", "CapsCase", "traceID", "TraceID" };
    const char* expected[] = {
        "CAPS_CASE", "CAPS_CASE", "TRACE_ID", "TRACE_ID"
    };
    for (auto i = 0,
              len = static_cast<int>(sizeof(testCases) / sizeof(testCases[0]));
         i < len;
         i++) {
        ASSERT_EQ(expected[i], capsCase(testCases[i]));
    }
}

}  // namespace compiler
}  // namespace jaeger_struct
