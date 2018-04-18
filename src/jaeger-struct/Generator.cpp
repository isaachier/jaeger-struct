#include <jaeger-struct/Generator.h>

#include <iostream>
#include <memory>

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

namespace jaegertracing {
namespace structgen {
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

template <typename Functor>
bool forEachField(const google::protobuf::Descriptor& message,
                  Context& context,
                  Functor f)
{
    for (auto i = 0, len = message.field_count(); i < len; i++) {
        if (!f(message.field(i), context)) {
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
    context._outStream.reset(
        context._context.Open(stripProto(file.name()) + ".jaeger.h"));
    context._printer.reset(new google::protobuf::io::Printer(
        context._outStream.get(), '$', nullptr));
    for (auto i = 0, len = file.message_type_count(); i < len; i++) {
        if (!f(*file.message_type(i), context)) {
            return false;
        }
    }
    return true;
}

bool generateField(const google::protobuf::FieldDescriptor* field,
                   Context& context)
{
    std::string typeStr;

#define TYPE_MAPPING(enumName, type)                                           \
    case google::protobuf::FieldDescriptor::CPPTYPE_##enumName:                \
        typeStr = #type;                                                       \
        break

    switch (field->cpp_type()) {
        TYPE_MAPPING(BOOL, bool);
        TYPE_MAPPING(FLOAT, float);
        TYPE_MAPPING(DOUBLE, double);
        TYPE_MAPPING(INT32, int32_t);
        TYPE_MAPPING(INT64, int64_t);
        TYPE_MAPPING(UINT32, uint32_t);
        TYPE_MAPPING(UINT64, uint64_t);
        TYPE_MAPPING(ENUM, int);
        TYPE_MAPPING(STRING, const char*);
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
        typeStr = field->message_type()->name();
        break;
    default:
        context._error = "Invalid type";
        return false;
    }
    context._printer->Print(
        "    $type$ $name$;\n", "type", typeStr, "name", field->name());
    return true;
}

bool generateStruct(const google::protobuf::Descriptor& message,
                    Context& context)
{
    context._printer->Print(
        "\ntypedef struct $name$ {\n", "name", message.name());
    const auto result = forEachField(message, context, generateField);
    context._printer->Print("} $name$;\n", "name", message.name());
    return result;
}

}  // anonymous namespace

bool Generator::Generate(const google::protobuf::FileDescriptor* file,
                         const std::string& parameter,
                         google::protobuf::compiler::GeneratorContext* arg,
                         std::string* error) const
{
    Context context(*arg, *error);
    return forEachMessage(*file, context, generateStruct);
}

}  // namespace structgen
}  // namespace jaegertracing

int main(int argc, char* argv[])
{
    jaegertracing::structgen::Generator gen;
    return google::protobuf::compiler::PluginMain(argc, argv, &gen);
}
