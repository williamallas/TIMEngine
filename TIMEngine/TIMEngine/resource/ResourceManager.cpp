#include "ResourceManager.h"
#include "ResourceSceneManager.h"
#include "resource.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

ResourceManager::ResourceManager(ThreadPool* pool, TextureLoader* texLoader) : _textureManager(texLoader)
{
    _pool = pool;

    _enginePixelShader = new MetaShader(MetaShader::PIXEL_SHADER);
    _engineVertexShader = new MetaShader(MetaShader::VERTEX_SHADER);
    _engineGeometryShader = nullptr;

    _engineVertexShader->setSource(vShaderSrcGeometryPass);
    _enginePixelShader->setSource(pShaderSrcGeometryPass);
}

ResourceManager::~ResourceManager()
{
    delete _enginePixelShader;
    delete _engineVertexShader;
    delete _engineGeometryShader;

    _materialPass.clear();
}

renderer::MeshBuffers* ResourceManager::getMesh(const std::string& str)
{
    renderer::MeshBuffers* mesh = _meshManager.get(str);
    if(!mesh)
    {
        const MeshManager::MeshContext& mc=meshDataContext(str);
        if(!mc.path.empty())
        {
            mesh = _meshManager.loadMesh(str, mc);
        }
    }
    return mesh;
}

bool ResourceManager::remove(renderer::MeshBuffers* mesh)
{
    return _meshManager.remove(mesh);
}

renderer::Texture* ResourceManager::getTexture(const std::string& name)
{
    return _textureManager.getTexture(name);
}

bool ResourceManager::remove(renderer::Texture* t)
{
    return _textureManager.remove(t);
}

bool ResourceManager::async_uploadData(renderer::Texture* tex,
                      const renderer::Texture::Parameter& texParam,
                      ubyte* data,
                      const TextureLoader::ImageFormat& format)
{
    if(_pool)
        return _textureManager.async_uploadData(*_pool, tex, texParam, data, format);
    else return false;
}

renderer::MaterialPass* ResourceManager::getMaterialPass(const std::string& str)
{
    auto it = _materialPass.find(str);
    if(it != _materialPass.end())
    {
        return it->second.get();
    }
    else
    {
        auto itt = _materialPassModels.find(str);
        if(itt != _materialPassModels.end())
        {
            renderer::MaterialPass* pass = new renderer::MaterialPass;
            for(size_t i=0 ; i<itt->second.pass.size() ; ++i)
            {
                pass->setPass(getMaterial(itt->second.pass[i]), i);
            }
            _materialPass[str] = boost::shared_ptr<renderer::MaterialPass>(pass);
            resourceCollector.trace(pass);
            return pass;
        }
    }
    return nullptr;
}

renderer::MaterialPass* ResourceManager::async_getMaterialPass(const std::string& str)
{
    auto it = _materialPass.find(str);
    if(it != _materialPass.end())
    {
        return it->second.get();
    }
    else
    {
        auto itt = _materialPassModels.find(str);
        if(itt != _materialPassModels.end())
        {
            renderer::MaterialPass* pass = new renderer::MaterialPass;
            for(size_t i=0 ; i<itt->second.pass.size() ; ++i)
            {
                pass->setPass(async_getMaterial(itt->second.pass[i]), i);
            }
            _materialPass[str] = boost::shared_ptr<renderer::MaterialPass>(pass);
            resourceCollector.trace(pass);
            return pass;
        }
    }
    return nullptr;
}

bool ResourceManager::remove(const renderer::MaterialPass* pass)
{
    for(auto it=_materialPass.begin() ; it != _materialPass.end() ; it++)
    {
        if(pass == (it->second).get())
        {
            _materialPass.erase(it);
            return true;
        }
    }
    return false;
}

renderer::Material* ResourceManager::addMaterial(renderer::Material* mat, const std::string& str)
{
    return _materials.add(mat, str);
}

