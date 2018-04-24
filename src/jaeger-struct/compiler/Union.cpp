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

#include <jaeger-struct/compiler/Union.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include <jaeger-struct/compiler/Strings.h>
#include <jaeger-struct/compiler/TypeRegistry.h>

namespace jaeger_struct {
namespace compiler {
namespace {

std::vector<Field>
determineFields(const google::protobuf::OneofDescriptor& descriptor,
                const TypeRegistry& registry)
{
    std::vector<const google::protobuf::FieldDescriptor*> fieldDescriptors;
    fieldDescriptors.reserve(descriptor.field_count());
    for (auto i = 0, len = descriptor.field_count(); i < len; ++i) {
        fieldDescriptors.emplace_back(descriptor.field(i));
    }
    std::sort(std::begin(fieldDescriptors),
              std::end(fieldDescriptors),
              [](const google::protobuf::FieldDescriptor* lhs,
                 const google::protobuf::FieldDescriptor* rhs) {
                  return lhs->number() < rhs->number();
              });
    std::vector<Field> fields;
    fields.reserve(fieldDescriptors.size());
    std::transform(
        std::begin(fieldDescriptors),
        std::end(fieldDescriptors),
        std::back_inserter(fields),
        [&registry](const google::protobuf::FieldDescriptor* fieldDescriptor) {
            return Field(*fieldDescriptor, registry);
        });
    return fields;
}

}  // anonymous namespace

Union::Union(const google::protobuf::OneofDescriptor& descriptor,
             const TypeRegistry& registry)
    : ComplexType(snakeCase(descriptor.full_name()),
                  determineFields(descriptor, registry))
{
}

void Union::writeDefinition(google::protobuf::io::Printer& printer) const
{
    printer.Print("enum {");
    printer.Indent();
    const auto fieldsVec = fields();
    std::for_each(std::begin(fieldsVec),
                  std::end(fieldsVec),
                  [this, &fieldsVec, &printer](const Field& field) {
                      if (&field != &*std::begin(fieldsVec)) {
                          printer.Print(",");
                      }
                      printer.Print(
                          "\n$name$_type", "name", name() + "_" + field.name());
                  });
    printer.Outdent();
    printer.Print("\n};\n");

    printer.Print("typedef struct $name$ {\n", "name", name());
    printer.Indent();
    printer.Print("uint8_t type;\n");
    printer.Print("union ");
    writeBracedDefinition(printer);
    printer.Print(" value;\n");
    printer.Outdent();
    printer.Print("} $name$;", "name", name());
}

}  // namespace compiler
}  // namespace jaeger_struct
