#include "Material.h"
#include "resource/MaterialManager.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

Material::Material()
{
    //ctor
}

Material::~Material()
{
    if(_counterData)
    {
        if(_mesh)
            _mesh->decrementRef();

        for(size_t i=0 ; i<_nbTexture ; i++)
        {
            if(_textures[i])
                _textures[i]->decrementRef();
        }
    }

    delete[] _textures;
    delete[] _userData;
    delete _optional;
    delete _counterData;
}

void Material::beginModif()
{
    if(_manager && _inManager)
    {
        _inManager=false;
        _manager->pop(this);
    }
}

void Material::endModif()
{
    if(_manager && !_inManager)
    {
        _inManager=true;
        _manager->push(this);
    }
}

bool Material::operator<(const Material& m) const
{
    #define TESTb(x,y) if((x)!=(y))return (x)
    #define TEST(x,y) if((x)!=(y))return (x)<(y)

    TESTb(_alwaysFirst, m._alwaysFirst);
    TESTb(m._alwaysLast, _alwaysLast);
    TEST(_id, m._id);

    TESTb(m._blend, _blend);
    if(_blend)
    {
        TEST(m._blendEquation, _blendEquation);
        TEST(m._blendFunc, _blendFunc);
    }

    TESTb(m._depthTest, _depthTest);
    if(_depthTest)
        TEST(_depthFunc, m._depthFunc);

    TESTb(_writeDepthBuffer, m._writeDepthBuffer);

    TESTb(m._cullFace, _cullFace);
    if(_cullFace)
        TEST(_cullBackFace, m._cullBackFace);

    TESTb(m._alphaTest, _alphaTest);
    if(_alphaTest)
    {
        TEST(_alphaFunc, m._alphaFunc);
        TEST(_alphaThreshold, m._alphaThreshold);
    }

    TEST(_writeColor, m._writeColor);
    if(_optional && m._optional)
    {
        TESTb(_optional->scissor, m._optional->scissor);
        if(_optional->scissor)
        {
            TEST(_optional->scissorCoord, m._optional->scissorCoord);
            TEST(_optional->scissorSize, m._optional->scissorSize);
        }
        TESTb(_optional->logicColor, m._optional->logicColor);
        if(_optional->logicColor)
            TEST(_optional->opcode, m._optional->opcode);
    }
    else TEST(_optional, m._optional);

    TEST(_mesh, m._mesh);
    TEST(_shader, m._shader);

    for(size_t i=0 ; i<std::min(_nbTexture, m._nbTexture) ; ++i)
    {
        if(_textures[i]<m._textures[i]) return true;
        else if(_textures[i]>m._textures[i]) return false;
    }
    TEST(m._nbTexture, _nbTexture);

    for(size_t i=0 ; i<std::min(_userDataSize, m._userDataSize) ; ++i)
    {
        if(_userData[i]<m._userData[i]) return true;
        else if(_userData[i]!=m._userData[i]) return false;
    }
    TEST(m._userDataSize, _userDataSize);

    TEST(_inManagerId, m._inManagerId);

    return false;
}

