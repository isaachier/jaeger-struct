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
        TYPE_MAPPING(STRING, const char*);
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
        typeStr = field.enum_type()->name();
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
        typeStr = field.message_type()->name();
        break;
    default:
        std::ostringstream oss;
        oss << "Unknown type " << field.type_name();
        throw std::invalid_argument(oss.str());
    }

    return typeStr;

#undef TYPE_MAPPING
}

std::string determineType(const google::protobuf::FieldDescriptor& field)
{
    switch (field.label()) {
    case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
        return "jaeger_optional";
    case google::protobuf::FieldDescriptor::LABEL_REPEATED:
        return "jaeger_vector";
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

void Field::writeDeclaration(google::protobuf::io::Printer& printer) const
{
    printer.Print("$type$ $name$;\n",
                  "type",
                  determineType(_descriptor),
                  "name",
                  _descriptor.name());
}

void Field::writeInitializer(google::protobuf::io::Printer& printer) const
{
    // TODO
}

}  // namespace compiler
}  // namespace jaeger_struct