renderer::Material* ResourceManager::getMaterial(const std::string& str)
{
    renderer::Material* mat = _materials.get(str);

    if(!mat)
    {
        const MaterialModel* model = materialModel(str);
        if(model)
        {
            mat = new renderer::Material;
            mat->setAlphaFunc(model->alphaFunc);
            mat->setAlphaTest(model->alphaTest);
            mat->setAlphaThreshold(model->alphaThreshold);
            mat->setAlwaysFisrt(model->alwaysFirst);
            mat->setAlwaysLast(model->alwaysLast);
            mat->setBlend(model->blend);
            mat->setBlendEquation(model->blendEquation);
            mat->setBlendFunc(model->blendFunc);
            mat->setColor(model->color);
            mat->setCullBackFace(model->cullBackFace);
            mat->setCullFace(model->cullFace);
            mat->setDepthFunc(model->depthFunc);
            mat->setDepthTest(model->depthTest);
            mat->setExponent(model->exponent);
            mat->setMaterial(model->material);
            mat->setWriteColor(model->writeColor);
            mat->setWriteDepth(model->writeDepthBuffer);

            if(model->optional)
            {
                mat->createOptionalMaterial();
                mat->setLogicColor(model->logicColor);
                mat->setOpcode(model->opcode);
                mat->setScissor(model->scissor);
                mat->setScissorCoord(model->scissorCoord);
                mat->setScissorSize(model->scissorSize);
            }

            mat->setShader(getShader(model->shader));
            mat->setMesh(getMesh(model->mesh));
            mat->setNbTexture(model->textures.size());
            for(size_t i=0 ; i<model->textures.size() ; ++i)
            {
                mat->setTexture(_textureManager.getTexture(model->textures[i]), i);
            }
            mat = _materials.add(mat, str);
        }
    }
    return mat;
}

renderer::Material* ResourceManager::async_getMaterial(const std::string& str)
{
    renderer::Material* mat = _materials.get(str);

    if(!mat)
    {
        const MaterialModel* model = materialModel(str);
        if(model)
        {
            mat = new renderer::Material;
            mat->setAlphaFunc(model->alphaFunc);
            mat->setAlphaTest(model->alphaTest);
            mat->setAlphaThreshold(model->alphaThreshold);
            mat->setAlwaysFisrt(model->alwaysFirst);
            mat->setAlwaysLast(model->alwaysLast);
            mat->setBlend(model->blend);
            mat->setBlendEquation(model->blendEquation);
            mat->setBlendFunc(model->blendFunc);
            mat->setColor(model->color);
            mat->setCullBackFace(model->cullBackFace);
            mat->setCullFace(model->cullFace);
            mat->setDepthFunc(model->depthFunc);
            mat->setDepthTest(model->depthTest);
            mat->setExponent(model->exponent);
            mat->setMaterial(model->material);
            mat->setWriteColor(model->writeColor);
            mat->setWriteDepth(model->writeDepthBuffer);

            if(model->optional)
            {
                mat->createOptionalMaterial();
                mat->setLogicColor(model->logicColor);
                mat->setOpcode(model->opcode);
                mat->setScissor(model->scissor);
                mat->setScissorCoord(model->scissorCoord);
                mat->setScissorSize(model->scissorSize);
            }

            mat->setShader(getShader(model->shader));
            mat->setMesh(async_getMesh(model->mesh));

            mat->setNbTexture(model->textures.size());
            for(size_t i=0 ; i<model->textures.size() ; ++i)
            {
                mat->setTexture(_textureManager.async_getTexture(model->textures[i], *_pool), i);
            }

            mat = _materials.add(mat, str);
        }
    }
    return mat;
}

bool ResourceManager::remove(renderer::Material* mat)
{
    return _materials.remove(mat);
}

bool ResourceManager::remove(renderer::ResourceInterface* res)
{
    {
        renderer::Texture* tex=dynamic_cast<renderer::Texture*>(res);
        if(tex)
        {
            return remove(tex);
        }
    }

    {
        renderer::MeshBuffers* mesh=dynamic_cast<renderer::MeshBuffers*>(res);
        if(mesh)
        {
            return remove(mesh);
        }
    }

    return false;
}

bool ResourceManager::loadXmlDataInformation(const std::string& file, const std::string& group)
{
    TiXmlDocument doc(PATH_DATA+file);
    if(!doc.LoadFile())
        return false;

    TiXmlElement* elem=doc.FirstChildElement();

    while(elem)
    {
        if(elem->ValueStr() == "DataGroup")
        {
            if(compare(group, elem->Attribute("name")))
            {
                parseDataGroupNode(elem);
                return true;
            }
        }
        elem=elem->NextSiblingElement();
    }

    return false;
}

