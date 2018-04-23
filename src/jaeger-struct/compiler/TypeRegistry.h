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

#ifndef JAEGER_STRUCT_COMPILER_TYPE_REGISTRY_H
#define JAEGER_STRUCT_COMPILER_TYPE_REGISTRY_H

#include <iostream>
#include <memory>
#include <unordered_map>

#include <google/protobuf/descriptor.h>

#include <jaeger-struct/compiler/Type.h>

namespace jaeger_struct {
namespace compiler {

class TypeRegistry {
  public:
    TypeRegistry();

    void registerType(std::shared_ptr<const Type>&& type)
    {
        _registry.emplace(type->name(), type);
    }

    std::shared_ptr<const Type> findType(const std::string& name) const
    {
        const auto itr = _registry.find(name);
        if (itr == std::end(_registry)) {
            return std::shared_ptr<const Type>();
        }
        return itr->second;
    }

    std::shared_ptr<const Type>
    findType(google::protobuf::FieldDescriptor::CppType type) const;

  private:
    std::unordered_map<std::string, std::shared_ptr<const Type>> _registry;
};

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_TYPE_REGISTRY_H