bool Material::operator==(const Material& m) const
{
    if(_alwaysFirst != m._alwaysFirst || _alwaysLast != m._alwaysLast || _id != m._id)
        return false;

    if(_blend && m._blend)
    {
        if(_blendEquation != m._blendEquation || _blendFunc != m._blendFunc)
            return false;
    }
    else if(_blend != m._blend) return false;

    if(_depthTest && m._depthTest)
    {
        if(_depthFunc != m._depthFunc)
            return false;
    }
    else if(_depthTest != m._depthTest) return false;

    if(_writeDepthBuffer != m._writeDepthBuffer)
        return false;

    if(_cullFace && m._cullFace)
    {
        if(_cullBackFace != m._cullBackFace)
            return false;
    }
    else if(_cullFace != m._cullFace) return false;

    if(_alphaTest && m._alphaTest)
    {
        if(_alphaThreshold != m._alphaThreshold || _alphaFunc != m._alphaFunc)
            return false;
    }
    else if(_alphaTest != m._alphaTest) return false;

    if(_writeColor != m._writeColor)
        return false;

    if(_optional && m._optional)
    {
        if(_optional->scissor && m._optional->scissor)
        {
            if(_optional->scissorCoord != m._optional->scissorCoord || _optional->scissorSize != m._optional->scissorSize)
                return false;
        }
        else if(_optional->scissor != m._optional->scissor)
            return false;

        if(_optional->logicColor && m._optional->logicColor)
        {
            if(_optional->opcode != m._optional->opcode)
                return false;
        }
        else if(_optional->logicColor != m._optional->logicColor)
            return false;
    }
    else if(_optional != m._optional)
        return false;

    if(_mesh != m._mesh || _shader != m._shader)
        return false;

    if(_nbTexture==m._nbTexture)
    {
        for(size_t i=0 ; i<_nbTexture ; ++i)
        {
            if(_textures[i] != m._textures[i])
                return false;
        }
    }
    else return false;

    if(_userDataSize==m._userDataSize)
    {
        for(size_t i=0 ; i<_userDataSize ; ++i)
        {
            if(_userData[i] != m._userData[i])
                return false;
        }
    }
    else return false;

    return true;
}

Material& Material::operator=(const Material& m)
{
    beginModif();
    _alwaysFirst=m._alwaysFirst; _alwaysLast=m._alwaysLast;
    _blend=m._blend; _blendEquation=m._blendEquation;
    _blendFunc=m._blendFunc; _depthTest=m._depthTest;
    _depthFunc=m._depthFunc; _writeDepthBuffer=m._writeDepthBuffer;
    _cullFace=m._cullFace; _cullBackFace=m._cullBackFace;
    _alphaTest=m._alphaTest; _alphaFunc=m._alphaFunc;
    _alphaThreshold=m._alphaThreshold; _writeColor=m._writeColor;
    _material=m._material; _exponent=m._exponent;
    _color=m._color; _shader=m._shader;
    _nbTexture=m._nbTexture;
    _id=m._id;

   setMesh(m._mesh);

    if(m._optional)
    {
        createOptionalMaterial();
        _optional->scissor=m._optional->scissor; _optional->scissorCoord=m._optional->scissorCoord;
        _optional->scissorSize=m._optional->scissorSize; _optional->logicColor=m._optional->logicColor;
        _optional->opcode=m._optional->opcode;
    }
    setNbTexture(m._nbTexture);
    for(size_t i=0 ; i<_nbTexture ; ++i)
    {
        setTexture(m._textures[i], i);
    }

    setUserDataSize(m._userDataSize);
    for(size_t i=0 ; i<_userDataSize ; ++i)
    {
        setUserData(m._userData[i], i);
    }

    endModif();
}

Material* Material::clone(const std::string& name)
{
    Material* clone = new Material();
    *clone = *this;
    if(_manager)
        _manager->add(clone, name);

    return clone;
}

std::string Material::str(const std::string& sep) const
{
    std::string s="Material(";
    s+=std::string(sep)+"alwaysFirst="+StringUtils(_alwaysFirst).str();
    s+=sep+"alwaysLast="+StringUtils(_alwaysLast).str();
    s+=sep+"blend="+StringUtils(_blend).str();
    s+=sep+"blendEquation="+StringUtils(_blendEquation).str();
    s+=sep+"blendFunc="+StringUtils(_blendFunc).str();
    s+=sep+"depthTest="+StringUtils(_depthTest).str();
    s+=sep+"depthFunc="+StringUtils(_depthFunc).str();
    s+=sep+"writeDepth="+StringUtils(_writeDepthBuffer).str();
    s+=sep+"cullFace="+StringUtils(_cullFace).str();
    s+=sep+"cullBackFace="+StringUtils(_cullBackFace).str();
    s+=sep+"alphaTest="+StringUtils(_alphaTest).str();
    s+sep+="alphaFunc="+StringUtils(_alphaFunc).str();
    s+=sep+"alphaThreshold="+StringUtils(_alphaThreshold).str();
    s+=sep+"writeColor="+StringUtils(_writeColor).str();
    s+=sep+"nbTexture="+StringUtils(_nbTexture).str();
    s+=sep+"material="+StringUtils(_material).str();
    s+=sep+"exponent="+StringUtils(_exponent).str();
    s+=sep+"color="+StringUtils(_color).str();
    s+=sep+"id="+StringUtils(_id).str();
    if(_optional)
    {
        s+=sep+"scissor="+StringUtils(_optional->scissor).str();
        s+=sep+"scissorCoord="+StringUtils(_optional->scissorCoord).str();
        s+=sep+"scissorSize="+StringUtils(_optional->scissorSize).str();
        s+=sep+"logicColor="+StringUtils(_optional->logicColor).str();
        s+=sep+"opcode="+StringUtils(_optional->opcode).str();
    }
    s += ")";
    return s;
}

