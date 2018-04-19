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

    void writeDeclaration(google::protobuf::io::Printer& printer) const;

    void writeInitializer(google::protobuf::io::Printer& printer) const;

  private:
    const google::protobuf::FieldDescriptor& _descriptor;
};

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_FIELD_H
