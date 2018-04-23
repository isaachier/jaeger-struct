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

#include <jaeger-struct/compiler/Type.h>

namespace jaeger_struct {
namespace compiler {

class TypeRegistry {
  public:
    TypeRegistry();

    void registerType(std::shared_ptr<Type>&& type)
    {
        _typeRegistry.emplace(type->name(), type);
    }

    std::shared_ptr<const Type> findType(const std::string& name) const
    {
        const auto itr = _typeRegistry.find(name);
        if (itr == std::end(_typeRegistry)) {
            return std::shared_ptr<Type>();
        }
        return std::static_pointer_cast<const Type>(itr->second);
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<Type>> _typeRegistry;
};

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_TYPE_REGISTRY_H
