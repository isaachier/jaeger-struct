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

#include <jaeger-struct/compiler/Generator.h>

#include <iostream>
#include <memory>
#include <unordered_set>

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <jaeger-struct/compiler/Enum.h>
#include <jaeger-struct/compiler/Strings.h>
#include <jaeger-struct/compiler/Struct.h>
#include <jaeger-struct/compiler/TypeRegistry.h>
#include <jaeger-struct/compiler/Union.h>

namespace jaeger_struct {
namespace compiler {
namespace {

struct Context {
    Context(google::protobuf::compiler::GeneratorContext& context,
            std::string& error)
        : _context(context)
        , _error(error)
        , _outStream()
        , _printer()
    {
    }

    void openFile(const std::string& fileName)
    {
        _printer.reset();
        _outStream.reset(_context.Open(fileName));
        _printer.reset(
            new google::protobuf::io::Printer(_outStream.get(), '$', nullptr));
    }

    google::protobuf::compiler::GeneratorContext& _context;
    std::string& _error;
    std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> _outStream;
    std::unique_ptr<google::protobuf::io::Printer> _printer;
};

bool endsWith(const std::string& str, const std::string& suffix)
{
    if (str.size() < suffix.size()) {
        return false;
    }
    return (strcmp(str.c_str() + str.size() - suffix.size(), suffix.c_str()) ==
            0);
}

std::string stripProto(const std::string& str)
{
    const std::string suffix(".proto");
    if (endsWith(str, suffix)) {
        return str.substr(0, str.size() - suffix.size());
    }
    return str;
}

void writeProlog(google::protobuf::io::Printer& printer,
                 const std::string& guard)
{
    printer.Print("#ifndef $guard$_H\n", "guard", guard);
    printer.Print("#define $guard$_H\n\n", "guard", guard);
    printer.Print("#include <jaeger-struct/runtime/list.h>\n");
    printer.Print("#include <jaeger-struct/runtime/string.h>\n\n");
    printer.Print("#ifdef __cplusplus\n");
    printer.Print("extern \"C\" {\n");
    printer.Print("#endif /* __cplusplus */\n\n");
}

void writeEpilog(google::protobuf::io::Printer& printer,
                 const std::string& guard)
{
    printer.Print("\n#ifdef __cplusplus\n");
    printer.Print("}\n");
    printer.Print("#endif /* __cplusplus */\n\n");
    printer.Print("#endif /* $guard$_H */\n", "guard", guard);
}

void generateTypes(const google::protobuf::FileDescriptor& file,
                   google::protobuf::io::Printer& printer,
                   TypeRegistry& registry)
{
    for (auto i = 0, len = file.enum_type_count(); i < len; ++i) {
        auto&& enumDescriptor = *file.enum_type(i);
        auto e = std::make_shared<const Enum>(enumDescriptor);
        printer.Print("\n");
        e->writeDefinition(printer);
        registry.registerType(std::static_pointer_cast<const Type>(e));
    }

    for (auto i = 0, len = file.message_type_count(); i < len; ++i) {
        auto&& message = *file.message_type(i);

        for (auto j = 0, len = message.enum_type_count(); j < len; ++j) {
            auto&& enumDescriptor = *message.enum_type(j);
            auto e = std::make_shared<const Enum>(enumDescriptor);
            printer.Print("\n");
            e->writeDefinition(printer);
            registry.registerType(std::static_pointer_cast<const Type>(e));
        }

        for (auto j = 0, oneOfLen = message.oneof_decl_count(); j < oneOfLen;
             ++j) {
            auto&& oneOf = *message.oneof_decl(j);
            auto u = std::make_shared<const Union>(oneOf, registry);
            printer.Print("\n");
            u->writeDefinition(printer);
            registry.registerType(std::static_pointer_cast<const Type>(u));
        }

        auto s = std::make_shared<const Struct>(message, registry);
        printer.Print("\n");
        s->writeDefinition(printer);
        registry.registerType(std::static_pointer_cast<const Type>(s));
    }
}

}  // anonymous namespace

bool Generator::Generate(const google::protobuf::FileDescriptor* file,
                         const std::string& parameter,
                         google::protobuf::compiler::GeneratorContext* arg,
                         std::string* error) const
{
    Context context(*arg, *error);
    const auto fileName = stripProto(file->name()) + ".h";
    context.openFile(fileName);
    const auto guard = capsCase(fileName);
    writeProlog(*context._printer, guard);
    TypeRegistry registry;
    generateTypes(*file, *context._printer, registry);
    writeEpilog(*context._printer, guard);
    return true;
}

}  // namespace compiler
}  // namespace jaeger_struct
