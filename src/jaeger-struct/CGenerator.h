#ifndef JAEGER_STRUCT_C_GENERATOR
#define JAEGER_STRUCT_C_GENERATOR

#include <protobuf/code_generator.h>

namespace jaegertracing {
namespace structgen {

class CGenerator : public google::protobuf::CodeGenerator {
};

}  // namespace structgen
}  // namespace jaegertracing

#endif  // JAEGER_STRUCT_C_GENERATOR