void ResourceManager::parseDataGroupNode(TiXmlElement* elem)
{
    elem=elem->FirstChildElement();
    while(elem)
    {
        if(elem->ValueStr() == "MeshData")
        {
            MeshManager::MeshContext mContext;
            mContext.path = str(elem->Attribute("path"));

            std::string type = str(elem->Attribute("type"));
            if(type == "OBJ")
            {
                elem->QueryBoolAttribute("tangent", &(mContext.tangent));
                mContext.format = MeshManager::MeshContext::OBJ;
            }
            else if(type=="TIM" || type.empty())
            {
                mContext.format = MeshManager::MeshContext::TIM;
            }
            else return;

            if(elem->Attribute("name"))
                _meshDataContext[str(elem->Attribute("name"))] = mContext;
        }
        else if(elem->ValueStr() == "TextureData")
        {
            TextureManager::TextureModel model;
            model.scale={1,1};
            model.type = renderer::Texture::Type::TEXTURE_2D;
            std::string name = str(elem->Attribute("name"));
            if(!name.empty())
            {
                model.data = str(elem->GetText());

                elem->QueryBoolAttribute("default", &model.parameter.defaultSampler);
                elem->QueryBoolAttribute("anisotropy", &model.parameter.anisotropy);
                elem->QueryBoolAttribute("depthMode", &model.parameter.depthMode);
                elem->QueryBoolAttribute("linear", &model.parameter.linear);
                elem->QueryBoolAttribute("trilinear", &model.parameter.trilinear);
                elem->QueryBoolAttribute("mipmap", &model.parameter.mipmap);
                elem->QueryBoolAttribute("repeat", &model.parameter.repeat);

                int level=model.parameter.anisotropyFilter;
                elem->QueryIntAttribute("anisotropyLevel", &level);
                model.parameter.anisotropyFilter = level;

                int nbComponent=4;
                elem->QueryIntAttribute("format", &nbComponent);
                if(nbComponent==4)
                    model.parameter.intern = renderer::Texture::Parameter::RGBA;
                else if(nbComponent==3)
                    model.parameter.intern = renderer::Texture::Parameter::RGB;
                else
                    model.parameter.intern = renderer::Texture::Parameter::R;

                float scaleTex=1;
                elem->QueryFloatAttribute("scale", &scaleTex);
                model.scale = {scaleTex, scaleTex};

                std::string type = str(elem->Attribute("type"));
                if(type == "" || type == "TEXTURE_2D")
                    model.type = renderer::Texture::Type::TEXTURE_2D;
                else if(type == "TEXTURE_ARRAY" || type == "TEXTURE_2D_ARRAY")
                    model.type = renderer::Texture::Type::TEXTURE_ARRAY;

                _textureManager.addModel(name, model);
            }
        }
        else if(elem->ValueStr() == "ShaderData")
        {
            std::string src = str(elem->Attribute("src"));
            std::string name = str(elem->Attribute("name"));

            if(!name.empty())
            {
                uivec3 idShaders;
                if(src.empty())
                {
                    auto vtmp=boost::move(StringUtils(str(elem->Attribute("option"))).splitWord());
                    boost::container::set<std::string> options(vtmp.begin(), vtmp.end());
                    auto optV = _engineVertexShader->compile(options);
                    auto optP = _enginePixelShader->compile(options);

                    if(!optV.hasValue() || !optP.hasValue())
                    {
                        err("Error compiling engineVertexShader:")<<_engineVertexShader->error()<<std::endl;
                        err("Error compiling enginePixelShader:")<<_enginePixelShader->error()<<std::endl<<std::endl;
                        elem=elem->NextSiblingElement();
                        continue;
                    }
                    else idShaders = {optV.value(), optP.value(), 0};
                }

                auto it=_mapShader.find(idShaders);
                if(it == _mapShader.end())
                {
                    auto shad = renderer::Shader::combine(Option<core::uint>(idShaders.x()),
                                                          Option<core::uint>(idShaders.y()),
                                                          Option<core::uint>(idShaders.z()));
                    if(!shad.hasValue())
                    {
                        err("Error linking shader ")<<name<< " "<<src<<std::endl<<renderer::Shader::lastLinkError()<<std::endl;
                        elem=elem->NextSiblingElement();
                        continue;
                    }

                    _mapShader[idShaders] = boost::shared_ptr<renderer::Shader>(shad.value());
                    _shaderLoaded[name] = shad.value();
                }
                else
                {
                    _shaderLoaded[name] = it->second.get();
                }
            }
        }
        else if(elem->ValueStr() == "Material")
        {
            std::string name = str(elem->Attribute("name"));
            if(!name.empty())
            {
                MaterialModel mat;
                parseMaterial(elem, mat);
                _materialModels[name] = mat;
            }
        }
        else if(elem->ValueStr() == "MaterialPass")
        {
            std::string name = str(elem->Attribute("name"));
            if(!name.empty())
            {
                MaterialPassModel mat;
                parseMaterialPass(elem, mat);
                _materialPassModels[name] = mat;
            }
        }

        elem=elem->NextSiblingElement();
    }
}

