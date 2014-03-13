#ifndef MATERIAL_H
#define MATERIAL_H

#include "core/core.h"

#include "Shader.h"
#include "MeshBuffers.h"
#include "Texture.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
    namespace resource{ class MaterialManager; }

namespace renderer
{

    class Material
    {
        friend class resource::MaterialManager;

    public:
        static const Material& random();

        enum CompareFunc
        {
            NEVER=0,
            NOTEQUAL,
            LESS,
            LEQUAL,
            GREATER,
            GEQUAL,
            EQUAL,
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
            bool scissor=false;
            uivec2 scissorCoord, scissorSize={512,512};

            bool logicColor=false;
            GLenum opcode=GL_XOR;
        };

        Material();
        virtual ~Material();
        Material(const Material&);

        size_t index() const;
        void beginModif();
        void endModif();
        Material* clone(const std::string& name="");

        bool operator<(const Material&) const;
        bool operator==(const Material&) const;
        bool operator!=(const Material&) const;
        Material& operator=(const Material&);

        bool bind() const;

        Shader* shader() const;
        MeshBuffers* mesh() const;
        Texture* texture(size_t unit) const;
        size_t nbTexture() const;
        const vec4& color() const;
        const vec4& material() const;
        float exponent() const;
        bool depthTest() const;
        bool writeDepth() const;
        CompareFunc depthFunc() const;
        const Vector4<bool>& writeColor() const;
        bool alphaTest() const;
        float alphaThreshold() const;
        CompareFunc alphaFunc() const;
        bool cullFace() const;
        bool cullBackFace() const;
        bool blend() const;
        BlendEquation blendEquation() const;
        const Vector2<BlendFunc>& blendFunc() const;
        bool alwaysFirst() const;
        bool alwaysLast() const;
        core::Option<OptionalMaterial> optionalMaterial() const;

        Material& setShader(Shader*);
        Material& setMesh(MeshBuffers*);
        Material& setTexture(Texture*, size_t);
        Material& setNbTexture(size_t);
        Material& setColor(const vec4&);
        Material& setMaterial(const vec4&);
        Material& setExponent(float);
        Material& setDepthTest(bool);
        Material& setWriteDepth(bool);
        Material& setDepthFunc(CompareFunc);
        Material& setWriteColor(const Vector4<bool>&);
        Material& setAlphaTest(bool);
        Material& setAlphaThreshold(float);
        Material& setAlphaFunc(CompareFunc);
        Material& setCullFace(bool);
        Material& setCullBackFace(bool);
        Material& setBlend(bool);
        Material& setBlendEquation(BlendEquation);
        Material& setBlendFunc(const Vector2<BlendFunc>&);
        Material& setAlwaysFisrt(bool);
        Material& setAlwaysLast(bool);
        Material& createOptionalMaterial();
        Material& setScissor(bool);
        Material& setScissorCoord(const uivec2&);
        Material& setScissorSize(const uivec2&);
        Material& setLogicColor(bool);
        Material& setOpcode(GLenum);

        /* out */
        std::string str(const std::string& sep = "\n") const;
        friend std::ostream& operator<< (std::ostream& stream, const Material& m) { stream << m.str(); return stream; }

        static uint toGLBlendFunc(BlendFunc);
        static uint toGLComparFunc(CompareFunc);
        static uint toGLBlendEquation(BlendEquation);

    private:
        size_t _index=0;
        resource::MaterialManager* _manager=nullptr;
        bool _inManager=false;
        size_t _inManagerId=0;

        /* Material */
        Shader* _shader = nullptr;
        MeshBuffers* _mesh = nullptr;
        Texture** _textures = nullptr;
        size_t _nbTexture = 0;

        vec4 _color=vec4(1);
        vec4 _material={1,1,0,0}; // ambient, diffuse, specular, emisive
        float _exponent=0;

        bool _depthTest=true, _writeDepthBuffer=true;
        CompareFunc _depthFunc=LESS;
        Vector4<bool> _writeColor = Vector4<bool>(true,true,true,true);

        bool _alphaTest=false;
        float _alphaThreshold=0.1;
        CompareFunc _alphaFunc=GREATER;

        bool _cullFace=true, _cullBackFace=true;

        bool _blend=false;
        BlendEquation _blendEquation=ADD;
        Vector2<BlendFunc> _blendFunc={SRC_ALPHA, ONE_MINUS_SRC_ALPHA};

        OptionalMaterial* _optional = nullptr;
        bool _alwaysFirst=false;
        bool _alwaysLast=false;

    };

