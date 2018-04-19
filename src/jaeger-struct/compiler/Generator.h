#ifndef JAEGER_STRUCT_COMPILER_GENERATOR
#define JAEGER_STRUCT_COMPILER_GENERATOR

#include <google/protobuf/compiler/code_generator.h>

namespace jaeger_struct {
namespace compiler {

class Generator : public google::protobuf::compiler::CodeGenerator {
  public:
    bool Generate(const google::protobuf::FileDescriptor* file,
                  const std::string& parameter,
                  google::protobuf::compiler::GeneratorContext* context,
                  std::string* error) const override;
};

}  // namespace compiler
}  // namespace jaeger_struct

#endif  // JAEGER_STRUCT_COMPILER_GENERATOR
