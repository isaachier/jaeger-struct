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

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <jaeger-struct/compiler/Field.h>

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

template <typename Message, typename Functor>
bool forEachField(const Message& message, Context& context, Functor f)
{
    for (auto i = 0, len = message.field_count(); i < len; i++) {
        if (!f(*message.field(i), context)) {
            return false;
        }
    }
    return true;
}

template <typename Functor>
bool forEachOneOf(const google::protobuf::Descriptor& message,
                  Context& context,
                  Functor f)
{
    for (auto i = 0, len = message.oneof_decl_count(); i < len; i++) {
        if (!f(*message.oneof_decl(i), context)) {
            return false;
        }
    }
    return true;
}

template <typename Functor>
bool forEachMessage(const google::protobuf::FileDescriptor& file,
                    Context& context,
                    Functor f)
{
    for (auto i = 0, len = file.message_type_count(); i < len; i++) {
        if (!f(*file.message_type(i), context)) {
            return false;
        }
    }
    return true;
}

bool generateField(const google::protobuf::FieldDescriptor& fieldDescriptor,
                   Context& context)
{
    Field field(fieldDescriptor);
    return field.writeDeclaration(*context._printer, context._error);
}

bool generateOneOf(const google::protobuf::OneofDescriptor& oneOfDescriptor,
                   Context& context)
{
    context._printer->Print("union {\n");
    context._printer->Indent();
    context._printer->Print("uint8_t tag;\n");
    const auto result = forEachField(oneOfDescriptor, context, generateField);
    context._printer->Outdent();
    context._printer->Print("} $name$;\n", "name", oneOfDescriptor.name());
    return true;
}

bool generateStruct(const google::protobuf::Descriptor& message,
                    Context& context)
{
    context._printer->Print(
        "\ntypedef struct $name$ {\n", "name", message.name());
    context._printer->Indent();
    const auto result = forEachField(message, context, generateField);
    forEachOneOf(message, context, generateOneOf);
    context._printer->Outdent();
    context._printer->Print("} $name$;\n", "name", message.name());
    return result;
}

void writeProlog(google::protobuf::io::Printer& printer,
                 const std::string& guard)
{
    printer.Print("#ifndef $guard$\n", "guard", guard);
    printer.Print("#define $guard$\n\n", "guard", guard);
    printer.Print("#include <jaeger-struct/runtime/list.h>\n");
    printer.Print("#include <jaeger-struct/runtime/optional.h>\n");
    printer.Print("#include <jaeger-struct/runtime/string.h>\n");
}

void writeEpilog(google::protobuf::io::Printer& printer,
                 const std::string& guard)
{
    printer.Print("#endif /* $guard$ */\n", "guard", guard);
}

std::string includeGuard(const std::string& fileName)
{
    std::string result;
    std::transform(std::begin(fileName),
                   std::end(fileName),
                   std::back_inserter(result),
                   [](char ch) {
                       if (std::islower(ch)) {
                           return static_cast<char>(std::toupper(ch));
                       }
                       if (!std::isupper(ch)) {
                           return '_';
                       }
                       return ch;
                   });
    return result;
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
    const auto guard = includeGuard(fileName);
    writeProlog(*context._printer, guard);
    if (!forEachMessage(*file, context, generateStruct)) {
        return false;
    }
    writeEpilog(*context._printer, guard);
    return true;
}

}  // namespace compiler
}  // namespace jaeger_struct

int main(int argc, char* argv[])
{
    jaeger_struct::compiler::Generator gen;
    return google::protobuf::compiler::PluginMain(argc, argv, &gen);
}