    inline Material::Material(const Material& m) { *this = m; }
    inline size_t Material::index() const { return _index; }

    inline bool Material::operator!=(const Material& m) const
    { return !(*this==m); }

    inline Shader* Material::shader() const { return _shader; }
    inline MeshBuffers* Material::mesh() const { return _mesh; }
    inline Texture* Material::texture(size_t unit) const { if(unit<_nbTexture) return _textures[unit]; else return nullptr; }
    inline size_t Material::nbTexture() const { return _nbTexture; }
    inline const vec4& Material::color() const { return _color; }
    inline const vec4& Material::material() const { return _material; }
    inline float Material::exponent() const { return _exponent; }
    inline bool Material::depthTest() const { return _depthTest; }
    inline bool Material::writeDepth() const { return _writeDepthBuffer; }
    inline Material::CompareFunc Material::depthFunc() const { return _depthFunc; }
    inline const Vector4<bool>& Material::writeColor() const { return _writeColor; }
    inline bool Material::alphaTest() const { return _alphaTest; }
    inline float Material::alphaThreshold() const { return _alphaThreshold; }
    inline Material::CompareFunc Material::alphaFunc() const { return _alphaFunc; }
    inline bool Material::cullFace() const { return _cullFace; }
    inline bool Material::cullBackFace() const { return _cullBackFace; }
    inline bool Material::blend() const { return _blend; }
    inline Material::BlendEquation Material::blendEquation() const { return _blendEquation; }
    inline const Vector2<Material::BlendFunc>& Material::blendFunc() const { return _blendFunc; }
    inline bool Material::alwaysFirst() const { return _alwaysFirst; }
    inline bool Material::alwaysLast() const { return _alwaysLast; }
    inline core::Option<Material::OptionalMaterial> Material::optionalMaterial() const
    { if(_optional) return Option<Material::OptionalMaterial>(*_optional); else return Option<Material::OptionalMaterial>(); }

    inline uint Material::toGLBlendFunc(BlendFunc f)
    {
        switch(f)
        {
            case BlendFunc::CONSTANT_ALPHA:return GL_CONSTANT_ALPHA;
            case BlendFunc::CONSTANT_COLOR:return GL_CONSTANT_COLOR;
            case BlendFunc::DST_ALPHA:return GL_DST_ALPHA;
            case BlendFunc::ONE:return GL_ONE;
            case BlendFunc::ONE_MINUS_CONSTANT_ALPHA:return GL_ONE_MINUS_CONSTANT_ALPHA;
            case BlendFunc::ONE_MINUS_CONSTANT_COLOR:return GL_ONE_MINUS_CONSTANT_COLOR;
            case BlendFunc::ONE_MINUS_DST_ALPHA:return GL_ONE_MINUS_DST_ALPHA;
            case BlendFunc::ONE_MINUS_DST_COLOR:return GL_ONE_MINUS_DST_COLOR;
            case BlendFunc::ONE_MINUS_SRC_ALPHA:return GL_ONE_MINUS_SRC_ALPHA;
            case BlendFunc::ONE_MINUS_SRC_COLOR:return GL_ONE_MINUS_SRC_COLOR;
            case BlendFunc::SRC_ALPHA:return GL_SRC_ALPHA;
            case BlendFunc::SRC_COLOR:return GL_SRC_COLOR;
            case BlendFunc::ZERO:return GL_ZERO;
        }
    }

    inline uint Material::toGLComparFunc(CompareFunc f)
    {
        switch(f)
        {
            case CompareFunc::ALWAYS:return GL_ALWAYS;
            case CompareFunc::NEVER:return GL_NEVER;
            case CompareFunc::NOTEQUAL:return GL_NOTEQUAL;
            case CompareFunc::LESS:return GL_LESS;
            case CompareFunc::LEQUAL:return GL_LEQUAL;
            case CompareFunc::GREATER:return GL_GREATER;
            case CompareFunc::GEQUAL:return GL_GEQUAL;
            case CompareFunc::EQUAL:return GL_EQUAL;
        }
    }

    inline uint Material::toGLBlendEquation(BlendEquation e)
    {
        switch(e)
        {
            case BlendEquation::ADD:return GL_FUNC_ADD;
            case BlendEquation::SUB:return GL_FUNC_SUBTRACT;
            case BlendEquation::REVERSE_SUB:return GL_FUNC_REVERSE_SUBTRACT;
            case BlendEquation::MIN:return GL_MIN;
            case BlendEquation::MAX:return GL_MAX;
        }
    }
}
}
#include "MemoryLoggerOn.h"

#endif // MATERIAL_H
