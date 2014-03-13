#include "MaterialRenderer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

MaterialRenderer::MaterialRenderer(const uivec2& res) : AbstractRenderer(res)
{

}

MaterialRenderer::MaterialRenderer(const boost::container::vector<Texture*>& colorBuf, Texture* depthBuf)  : AbstractRenderer(colorBuf, depthBuf)
{

}

MaterialRenderer::~MaterialRenderer()
{

}

void MaterialRenderer::drawPool(size_t nbInstance, Material* mat)
{
    mat->shader()->setUniform(_matrixPool, nbInstance, mat->shader()->engineUniformId(Shader::EngineUniform::MODEL));
    mat->shader()->setUniform(_cameraPos, mat->shader()->engineUniformId(Shader::EngineUniform::CAMERA_WORLD));
    mat->shader()->setUniform(_frameTime, mat->shader()->engineUniformId(Shader::EngineUniform::FRAME_TIME));
    mat->shader()->setUniform(_time, mat->shader()->engineUniformId(Shader::EngineUniform::TIME));

    mat->shader()->setUniform(_view, mat->shader()->engineUniformId(Shader::EngineUniform::VIEW));
    mat->shader()->setUniform(_proj, mat->shader()->engineUniformId(Shader::EngineUniform::PROJ));
    mat->shader()->setUniform(_projView, mat->shader()->engineUniformId(Shader::EngineUniform::PROJVIEW));
    mat->shader()->setUniform(_invView, mat->shader()->engineUniformId(Shader::EngineUniform::INV_VIEW));
    mat->shader()->setUniform(_invViewProj, mat->shader()->engineUniformId(Shader::EngineUniform::INV_PROJVIEW));

    mat->mesh()->indexBuffer()->draw(mat->mesh()->indexBuffer()->size(), mat->mesh()->primitive(), nbInstance);
}

void MaterialRenderer::draw(Material* mat) const
{
    mat->bind();
    mat->shader()->setUniform(_cameraPos, mat->shader()->engineUniformId(Shader::EngineUniform::CAMERA_WORLD));
    mat->shader()->setUniform(_frameTime, mat->shader()->engineUniformId(Shader::EngineUniform::FRAME_TIME));
    mat->shader()->setUniform(_time, mat->shader()->engineUniformId(Shader::EngineUniform::TIME));

    static const mat4 IDENTITY = mat4::IDENTITY();
    mat->shader()->setUniform(IDENTITY, mat->shader()->engineUniformId(Shader::EngineUniform::MODEL));
    mat->shader()->setUniform(_view, mat->shader()->engineUniformId(Shader::EngineUniform::VIEW));
    mat->shader()->setUniform(_proj, mat->shader()->engineUniformId(Shader::EngineUniform::PROJ));
    mat->shader()->setUniform(_projView, mat->shader()->engineUniformId(Shader::EngineUniform::PROJVIEW));
    mat->shader()->setUniform(_invView, mat->shader()->engineUniformId(Shader::EngineUniform::INV_VIEW));
    mat->shader()->setUniform(_projView, mat->shader()->engineUniformId(Shader::EngineUniform::INV_PROJVIEW));

    mat->mesh()->indexBuffer()->draw(mat->mesh()->indexBuffer()->size(), mat->mesh()->primitive(), 1);
}

}
}
