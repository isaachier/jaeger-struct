#include <jaeger-struct/CGenerator.h>

#include <cassert>
#include <iostream>

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>

namespace jaegertracing {
namespace structgen {

bool CGenerator::Generate(const google::protobuf::FileDescriptor* file,
                          const std::string& parameter,
                          google::protobuf::compiler::GeneratorContext* context,
                          std::string* error) const
{
    assert(file != NULL);
    std::cerr << "filename = " << file->name();
    if (!parameter.empty()) {
        std::cerr << ", parameter = " << parameter;
    }
    std::cerr << '\n';
    for (auto i = 0, len = file->message_type_count(); i < len; i++) {
        auto* message = file->message_type(i);
        std::cerr << "message name = " << message->name() << '\n';
    }
    return true;
}

}  // namespace structgen
}  // namespace jaegertracing

int main(int argc, char* argv[])
{
    jaegertracing::structgen::CGenerator gen;
    return google::protobuf::compiler::PluginMain(argc, argv, &gen);
}
