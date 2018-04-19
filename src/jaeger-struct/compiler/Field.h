#ifndef JAEGER_STRUCT_COMPILER_FIELD_H
#define JAEGER_STRUCT_COMPILER_FIELD_H

#include <string>

namespace google {
namespace protobuf {
namespace io {

class Printer;

}  // namespace io

class FieldDescriptor;

}  // namespace protobuf
}  // namespace google

namespace jaeger_struct {
namespace compiler {

class Field {
  public:
    explicit Field(const google::protobuf::FieldDescriptor& descriptor);

    ~Field();

    bool writeDeclaration(google::protobuf::io::Printer& printer,
                          std::string& error) const;

    bool writeInitializer(google::protobuf::io::Printer& printer,
                          std::string& error) const;

  private:
    const google::protobuf::FieldDescriptor& _descriptor;
};

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_FIELD_H