void ResourceManager::parseMaterial(TiXmlElement* elem, MaterialModel& model)
{
    elem=elem->FirstChildElement();

    while(elem)
    {
        if(elem->ValueStr() == "shader")
            model.shader = str(elem->GetText());
        else if(elem->ValueStr() == "mesh")
            model.mesh = str(elem->GetText());
        else if(elem->ValueStr() == "texture")
            model.textures.push_back(elem->GetText());
        else if(elem->ValueStr() == "exponent")
            model.exponent = StringUtils(str(elem->GetText())).toFloat();
        else if(elem->ValueStr() == "alphaThreshold")
            model.alphaThreshold = StringUtils(str(elem->GetText())).toFloat();

        else if(elem->ValueStr() == "depthTest")
            model.depthTest = StringUtils(str(elem->GetText())).toBool();
        else if(elem->ValueStr() == "writeDepth")
            model.writeDepthBuffer = StringUtils(str(elem->GetText())).toBool();
        else if(elem->ValueStr() == "alphaTest")
            model.alphaTest = StringUtils(str(elem->GetText())).toBool();
        else if(elem->ValueStr() == "cullFace")
            model.cullFace = StringUtils(str(elem->GetText())).toBool();
        else if(elem->ValueStr() == "cullBackFace")
            model.cullBackFace = StringUtils(str(elem->GetText())).toBool();
        else if(elem->ValueStr() == "blend")
            model.blend = StringUtils(str(elem->GetText())).toBool();
        else if(elem->ValueStr() == "alwaysFirst")
            model.alwaysFirst = StringUtils(str(elem->GetText())).toBool();
        else if(elem->ValueStr() == "alwaysLast")
            model.alwaysLast = StringUtils(str(elem->GetText())).toBool();

        else if(elem->ValueStr() == "depthFunc")
            model.depthFunc = MaterialModel::comparFuncToString(str(elem->GetText()));
        else if(elem->ValueStr() == "alphaFunc")
            model.alphaFunc = MaterialModel::comparFuncToString(str(elem->GetText()));
        else if(elem->ValueStr() == "blendEquation")
            model.blendEquation = MaterialModel::blendEquationToString(str(elem->GetText()));

        else if(elem->ValueStr() == "blendFunc")
        {
            auto vec_str = toStrVec<2>(str(elem->GetText()));
            model.blendFunc = { MaterialModel::blendFuncToString(vec_str.x()),
                                MaterialModel::blendFuncToString(vec_str.y()) };
        }
        else if(elem->ValueStr() == "writeColor")
        {
            auto vec_str = toStrVec<4>(str(elem->GetText()));
            model.writeColor = { StringUtils(vec_str.x()).toBool(),
                                 StringUtils(vec_str.y()).toBool(),
                                 StringUtils(vec_str.z()).toBool(),
                                 StringUtils(vec_str.w()).toBool() };
        }
        else if(elem->ValueStr() == "color")
            model.color = toVec<4>(str(elem->GetText()));
        else if(elem->ValueStr() == "material")
            model.material = toVec<4>(str(elem->GetText()));

        else if(elem->ValueStr() == "optional")
        {
            model.optional = true;
            parseOptionalMaterial(elem, model);
        }

        elem=elem->NextSiblingElement();
    }
}

void ResourceManager::parseMaterialPass(TiXmlElement* elem, MaterialPassModel& model)
{
    elem=elem->FirstChildElement();
    while(elem)
    {
        if(elem->ValueStr() == "Material")
        {
            std::string name = str(elem->Attribute("name"));
            if(!name.empty())
            {
                if(elem->FirstChildElement())
                {
                    MaterialModel mat;
                    parseMaterial(elem, mat);
                    _materialModels[name] = mat;
                }
                model.pass.push_back(name);
            }
        }
        elem=elem->NextSiblingElement();
    }
}