const Material& Material::random()
{
    static Material m;
    m.setAlphaFunc((CompareFunc)(Rand::rand()%(CompareFunc::ALWAYS+1)));
    m.setAlphaTest(Rand::rand()%2);
    m.setAlphaThreshold(Rand::frand());
    m.setAlwaysFisrt(Rand::rand()%2 && Rand::rand()%2);
    m.setAlwaysLast(Rand::rand()%2 && Rand::rand()%2);
    m.setBlend(Rand::rand()%2);
    m.setBlendEquation((BlendEquation)(Rand::rand()%(BlendEquation::MAX+1)));
    m.setBlendFunc({(BlendFunc)(Rand::rand()%(BlendFunc::ONE_MINUS_CONSTANT_ALPHA+1)), (BlendFunc)(Rand::rand()%(BlendFunc::ONE_MINUS_CONSTANT_ALPHA+1))});
    m.setColor({Rand::frand(),Rand::frand(),Rand::frand(),Rand::frand()});
    m.setCullBackFace(Rand::rand()%2);
    m.setCullFace(Rand::rand()%2);
    m.setDepthFunc((CompareFunc)(Rand::rand()%(CompareFunc::ALWAYS+1)));
    m.setDepthTest(Rand::rand()%2);
    m.setExponent(Rand::rand()%255);
    m.setMaterial({Rand::frand(),Rand::frand(),Rand::frand(),Rand::frand()});
    m.setWriteColor(Vector4<bool>(Rand::rand()%2,Rand::rand()%2,Rand::rand()%2,Rand::rand()%2));
    m.setWriteDepth(Rand::rand()%2);
    return m;
}

#define MANAGE_IN if(_manager){if(_inManager)_manager->pop(this);}
#define MANAGE_OUT if(_manager){if(_inManager)_manager->push(this);}

Material& Material::setShader(Shader* x) { MANAGE_IN _shader=x; MANAGE_OUT return *this; }
Material& Material::setMesh(MeshBuffers* x)
{
    MANAGE_IN
    if(_mesh && _counterData)
        _mesh->decrementRef();
    _mesh=x;
    if(_mesh && _counterData)
        _mesh->incrementRef();
    MANAGE_OUT
    return *this;
}
Material& Material::setTexture(Texture* x, size_t s)
{
    MANAGE_IN

    if(s<_nbTexture)
    {
        if(_textures[s] && _counterData)
            _textures[s]->decrementRef();

        _textures[s]=x;
        if(_textures[s] && _counterData)
            _textures[s]->incrementRef();
    }

    MANAGE_OUT
    return *this;
}

Material& Material::setNbTexture(size_t x)
{
    MANAGE_IN
    if(!x)
    {
        delete[] _textures;
        _textures=nullptr;
    }
    else
    {
        Texture** tmp=_textures;
        _textures=new Texture*[x];
        for(size_t i=0 ; i<std::min(_nbTexture, x) ; ++i)
            _textures[i] = tmp[i];
        delete[] tmp;
    }
    _nbTexture=x;
     MANAGE_OUT
     return *this;
}

Material& Material::setUserDataSize(size_t x)
{
    MANAGE_IN
    if(!x)
    {
        delete[] _userData;
        _userData=nullptr;
    }
    else
    {
        vec4* tmp=_userData;
        _userData=new vec4[x];
        for(size_t i=0 ; i<std::min(_userDataSize, x) ; ++i)
            _userData[i] = tmp[i];
        delete[] tmp;
    }
    _userDataSize=x;
    MANAGE_OUT
    return *this;
}

