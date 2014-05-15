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

    bool GLState::assertGLError(const char* file, size_t line)
    {
        GLenum glerror=GL_NO_ERROR;
        bool e=true;

        while ( (glerror = glGetError()) != GL_NO_ERROR)
        {
            e=false;
            err("Assertion failed, ")<<file<<" at line " << line << " -> glGetError():"<<glerror<<std::endl;
        }

        if(!e)
            exit(-1);

        return e;
    }

    void GLState::getHardwardProperties()
    {
        glGetError();
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

        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &_hardwardProperties[ANISOTROPY_SAMPLES]);
        _hardwardProperties[ANISOTROPY]=_hardwardProperties[ANISOTROPY_SAMPLES]&&glGetError()==GL_NO_ERROR;
    }

    void GLState::resetStates()
    {
        for(int i=0 ; i<NB_STATES ; ++i)
            _glStates[i]=0;

        for(int i=0 ; i<NB_BSTATES ; ++i)
            _glBoolStates[i]=false;

        for(int i=0 ; i<MAX_TEXTURE_UNIT ; ++i)
        {
            _enabledTexture[i]=0;
            _typeEnabledTexture[i] = GL_TEXTURE_2D;
            _samplerTexture[i]=0;
        }

        _textureUnit=0;
        _viewPort[0]=uivec2();
        _viewPort[1]=uivec2(500,500);
        _drawBufferMode=GL_BACK;
        _readBufferMode=GL_BACK;
        _blendFunc={GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
        _glStates[BLEND_EQUATION]=GL_FUNC_ADD;
        _glStates[CULL_FACE_MODE]=GL_BACK;
        _glStates[DEPTH_FUNC]=GL_GREATER;
        _glStates[OPCODE]=GL_XOR;
        _alphaThreshold=0.1;
        _glStates[ALPHA_FUNC]=GL_LESS;
        _colorMask={true,true,true,true};
        _scissorCoord={0,0};
        _scissorSize={100,100};
    }

    void GLState::applyAll()
    {
        glBindBuffer(GL_ARRAY_BUFFER, _glStates[ARRAY_BUFFER]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glStates[ELEMENT_ARRAY_BUFFER]);
        glUseProgram(_glStates[SHADER]);

        for(int i=0 ; i<MAX_TEXTURE_UNIT ; ++i)
        {
            glActiveTexture(GL_TEXTURE0+i);
            glBindTexture(_typeEnabledTexture[i], _enabledTexture[i]);
            glBindSampler(i, _samplerTexture[i]);
        }

        glActiveTexture(GL_TEXTURE0+_textureUnit);
        glBindFramebuffer(GL_FRAMEBUFFER, _glStates[FRAME_BUFFER]);
        glViewport(_viewPort[0].x(), _viewPort[0].y(), _viewPort[1].x(), _viewPort[1].y());
        glDrawBuffer(_drawBufferMode);
        glReadBuffer(_readBufferMode);

        glSet(GL_DEPTH_TEST, _glBoolStates[DEPTH_TEST]);
        glSet(GL_BLEND, _glBoolStates[BLEND]);
        glSet(GL_ALPHA_TEST, _glBoolStates[ALPHA_TEST]);
        glSet(GL_CULL_FACE, _glBoolStates[CULL_FACE]);
        glSet(GL_SCISSOR_TEST, _glBoolStates[SCISSOR_TEST]);
        glSet(GL_COLOR_LOGIC_OP, _glBoolStates[LOGIC_COLOR]);

        if(_glBoolStates[DEPTH_MASK])
            glDepthMask(GL_TRUE);
        else
            glDepthMask(GL_FALSE);

        glBlendFunc(_blendFunc.x(), _blendFunc.y());
        glBlendEquation(_glStates[BLEND_EQUATION]);
        glCullFace(_glStates[CULL_FACE_MODE]);
        glAlphaFunc(_glStates[ALPHA_FUNC], _alphaThreshold);
        glDepthFunc(_glStates[DEPTH_FUNC]);
        glColorMask(_colorMask[0], _colorMask[1], _colorMask[2], _colorMask[3]);
        glScissor(_scissorCoord.x(), _scissorCoord.y(), _scissorSize.x(), _scissorSize.y());
        glLogicOp(_glStates[OPCODE]);
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
        hardward += "Anisotropy:"+StringUtils(openGL.hardward(GLState::Hardward::ANISOTROPY)).str()+"\n";
        hardward += "AnisotropySamples:"+StringUtils(openGL.hardward(GLState::Hardward::ANISOTROPY_SAMPLES)).str()+"\n";
        return hardward;
    }
}
}
