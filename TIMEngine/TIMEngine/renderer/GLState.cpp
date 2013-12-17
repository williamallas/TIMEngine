#include "GLState.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    GLState& openGL = GLState::instance();

    GLState::GLState()
    {
        resetStates();
    }

    void GLState::getHardwardProperties()
    {
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_hardwardProperties[TEXTURE2D_SIZE]);
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &_hardwardProperties[TEXTURE3D_SIZE]);
        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &_hardwardProperties[CUBEMAP_SIZE]);

        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_hardwardProperties[FRAGMENT_TEX_UNITS]);
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &_hardwardProperties[VERTEX_TEX_UNITS]);
        glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &_hardwardProperties[GEOMETRY_TEX_UNITS]);
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_hardwardProperties[COMBINED_TEX_UNITS]);

        glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &_hardwardProperties[IDEAL_VBO_SIZE]);
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &_hardwardProperties[IDEAL_IBO_SIZE]);

        glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &_hardwardProperties[FRAGMENT_IN]);
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &_hardwardProperties[FRAGMENT_OUT]);
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &_hardwardProperties[FRAGMENT_VEC_UNIFORM]);

        glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &_hardwardProperties[VERTEX_OUT]);
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_hardwardProperties[VERTEX_ATTRIBS]);
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &_hardwardProperties[VERTEX_VEC_UNIFORM]);

        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &_hardwardProperties[GEOMETRY_OUT]);
        glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &_hardwardProperties[GEOMETRY_IN]);
        glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &_hardwardProperties[GEOMETRY_UNIFORM]);
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &_hardwardProperties[GEOMETRY_OUT_VERTICES]);

        glGetIntegerv(GL_MAJOR_VERSION, &_hardwardProperties[MAJOR_VERSION]);
        glGetIntegerv(GL_MINOR_VERSION, &_hardwardProperties[MINOR_VERSION]);


    }

    void GLState::resetStates()
    {
        for(int i=0 ; i<NB_STATES ; i++)
            _glStates[i]=0;
    }

    void GLState::applyAll()
    {
        glBindBuffer(GL_ARRAY_BUFFER, _glStates[ARRAY_BUFFER]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glStates[ELEMENT_ARRAY_BUFFER]);
        glUseProgram(_glStates[SHADER]);
    }

    std::string GLState::strHardward() const
    {
        std::string hardward = "MaxTexture2dSize:"+StringUtils(openGL.hardward(GLState::Hardward::TEXTURE2D_SIZE)).str()+"\n";
        hardward += "MaxTexture3dSize:"+StringUtils(openGL.hardward(GLState::Hardward::TEXTURE3D_SIZE)).str()+"\n";
        hardward += "MaxFragTextureUnits:"+StringUtils(openGL.hardward(GLState::Hardward::FRAGMENT_TEX_UNITS)).str()+"\n";
        hardward += "MaxVertexTextureUnits:"+StringUtils(openGL.hardward(GLState::Hardward::VERTEX_TEX_UNITS)).str()+"\n";
        hardward += "MaxGeometryTextureUnits:"+StringUtils(openGL.hardward(GLState::Hardward::GEOMETRY_TEX_UNITS)).str()+"\n";
        hardward += "MaxCombinedTextureUnits:"+StringUtils(openGL.hardward(GLState::Hardward::COMBINED_TEX_UNITS)).str()+"\n";
        hardward += "IdealVertexArraySize:"+StringUtils(openGL.hardward(GLState::Hardward::IDEAL_VBO_SIZE)).str()+"\n";
        hardward += "IdealElementArraySize:"+StringUtils(openGL.hardward(GLState::Hardward::IDEAL_IBO_SIZE)).str()+"\n";
        hardward += "MaxVertexAttribs:"+StringUtils(openGL.hardward(GLState::Hardward::VERTEX_ATTRIBS)).str()+"\n";
        hardward += "MaxVertexOutput:"+StringUtils(openGL.hardward(GLState::Hardward::VERTEX_OUT)).str()+"\n";
        hardward += "MaxVertexVecUniforms:"+StringUtils(openGL.hardward(GLState::Hardward::VERTEX_VEC_UNIFORM)).str()+"\n";
        hardward += "MaxGeometryInput:"+StringUtils(openGL.hardward(GLState::Hardward::GEOMETRY_IN)).str()+"\n";
        hardward += "MaxGeometryOutput:"+StringUtils(openGL.hardward(GLState::Hardward::GEOMETRY_OUT)).str()+"\n";
        hardward += "MaxGeometryOutputVertices:"+StringUtils(openGL.hardward(GLState::Hardward::GEOMETRY_OUT_VERTICES)).str()+"\n";
        hardward += "MaxGeometryUniform:"+StringUtils(openGL.hardward(GLState::Hardward::GEOMETRY_UNIFORM)).str()+"\n";
        hardward += "MaxFragmentInput:"+StringUtils(openGL.hardward(GLState::Hardward::FRAGMENT_IN)).str()+"\n";
        hardward += "MaxFragmentOutput:"+StringUtils(openGL.hardward(GLState::Hardward::FRAGMENT_OUT)).str()+"\n";
        hardward += "MaxFragmentUniform:"+StringUtils(openGL.hardward(GLState::Hardward::FRAGMENT_VEC_UNIFORM)).str()+"\n";
        hardward += "MajorVersion:"+StringUtils(openGL.hardward(GLState::Hardward::MAJOR_VERSION)).str()+"\n";
        hardward += "MinorVersion:"+StringUtils(openGL.hardward(GLState::Hardward::MINOR_VERSION)).str()+"\n";
        return hardward;
    }
}
}
