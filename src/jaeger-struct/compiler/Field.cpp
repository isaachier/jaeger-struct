#include <jaeger-struct/compiler/Field.h>

#include <sstream>
#include <stdexcept>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

namespace jaeger_struct {
namespace compiler {
namespace {

std::string underlyingType(const google::protobuf::FieldDescriptor& field)
{
    std::string typeStr;

#define TYPE_MAPPING(enumName, type)                                           \
    case google::protobuf::FieldDescriptor::CPPTYPE_##enumName:                \
        typeStr = #type;                                                       \
        break

    switch (field.cpp_type()) {
        TYPE_MAPPING(BOOL, bool);
        TYPE_MAPPING(FLOAT, float);
        TYPE_MAPPING(DOUBLE, double);
        TYPE_MAPPING(INT32, int32_t);
        TYPE_MAPPING(INT64, int64_t);
        TYPE_MAPPING(UINT32, uint32_t);
        TYPE_MAPPING(UINT64, uint64_t);
        TYPE_MAPPING(STRING, jaeger_string);
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
        typeStr = field.enum_type()->name();
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
        typeStr = field.message_type()->name();
        break;
    default:
        break;
    }

    return typeStr;

#undef TYPE_MAPPING
}

std::string determineType(const google::protobuf::FieldDescriptor& field)
{
    switch (field.label()) {
    case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
        return "JAEGER_OPTIONAL(" + underlyingType(field) + ")";
    case google::protobuf::FieldDescriptor::LABEL_REPEATED:
        return "JAEGER_LIST(" + underlyingType(field) + ")";
    case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
        return underlyingType(field);
    default:
        return "";
    }
}

}  // anonymous namespace

Field::Field(const google::protobuf::FieldDescriptor& descriptor)
    : _descriptor(descriptor)
{
}

Field::~Field() = default;

#define DETERMINE_TYPE_OR_FAIL(var)                                            \
    const auto var = determineType(_descriptor);                               \
    {                                                                          \
        if (type.empty()) {                                                    \
            std::ostringstream oss;                                            \
            oss << "Unknown type " << _descriptor.cpp_type_name();             \
            error = oss.str();                                                 \
            return false;                                                      \
        }                                                                      \
    }

bool Field::writeDeclaration(google::protobuf::io::Printer& printer,
                             std::string& error) const
{
    DETERMINE_TYPE_OR_FAIL(type);
    printer.Print("$type$ $name$;\n", "type", type, "name", _descriptor.name());
    return true;
}

bool Field::writeInitializer(google::protobuf::io::Printer& printer,
                             std::string& error) const
{
    DETERMINE_TYPE_OR_FAIL(type);
    if (type == "jaeger_list") {
        printer.Print("jaeger_list_init(&$name$, sizeof($underlying$))",
                      "name",
                      _descriptor.name(),
                      "underlying",
                      underlyingType(_descriptor));
    }
    return true;
}

}  // namespace compiler
}  // namespace jaeger_struct