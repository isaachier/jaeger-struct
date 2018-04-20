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

#include <functional>
#include <iostream>
#include <memory>
#include <unordered_set>

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <jaeger-struct/compiler/Field.h>

namespace jaeger_struct {
namespace compiler {
namespace {

enum class StringCase { kCapsCase, kSnakeCase };

std::string stringCasing(const std::string str, StringCase casing)
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
    std::transform(std::begin(str),
                   std::end(str),
                   std::back_inserter(result),
                   [predicate, transform](char ch) {
                       if (predicate(ch)) {
                           return transform(ch);
                       }
                       return ch;
                   });
    return result;
}

std::string makeIdentifier(const std::string& str)
{
    enum class State { kDefault, kSeparator };

    std::string result;
    State state = State::kDefault;
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

std::string capsCase(const std::string& str)
{
    return stringCasing(makeIdentifier(str), StringCase::kCapsCase);
}

std::string snakeCase(const std::string& str)
{
    return stringCasing(makeIdentifier(str), StringCase::kSnakeCase);
}

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

bool generateField(const google::protobuf::FieldDescriptor& fieldDescriptor,
                   Context& context)
{
    Field field(fieldDescriptor);
    return field.writeDeclaration(*context._printer, context._error);
}

bool generateUnion(const google::protobuf::OneofDescriptor& oneOfDescriptor,
                   Context& context)
{
    context._printer->Print("union {\n");
    context._printer->Indent();
    context._printer->Print("uint8_t tag;\n");
    for (auto i = 0, len = oneOfDescriptor.field_count(); i < len; i++) {
        if (!generateField(*oneOfDescriptor.field(i), context)) {
            return false;
        }
    }
    context._printer->Outdent();
    context._printer->Print("} $name$;\n", "name", oneOfDescriptor.name());
    return true;
}

bool generateEnum(const google::protobuf::EnumDescriptor& enumDescriptor,
                  Context& context)
{
    const auto type = makeIdentifier(enumDescriptor.full_name());
    context._printer->Print("enum $type$ {\n", "type", type);
    context._printer->Indent();
    for (auto i = 0, len = enumDescriptor.value_count(); i < len; i++) {
        auto&& value = *enumDescriptor.value(i);
        context._printer->Print(
            "$name$ = $value$$comma$\n",
            "name",
            capsCase(enumDescriptor.full_name() + '_' + value.name()),
            "value",
            std::to_string(value.number()),
            "comma",
            i == (len - 1) ? "" : ",");
    }
    context._printer->Outdent();
    context._printer->Print("};\n");
    return true;
}

bool generateStruct(const google::protobuf::Descriptor& message,
                    Context& context)
{
    context._printer->Print("\ntypedef struct $name$ {\n",
                            "name",
                            makeIdentifier(message.full_name()));
    context._printer->Indent();

    std::unordered_set<const google::protobuf::FieldDescriptor*> unionFields;
    for (auto i = 0, len = message.oneof_decl_count(); i < len; i++) {
        auto&& oneOf = *message.oneof_decl(i);
        for (auto j = 0, oneOfLen = oneOf.field_count(); j < oneOfLen; j++) {
            unionFields.insert(oneOf.field(j));
        }
    }

    std::unordered_set<const google::protobuf::FieldDescriptor*> fields;
    for (auto i = 0, len = message.field_count(); i < len; i++) {
        auto&& field = *message.field(i);
        fields.insert(&field);
    }

    for (auto&& unionField : unionFields) {
        auto itr = fields.find(unionField);
        assert(itr != std::end(fields));
        fields.erase(itr);
    }

    std::vector<const google::protobuf::FieldDescriptor*> sortedFields(
        fields.cbegin(), fields.cend());
    std::sort(std::begin(sortedFields),
              std::end(sortedFields),
              [](const google::protobuf::FieldDescriptor* lhs,
                 const google::protobuf::FieldDescriptor* rhs) {
                  return lhs->number() < rhs->number();
              });
    for (auto&& field : fields) {
        if (!generateField(*field, context)) {
            return false;
        }
    }

    for (auto i = 0, len = message.oneof_decl_count(); i < len; i++) {
        auto&& oneOf = *message.oneof_decl(i);
        if (!generateUnion(oneOf, context)) {
            return false;
        }
    }

    context._printer->Outdent();
    context._printer->Print("} $name$;\n", "name", message.name());
    return true;
}

bool generateForwardDeclarations(
    const google::protobuf::FileDescriptor& descriptor, Context& context)
{
    for (auto i = 0, len = descriptor.message_type_count(); i < len; i++) {
        auto&& message = *descriptor.message_type(i);
        const auto name = makeIdentifier(message.full_name());
        context._printer->Print("struct $name$;\n", "name", name);

        for (auto j = 0, enumLen = message.enum_type_count(); j < enumLen;
             j++) {
            if (!generateEnum(*message.enum_type(j), context)) {
                return false;
            }
        }
    }
    return true;
}

bool generateDefinitions(const google::protobuf::FileDescriptor& descriptor,
                         Context& context)
{
    for (auto i = 0, len = descriptor.message_type_count(); i < len; i++) {
        if (!generateStruct(*descriptor.message_type(i), context)) {
            return false;
        }
    }
    return true;
}

void writeProlog(google::protobuf::io::Printer& printer,
                 const std::string& guard)
{
    printer.Print("#ifndef $guard$\n", "guard", guard);
    printer.Print("#define $guard$\n\n", "guard", guard);
    printer.Print("#include <jaeger-struct/runtime/list.h>\n");
    printer.Print("#include <jaeger-struct/runtime/optional.h>\n");
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
    printer.Print("#endif /* $guard$ */\n", "guard", guard);
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
    if (!generateForwardDeclarations(*file, context)) {
        return false;
    }
    if (!generateDefinitions(*file, context)) {
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
