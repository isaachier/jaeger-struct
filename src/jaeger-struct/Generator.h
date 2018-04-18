#ifndef JAEGER_STRUCT_C_GENERATOR
#define JAEGER_STRUCT_C_GENERATOR

#include <google/protobuf/compiler/code_generator.h>

namespace jaegertracing {
namespace structgen {

class Generator : public google::protobuf::compiler::CodeGenerator {
  public:
    bool Generate(const google::protobuf::FileDescriptor* file,
                  const std::string& parameter,
                  google::protobuf::compiler::GeneratorContext* context,
                  std::string* error) const override;
};

}  // namespace structgen
}  // namespace jaegertracing

#endif  // JAEGER_STRUCT_C_GENERATOR
