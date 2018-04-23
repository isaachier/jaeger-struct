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

#include <jaeger-struct/compiler/Enum.h>

#include <google/protobuf/descriptor.h>

#include <jaeger-struct/compiler/Strings.h>

namespace jaeger_struct {
namespace compiler {
namespace {

std::set<Enum::Value>
determineValues(const google::protobuf::EnumDescriptor& descriptor)
{
    std::set<Enum::Value> result;
    for (auto i = 0, len = descriptor.value_count(); i < len; ++i) {
        auto&& value = *descriptor.value(i);
        result.emplace(
            Enum::Value(snakeCase(descriptor.full_name() + '_' + value.name()),
                        value.number()));
    }
    return result;
}

}  // anonymous namespace

Enum::Enum(const google::protobuf::EnumDescriptor& descriptor)
    : _name(snakeCase(descriptor.full_name()))
    , _values(determineValues(descriptor))
{
}

}  // namespace compiler
}  // namespace jaeger_struct
