
#include "MaterialModel.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

renderer::Material::CompareFunc MaterialModel::comparFuncToString(const std::string& str)
{
    if(str == "NEVER")
        return renderer::Material::CompareFunc::NEVER;
    else if(str == "NOTEQUAL")
        return renderer::Material::CompareFunc::NOTEQUAL;
    else if(str == "LESS")
        return renderer::Material::CompareFunc::LESS;
    else if(str == "LEQUAL")
        return renderer::Material::CompareFunc::LEQUAL;
    else if(str == "GREATER")
        return renderer::Material::CompareFunc::GREATER;
    else if(str == "GEQUAL")
        return renderer::Material::CompareFunc::GEQUAL;
    else if(str == "EQUAL")
        return renderer::Material::CompareFunc::EQUAL;
    else if(str == "ALWAYS")
        return renderer::Material::CompareFunc::ALWAYS;
    else return renderer::Material::CompareFunc::GREATER;
}

renderer::Material::BlendFunc MaterialModel::blendFuncToString(const std::string& str)
{
    if(str == "ZERO")
        return renderer::Material::BlendFunc::ZERO;
    else if(str == "ONE")
        return renderer::Material::BlendFunc::ONE;
    else if(str == "SRC_COLOR")
        return renderer::Material::BlendFunc::SRC_COLOR;
    else if(str == "ONE_MINUS_SRC_COLOR")
        return renderer::Material::BlendFunc::ONE_MINUS_SRC_COLOR;
    else if(str == "DST_COLOR")
        return renderer::Material::BlendFunc::DST_COLOR;
    else if(str == "ONE_MINUS_DST_COLOR")
        return renderer::Material::BlendFunc::ONE_MINUS_DST_COLOR;
    else if(str == "SRC_ALPHA")
        return renderer::Material::BlendFunc::SRC_ALPHA;
    else if(str == "ONE_MINUS_SRC_ALPHA")
        return renderer::Material::BlendFunc::ONE_MINUS_SRC_ALPHA;
    else if(str == "DST_ALPHA")
        return renderer::Material::BlendFunc::DST_ALPHA;
    else if(str == "ONE_MINUS_DST_ALPHA")
        return renderer::Material::BlendFunc::ONE_MINUS_DST_ALPHA;
    else if(str == "CONSTANT_COLOR")
        return renderer::Material::BlendFunc::CONSTANT_COLOR;
    else if(str == "ONE_MINUS_CONSTANT_COLOR")
        return renderer::Material::BlendFunc::ONE_MINUS_CONSTANT_COLOR;
    else if(str == "CONSTANT_ALPHA")
        return renderer::Material::BlendFunc::CONSTANT_ALPHA;
    else if(str == "ONE_MINUS_CONSTANT_ALPHA")
        return renderer::Material::BlendFunc::ONE_MINUS_CONSTANT_ALPHA;
    else return renderer::Material::BlendFunc::ONE;
}

renderer::Material::BlendEquation MaterialModel::blendEquationToString(const std::string& str)
{
    if(str == "ADD")
        return renderer::Material::BlendEquation::ADD;
    else if(str == "SUB")
        return renderer::Material::BlendEquation::SUB;
    else if(str == "REVERSE_SUB")
        return renderer::Material::BlendEquation::REVERSE_SUB;
    else if(str == "MIN")
        return renderer::Material::BlendEquation::MIN;
    else if(str == "MAX")
        return renderer::Material::BlendEquation::MAX;
    else return renderer::Material::BlendEquation::ADD;
}

}
}