Material& Material::setUserData(const vec4& v, size_t x)
{
    MANAGE_IN
    if(x < _userDataSize)
    {
        _userData[x] = v;
    }
    MANAGE_OUT
    return *this;
}

Material& Material::setColor(const vec4& x) { MANAGE_IN _color=x; MANAGE_OUT return *this; }
Material& Material::setMaterial(const vec4& x) { MANAGE_IN _material=x; MANAGE_OUT return *this; }
Material& Material::setExponent(float x) { MANAGE_IN _exponent=x; MANAGE_OUT return *this; }
Material& Material::setDepthTest(bool x) { MANAGE_IN _depthTest=x; MANAGE_OUT return *this; }
Material& Material::setWriteDepth(bool x) { MANAGE_IN _writeDepthBuffer=x; MANAGE_OUT return *this; }
Material& Material::setDepthFunc(CompareFunc x) { MANAGE_IN _depthFunc=x; MANAGE_OUT return *this; }
Material& Material::setWriteColor(const Vector4<bool>& x) { MANAGE_IN _writeColor=x; MANAGE_OUT return *this; }
Material& Material::setAlphaTest(bool x) { MANAGE_IN _alphaTest=x; MANAGE_OUT return *this; }
Material& Material::setAlphaThreshold(float x) { MANAGE_IN _alphaThreshold=x; MANAGE_OUT return *this; }
Material& Material::setAlphaFunc(CompareFunc x) { MANAGE_IN _alphaFunc=x; MANAGE_OUT return *this; }
Material& Material::setCullFace(bool x) { MANAGE_IN _cullFace=x; MANAGE_OUT return *this; }
Material& Material::setCullBackFace(bool x) { MANAGE_IN _cullBackFace=x; MANAGE_OUT return *this; }
Material& Material::setBlend(bool x) { MANAGE_IN _blend=x; MANAGE_OUT return *this; }
Material& Material::setBlendEquation(BlendEquation x) { MANAGE_IN _blendEquation=x; MANAGE_OUT return *this; }
Material& Material::setBlendFunc(const Vector2<BlendFunc>& x) { MANAGE_IN _blendFunc=x; MANAGE_OUT return *this; }
Material& Material::setAlwaysFisrt(bool x) { MANAGE_IN _alwaysFirst=x; MANAGE_OUT return *this; }
Material& Material::setAlwaysLast(bool x) { MANAGE_IN _alwaysLast=x; MANAGE_OUT return *this; }
Material& Material::createOptionalMaterial() { MANAGE_IN if(!_optional)_optional=new OptionalMaterial; MANAGE_OUT return *this; }
Material& Material::setScissor(bool x) { MANAGE_IN if(_optional)_optional->scissor=x; MANAGE_OUT return *this; }
Material& Material::setScissorCoord(const uivec2& x) { MANAGE_IN if(_optional)_optional->scissorCoord; MANAGE_OUT return *this; }
Material& Material::setScissorSize(const uivec2& x) { MANAGE_IN if(_optional)_optional->scissorSize=x; MANAGE_OUT return *this; }
Material& Material::setLogicColor(bool x) { MANAGE_IN if(_optional)_optional->logicColor=x; MANAGE_OUT return *this; }
Material& Material::setOpcode(GLenum x) { MANAGE_IN if(_optional)_optional->opcode=x; MANAGE_OUT return *this; }
Material& Material::setId(int id) { MANAGE_IN _id=id; MANAGE_OUT return *this; }