void ResourceManager::parseOptionalMaterial(TiXmlElement* elem, MaterialModel& model)
{
    elem=elem->FirstChildElement();

    while(elem)
    {
        if(elem->ValueStr() == "scissor")
            model.scissor = StringUtils(str(elem->GetText())).toBool();
        else if(elem->ValueStr() == "logicColor")
            model.logicColor = StringUtils(str(elem->GetText())).toBool();
        else if(elem->ValueStr() == "opcode")
            model.opcode = StringUtils(str(elem->GetText())).toInt();
        else if(elem->ValueStr() == "scissorCoord")
        {
            auto vec_str = toStrVec<2>(str(elem->GetText()));
            model.scissorCoord = { (size_t)StringUtils(vec_str.x()).toInt(), (size_t)StringUtils(vec_str.y()).toInt() };
        }
        else if(elem->ValueStr() == "scissorSize")
        {
            auto vec_str = toStrVec<2>(str(elem->GetText()));
            model.scissorSize = { (size_t)StringUtils(vec_str.x()).toInt(), (size_t)StringUtils(vec_str.y()).toInt() };
        }
        elem=elem->NextSiblingElement();
    }
}

renderer::MeshBuffers* ResourceManager::async_getMesh(const std::string& str)
{
    if(!_pool)
        return nullptr;

    renderer::MeshBuffers* m = _meshManager.get(str);
    if(!m)
    {
        m=new renderer::MeshBuffers(nullptr, nullptr);
        m->_isComplete=false;
        _curMeshLoading.push_back({m, _meshManager.async_loadMesh(_meshDataContext[str], *_pool)});
        _meshManager.addMesh(str, m);
    }
    return m;
}

void ResourceManager::flush(float time)
{
    if(!_pool)
        return;

    boost::container::vector<std::pair<renderer::MeshBuffers*, boost::unique_future<renderer::MeshBuffers*>>> cpy;
    for(size_t i=0 ; i<_curMeshLoading.size() ; ++i)
    {
        if(_curMeshLoading[i].second.get_state() == boost::future_state::state::ready)
        {
            _curMeshLoading[i].first->swap(_curMeshLoading[i].second.get());
            _curMeshLoading[i].first->_isComplete=true;
            delete _curMeshLoading[i].second.get();

            if(_curMeshLoading[i].first)
            {
                if(_curMeshLoading[i].first->vertexBuffer() && _curMeshLoading[i].first->indexBuffer())
                {
                    _curMeshLoading[i].first->vertexBuffer()->uploadOnGpu();
                    _curMeshLoading[i].first->indexBuffer()->uploadOnGpu();
                    _meshManager.addUsedMeshGpuMemory(_curMeshLoading[i].first->indexBuffer()->size()*4);
                    _meshManager.addUsedMeshGpuMemory(_curMeshLoading[i].first->vertexBuffer()->size()*
                                                  _curMeshLoading[i].first->vertexBuffer()->formatSize()*4);
                }
            }
        }
        else
        {
            cpy.push_back(std::pair<renderer::MeshBuffers*, boost::unique_future<renderer::MeshBuffers*>>
                          (_curMeshLoading[i].first, boost::move(_curMeshLoading[i].second)));
        }
    }

    _curMeshLoading = boost::move(cpy);

    _materials.updateIndex();
    _textureManager.flush(*_pool);
}

std::string ResourceManager::printUsedGpuMemory() const
{
    std::string str="Texture Memory:";

    float mb = _textureManager.onGpuTextureMemory()/(float)1000000;
    mb = ((int)(mb*100))/100.0;

    str += StringUtils(mb).str() + " MB (" +
           StringUtils(_textureManager.onGpuTextureMemory()).str() + " bytes)\n";
    str += "Mesh Memory:";

    mb = _meshManager.onGpuMeshMemory()/(float)1000000;
    mb = ((int)(mb*100))/100.0;
    str += StringUtils(mb).str() + " MB (" +
           StringUtils(_meshManager.onGpuMeshMemory()).str() + " bytes)\n";
    return str;
}


}
}
