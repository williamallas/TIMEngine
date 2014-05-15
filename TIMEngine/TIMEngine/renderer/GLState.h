#ifndef GLSTATE_H_INCLUDED
#define GLSTATE_H_INCLUDED

#include "GL/glew.h"

#include "core.h"
#include "Singleton.h"

#define GL_ASSERT() tim::renderer::GLState::assertGLError(__FILE__,__LINE__)
#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    static const size_t MAX_TEXTURE_UNIT=32;

    class GLState : public Singleton<GLState>
    {
        friend class Singleton<GLState>;

    public:
        void resetStates();
        void applyAll();

        static bool assertGLError(const char*, size_t);

        static void clearDepth();
        void clearColor(const vec4&);
        static void flush();
        static void finish();

        bool bindArrayBuffer(uint);
        bool bindPixelBufferUnpack(uint);
        bool bindElementArrayBuffer(uint);
        bool bindShader(uint);
        bool bindFrameBuffer(uint);
        bool bindTexture(uint, GLenum, uint);
        bool bindTextureSampler(uint, uint);

        void unbindArrayBuffer(uint);
        void unbindPixelBufferUnpack(uint);
        void unbindElementArrayBuffer(uint);
        void unbindShader(uint);
        void unbindFrameBuffer(uint);
        void unbindTexture(uint, GLenum, uint);
        void unbindTextureSampler(uint, uint);

        void setViewPort(const uivec2&, const uivec2&);
        void drawBuffer(uint);
        void readBuffer(uint);
        void depthTest(bool);
        void depthFunc(uint);
        void depthMask(bool);
        void colorMask(const Vector4<bool>&);
        void cullFace(bool);
        void cullFaceMode(uint);
        void blend(bool);
        void blendFunc(const Vector2<uint>&);
        void blendEquation(uint);
        void alphaFunc(uint, float);
        void alphaTest(bool);
        void scissorTest(bool);
        void scissorParam(const uivec2&, const uivec2&);
        void logicColor(bool);
        void logicOp(uint);

        enum Hardward
        {
            TEXTURE2D_SIZE,
            TEXTURE3D_SIZE,
            CUBEMAP_SIZE,
            FRAGMENT_TEX_UNITS,
            VERTEX_TEX_UNITS,
            GEOMETRY_TEX_UNITS,
            COMBINED_TEX_UNITS,

            IDEAL_VBO_SIZE,
            IDEAL_IBO_SIZE,

            FRAGMENT_OUT,
            FRAGMENT_IN,
            FRAGMENT_VEC_UNIFORM,

            GEOMETRY_IN,
            GEOMETRY_OUT,
            GEOMETRY_UNIFORM,
            GEOMETRY_OUT_VERTICES,

            VERTEX_OUT,
            VERTEX_ATTRIBS,
            VERTEX_VEC_UNIFORM,

            MAJOR_VERSION,
            MINOR_VERSION,

            ANISOTROPY,
            ANISOTROPY_SAMPLES,

            LAST,
        };

        int hardward(Hardward hp) const;
        void getHardwardProperties();
        std::string strHardward() const;

    protected:
        GLState();
        virtual ~GLState() {}

    private:
        GLState(const GLState&);
        GLState& operator=(const GLState&);

        /* Hardware propertys */
        int _hardwardProperties[LAST] = {0};

        /* State */
        vec4 _clearColor;
        uint _enabledTexture[MAX_TEXTURE_UNIT];
        uint _typeEnabledTexture[MAX_TEXTURE_UNIT];
        uint _samplerTexture[MAX_TEXTURE_UNIT];
        int _textureUnit;
        uivec2 _viewPort[2];
        uint _drawBufferMode, _readBufferMode;
        Vector2<uint> _blendFunc;
        float _alphaThreshold;
        Vector4<bool> _colorMask;
        uivec2 _scissorCoord, _scissorSize;

        enum
        {
            ARRAY_BUFFER=0,
            ELEMENT_ARRAY_BUFFER,
            PIXEL_BUFFER_UNPACK,
            SHADER,
            FRAME_BUFFER,
            BLEND_EQUATION,
            CULL_FACE_MODE,
            ALPHA_FUNC,
            DEPTH_FUNC,
            OPCODE,
            NB_STATES,
        };
        uint _glStates[NB_STATES];

        enum
        {
            DEPTH_TEST=0,
            DEPTH_MASK,
            BLEND,
            CULL_FACE,
            ALPHA_TEST,
            SCISSOR_TEST,
            LOGIC_COLOR,
            NB_BSTATES,
        };
        bool _glBoolStates[NB_BSTATES];

        static void glSet(uint, bool);
    };

    extern GLState& openGL;

    inline void GLState::glSet(uint s, bool b)
    {
        if(b) glEnable(s);
        else glDisable(s);
    }

    inline int GLState::hardward(Hardward hp) const
    {
        return _hardwardProperties[hp];
    }

    inline bool GLState::bindArrayBuffer(uint id)
    {
        if(_glStates[ARRAY_BUFFER] != id)
        {
            _glStates[ARRAY_BUFFER] = id;
            glBindBuffer(GL_ARRAY_BUFFER, id);
            return true;
        }
        return false;
    }

    inline bool GLState::bindElementArrayBuffer(uint id)
    {
        if(_glStates[ELEMENT_ARRAY_BUFFER] != id)
        {
            _glStates[ELEMENT_ARRAY_BUFFER] = id;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
            return true;
        }
        return false;
    }

    inline bool GLState::bindPixelBufferUnpack(uint id)
    {
        if(_glStates[PIXEL_BUFFER_UNPACK] != id)
        {
            _glStates[PIXEL_BUFFER_UNPACK] = id;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, id);
            return true;
        }
        return false;
    }

    inline bool GLState::bindShader(uint id)
    {
        if(_glStates[SHADER] != id)
        {
            _glStates[SHADER] = id;
            glUseProgram(id);
            return true;
        }
        return false;
    }

    inline bool GLState::bindTexture(uint id, GLenum type, uint unit)
    {
        if(_textureUnit != unit)
        {
            _textureUnit=unit;
            glActiveTexture(GL_TEXTURE0+_textureUnit);
        }

        if(_enabledTexture[unit] != id)
        {
            _enabledTexture[unit] = id;
            _typeEnabledTexture[unit] = type;
            glBindTexture(type, id);
            return true;
        }
        return false;
    }

    inline bool  GLState::bindTextureSampler(uint unit, uint sampler)
    {
        if(_samplerTexture[unit] != sampler)
        {
            _samplerTexture[unit]=sampler;
            glBindSampler(unit, sampler);
            return true;
        }
        return false;
    }

    inline bool GLState::bindFrameBuffer(uint id)
    {
        if(_glStates[FRAME_BUFFER] != id)
        {
            _glStates[FRAME_BUFFER]=id;
            glBindFramebuffer(GL_FRAMEBUFFER, id);
            return true;
        }
        return false;
    }

    inline void GLState::unbindArrayBuffer(uint id)
    {
        if(_glStates[ARRAY_BUFFER] == id)
        {
            _glStates[ARRAY_BUFFER] = 0;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    inline void GLState::unbindPixelBufferUnpack(uint id)
    {
        if(_glStates[PIXEL_BUFFER_UNPACK] == id)
        {
            _glStates[PIXEL_BUFFER_UNPACK] = 0;
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        }
    }

    inline void GLState::unbindElementArrayBuffer(uint id)
    {
        if(_glStates[ELEMENT_ARRAY_BUFFER] == id)
        {
            _glStates[ELEMENT_ARRAY_BUFFER] = 0;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }

    inline void GLState::unbindShader(uint id)
    {
        if(_glStates[SHADER] == id)
        {
            _glStates[SHADER] = 0;
            glUseProgram(0);
        }
    }

    inline void GLState::unbindFrameBuffer(uint id)
    {
        if(_glStates[FRAME_BUFFER] == id)
        {
            _glStates[FRAME_BUFFER] = 0;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    inline void GLState::unbindTexture(uint id, GLenum type, uint unit)
    {
        if(_textureUnit != unit)
        {
            _textureUnit=unit;
            glActiveTexture(GL_TEXTURE0+_textureUnit);
        }

        if(_enabledTexture[unit] == id)
        {
            _enabledTexture[unit] = 0;
            _typeEnabledTexture[unit] = type;
            glBindTexture(type, 0);
        }
    }

    inline void GLState::unbindTextureSampler(uint unit, uint sampler)
    {
        if(_samplerTexture[unit] == sampler)
        {
            _samplerTexture[unit]=0;
            glBindSampler(unit, 0);
        }
    }

    inline void GLState::setViewPort(const uivec2& coord, const uivec2& size)
    {
        if(coord != _viewPort[0] || size != _viewPort[1])
        {
            _viewPort[0]=coord;
            _viewPort[1]=size;
            glViewport(coord.x(), coord.y(), size.x(), size.y());
        }
    }

    inline void GLState::drawBuffer(uint mode)
    {
        if(mode != _drawBufferMode)
        {
            _drawBufferMode=mode;
            glDrawBuffer(mode);
        }
    }

    inline void GLState::readBuffer(uint mode)
    {
        if(mode != _readBufferMode)
        {
            _readBufferMode=mode;
            glReadBuffer(mode);
        }
    }

    inline void GLState::depthTest(bool b)
    {
        if(b != _glBoolStates[DEPTH_TEST])
        {
            _glBoolStates[DEPTH_TEST]=b;
            glSet(GL_DEPTH_TEST, b);
        }
    }

    inline void GLState::depthMask(bool b)
    {
        if(b != _glBoolStates[DEPTH_MASK])
        {
            _glBoolStates[DEPTH_MASK]=b;
            if(b) glDepthMask(GL_TRUE);
            else glDepthMask(GL_FALSE);
        }
    }

    inline void GLState::blend(bool b)
    {
        if(b != _glBoolStates[BLEND])
        {
            _glBoolStates[BLEND]=b;
            glSet(GL_BLEND, b);
        }
    }

    inline void GLState::alphaTest(bool b)
    {
        if(b != _glBoolStates[ALPHA_TEST])
        {
            _glBoolStates[ALPHA_TEST]=b;
            glSet(GL_ALPHA_TEST, b);
        }
    }

    inline void GLState::cullFace(bool b)
    {
        if(b != _glBoolStates[CULL_FACE])
        {
            _glBoolStates[CULL_FACE]=b;
            glSet(GL_CULL_FACE, b);
        }
    }

    inline void GLState::blendFunc(const Vector2<uint>& b)
    {
        if(b != _blendFunc)
        {
            _blendFunc=b;
            glBlendFunc(b.x(), b.y());
        }
    }

    inline void GLState::blendEquation(uint e)
    {
        if(e != _glStates[BLEND_EQUATION])
        {
            _glStates[BLEND_EQUATION]=e;
            glBlendEquation(e);
        }
    }

    inline void GLState::cullFaceMode(uint m)
    {
        if(m != _glStates[CULL_FACE_MODE])
        {
            _glStates[CULL_FACE_MODE]=m;
            glCullFace(m);
        }
    }

    inline void GLState::alphaFunc(uint func, float threshold)
    {
        if(func != _glStates[ALPHA_FUNC] || _alphaThreshold!=threshold)
        {
            _glStates[ALPHA_FUNC]=func;
            _alphaThreshold=threshold;
            glAlphaFunc(func, threshold);
        }
    }

    inline void GLState::depthFunc(uint func)
    {
        if(func != _glStates[DEPTH_FUNC])
        {
            _glStates[DEPTH_FUNC]=func;
            glDepthFunc(func);
        }
    }

    inline void GLState::colorMask(const Vector4<bool>& m)
    {
        if(_colorMask!=m)
        {
            _colorMask=m;
            glColorMask(m[0], m[1], m[2], m[3]);
        }
    }

    inline void GLState::scissorTest(bool b)
    {
        if(_glBoolStates[SCISSOR_TEST]!=b)
        {
            _glBoolStates[SCISSOR_TEST]=b;
            glSet(GL_SCISSOR_TEST, b);
        }
    }

    inline void GLState::logicColor(bool b)
    {
        if(_glBoolStates[LOGIC_COLOR]!=b)
        {
            _glBoolStates[LOGIC_COLOR]=b;
            glSet(GL_COLOR_LOGIC_OP, b);
        }
    }

    inline void GLState::scissorParam(const uivec2& coord, const uivec2& size)
    {
        if(_scissorCoord!=coord || _scissorSize!=size)
        {
            _scissorCoord=coord;
            _scissorSize=size;
            glScissor(coord.x(), coord.y(), size.x(), size.y());
        }
    }

    inline void GLState::logicOp(uint opcode)
    {
        if(_glStates[OPCODE]!=opcode)
        {
            _glStates[OPCODE]=opcode;
            glLogicOp(opcode);
        }
    }

    inline void GLState::clearDepth()
    {
        openGL.depthMask(true);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    inline void GLState::clearColor(const vec4& col)
    {
        if(_clearColor != col)
        {
            _clearColor=col;
            glClearColor(col.x(),col.y(),col.z(), col.w());
        }
        colorMask({true,true,true,true});
        glClear(GL_COLOR_BUFFER_BIT);
    }

    inline void GLState::flush()
    {
        glFlush();
    }

    inline void GLState::finish()
    {
        glFinish();
    }
}
}
#include "MemoryLoggerOff.h"

#endif // GLSTATE_H_INCLUDED
