#ifndef MATERIAL_H
#define MATERIAL_H

#include "GL/glew.h"
#include "core/core.h"

#include "Shader.h"
#include "MeshBuffers.h"
#include "Texture.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

    class Material
    {
    public:

        enum CompareFunc
        {
            EQUAL=0,
            NOTEQUAL,
            LESS,
            GREATER,
            LEQUAL,
            GEQUAL,
            NEVER,
            ALWAYS,
        };

        enum BlendFunc
        {
            ZERO=0,
            ONE,
            SRC_COLOR,
            ONE_MINUS_SRC_COLOR,
            DST_COLOR,
            ONE_MINUS_DST_COLOR,
            SRC_ALPHA,
            ONE_MINUS_SRC_ALPHA,
            DST_ALPHA,
            ONE_MINUS_DST_ALPHA,
            CONSTANT_COLOR,
            ONE_MINUS_CONSTANT_COLOR,
            CONSTANT_ALPHA,
            ONE_MINUS_CONSTANT_ALPHA,
        };

        enum BlendEquation
        {
            ADD=0,
            SUB,
            REVERSE_SUB,
            MIN,
            MAX,
        };

        struct OptionalMaterial
        {
            bool scissor;
            ivec2 scissorCoord, scissorSize;

            bool logicColor;
            GLenum opcode;
        };

        Material();
        virtual ~Material();

    private:

        /* Material */
        Shader* _shader;
        MeshBuffers* _mesh;
        // texture

        vec4 _color;
        vec4 _material; // ambient, diffuse, specular, emisive
        int _exponent;

        bool _depthTest, _writeDepthBuffer;
        CompareFunc _depthFunc;
        Vector4<bool> _writeColor;

        bool _alphaTest;
        float _aplhaTreshold;
        CompareFunc _alphaFunc;

        bool _cullFace, _cullBackFace;

        bool _blend;
        BlendEquation _blendEquation;
        Vector2<BlendFunc> _blendFunc;

        OptionalMaterial* _optional = nullptr;
    };

}
}
#include "MemoryLoggerOn.h"

#endif // MATERIAL_H
