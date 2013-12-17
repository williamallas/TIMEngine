#ifndef GLSTATE_H_INCLUDED
#define GLSTATE_H_INCLUDED

#include "GL/glew.h"

#include "core.h"
#include "Singleton.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    typedef GLuint uint;

    class GLState : public Singleton<GLState>
    {
        friend class Singleton<GLState>;

    public:
        void resetStates();
        void applyAll();

        void bindArrayBuffer(uint id);
        void bindElementArrayBuffer(GLenum id);
        void bindShader(uint id);

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
        enum
        {
            ARRAY_BUFFER=0,
            ELEMENT_ARRAY_BUFFER,
            SHADER,
            NB_STATES,
        };
        uint _glStates[NB_STATES];
    };

    extern GLState& openGL;

    inline int GLState::hardward(Hardward hp) const
    {
        return _hardwardProperties[hp];
    }

    inline void GLState::bindArrayBuffer(uint id)
    {
        if(_glStates[ARRAY_BUFFER] != id)
        {
            _glStates[ARRAY_BUFFER] = id;
            glBindBuffer(GL_ARRAY_BUFFER, id);
        }
    }

    inline void GLState::bindElementArrayBuffer(uint id)
    {
        if(_glStates[ELEMENT_ARRAY_BUFFER] != id)
        {
            _glStates[ELEMENT_ARRAY_BUFFER] = id;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        }
    }

    inline void GLState::bindShader(uint id)
    {
        if(_glStates[SHADER] != id)
        {
            _glStates[SHADER] = id;
            glUseProgram(id);
        }
    }
}
}
#include "MemoryLoggerOff.h"

#endif // GLSTATE_H_INCLUDED
