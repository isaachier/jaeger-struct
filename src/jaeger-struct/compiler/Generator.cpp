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

void writeMacros(Context& context)
{
    context.openFile("macros.h");
    context._printer->Print("#ifndef MACROS_H\n");
    context._printer->Print("#define MACROS_H\n");
    context._printer->Print("#endif /* MACROS_H */\n");
}

void writeProlog(google::protobuf::io::Printer& printer,
                 const std::string& includeGuard)
{
    printer.Print("#ifndef $guard$\n", "guard", includeGuard);
    printer.Print("#define $guard$\n", "guard", includeGuard);
    printer.Print("#include \"macros.h\"\n");
}

std::string includeGuard(const std::string& fileName)
{
    std::string result;
    std::transform(std::begin(fileName),
                   std::end(fileName),
                   std::back_inserter(result),
                   [](int ch) {
                       if (std::islower(ch)) {
                           return static_cast<char>(std::toupper(ch));
                       }
                       if (!std::isupper(ch)) {
                           return '_';
                       }
                       return static_cast<char>(ch);
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
    writeMacros(context);
    const auto fileName = stripProto(file->name()) + ".h";
    context.openFile(fileName);
    writeProlog(*context._printer, includeGuard(fileName));
    return forEachMessage(*file, context, generateStruct);
}

}  // namespace compiler
}  // namespace jaeger_struct

int main(int argc, char* argv[])
{
    jaeger_struct::compiler::Generator gen;
    return google::protobuf::compiler::PluginMain(argc, argv, &gen);
}