bool Material::bind() const
{
    openGL.depthTest(_depthTest);
    if(_depthTest)
    {
        openGL.depthMask(_writeDepthBuffer);
        openGL.depthFunc(toGLComparFunc(_depthFunc));
    }
    openGL.colorMask(_writeColor);
    openGL.alphaTest(_alphaTest);
    if(_alphaTest)
        openGL.alphaFunc(toGLComparFunc(_alphaFunc), _alphaThreshold);
    openGL.cullFace(_cullFace);
    if(_cullFace)
    {
        if(_cullBackFace)
            openGL.cullFaceMode(GL_BACK);
        else
            openGL.cullFaceMode(GL_FRONT);
    }
    openGL.blend(_blend);
    if(_blend)
    {
        openGL.blendEquation(toGLBlendEquation(_blendEquation));
        openGL.blendFunc({toGLBlendFunc(_blendFunc.x()), toGLBlendFunc(_blendFunc.y())});
    }

    if(_optional)
    {
        openGL.logicColor(_optional->logicColor);
        if(_optional->logicColor)
            openGL.logicOp(_optional->opcode);
        openGL.scissorTest(_optional->scissor);
        if(_optional->scissor)
            openGL.scissorParam(_optional->scissorCoord, _optional->scissorSize);
    }
    else
    {
        openGL.logicColor(false);
        openGL.scissorTest(false);
    }

    if(_shader)
    {
        _shader->bind();
        _shader->setUniform(_color, _shader->engineUniformId(Shader::EngineUniform::COLOR));
        _shader->setUniform(_material, _shader->engineUniformId(Shader::EngineUniform::MATERIAL));
        _shader->setUniform(_exponent, _shader->engineUniformId(Shader::EngineUniform::EXPONENT));

        for(size_t i=0 ; i<std::min(_userDataSize,16u) ; ++i)
        {
            _shader->setUniform(_userData[i], _shader->engineUseDataId(i));
        }

        for(size_t i=0 ; i<_nbTexture ; ++i)
        {
            if(_textures[i])
                _shader->setUniform(_textures[i]->scale(), _shader->engineTextureScaleId(i));
        }

        if(_mesh)
        {
            if(_mesh->vertexBuffer())
            {
                openGL.bindArrayBuffer(_mesh->vertexBuffer()->id());
                glBindBuffer(GL_ARRAY_BUFFER,_mesh->vertexBuffer()->id() );

                bool used[VertexBuffer::MAX_VAO_ATTRIB] = {false};

                int id = _shader->engineAttribId(Shader::EngineAttrib::VERTEX);
                if(id >= 0)
                {
                    _mesh->vertexBuffer()->bindVertexAttrib(id, VertexBuffer::vertexEngineAttrib(_mesh->vertexBuffer()->format(), VertexBuffer::VertexAttrib::VERTEX));
                    used[id] = true;
                }

                id = _shader->engineAttribId(Shader::EngineAttrib::NORMAL);
                if(id >= 0)
                {
                    _mesh->vertexBuffer()->bindVertexAttrib(id, VertexBuffer::vertexEngineAttrib(_mesh->vertexBuffer()->format(), VertexBuffer::VertexAttrib::NORMAL));
                    used[id] = true;
                }

                id = _shader->engineAttribId(Shader::EngineAttrib::TEXCOORD);
                if(id >= 0)
                {
                    _mesh->vertexBuffer()->bindVertexAttrib(id, VertexBuffer::vertexEngineAttrib(_mesh->vertexBuffer()->format(), VertexBuffer::VertexAttrib::TEXCOORD));
                    used[id] = true;
                }

                id = _shader->engineAttribId(Shader::EngineAttrib::TANGENT);
                if(id >= 0)
                {
                    _mesh->vertexBuffer()->bindVertexAttrib(id, VertexBuffer::vertexEngineAttrib(_mesh->vertexBuffer()->format(), VertexBuffer::VertexAttrib::TANGENT));
                    used[id] = true;
                }

                for(uint i=0 ; i<VertexBuffer::MAX_VAO_ATTRIB ; ++i)
                {
                    if(!used[i])
                        _mesh->vertexBuffer()->bindVertexAttrib(i, {0,0});
                }
            }
            else return false;

            if(_mesh->indexBuffer())
                openGL.bindElementArrayBuffer(_mesh->indexBuffer()->id());
            else return false;
        }
        else return false;

        for(size_t i=0 ; i<_nbTexture ; ++i)
        {
            if(_textures[i])
            {
                openGL.bindTexture(_textures[i]->id(), Texture::toGLType(_textures[i]->type()), i);
                if(_textures[i]->sampler())
                    openGL.bindTextureSampler(i, _textures[i]->sampler()->id());
                else
                    openGL.bindTextureSampler(i, 0);
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}

}
}
