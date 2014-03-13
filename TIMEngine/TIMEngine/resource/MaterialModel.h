#ifndef MATERIALMODEL_H_INCLUDED
#define MATERIALMODEL_H_INCLUDED

#include "core/core.h"
#include "renderer/Material.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

    struct MaterialModel
    {
        std::string shader;
        std::string mesh;
        boost::container::vector<std::string> textures;

        vec4 color=vec4(1);
        vec4 material={1,1,1,0}; // ambient, diffuse, specular, emisive
        float exponent=50;

        bool depthTest=true, writeDepthBuffer=true;
        renderer::Material::CompareFunc depthFunc=renderer::Material::LESS;
        Vector4<bool> writeColor = Vector4<bool>(true,true,true,true);

        bool alphaTest=false;
        float alphaThreshold=0.1;
        renderer::Material::CompareFunc alphaFunc=renderer::Material::GREATER;

        bool cullFace=true, cullBackFace=true;

        bool blend=false;
        renderer::Material::BlendEquation blendEquation=renderer::Material::ADD;
        Vector2<renderer::Material::BlendFunc> blendFunc={renderer::Material::SRC_ALPHA, renderer::Material::ONE_MINUS_SRC_ALPHA};

        bool alwaysFirst=false;
        bool alwaysLast=false;

        bool optional=false;
        bool scissor=false;
        uivec2 scissorCoord, scissorSize={512,512};

        bool logicColor=false;
        GLenum opcode=GL_XOR;

        static renderer::Material::CompareFunc comparFuncToString(const std::string&);
        static renderer::Material::BlendFunc blendFuncToString(const std::string&);
        static renderer::Material::BlendEquation blendEquationToString(const std::string&);
    };

    struct MaterialPassModel
    {
        boost::container::vector<std::string> pass;
    };

}
}
#include "MemoryLoggerOff.h"

#endif // MATERIALMODEL_H_INCLUDED
