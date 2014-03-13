#include "MeshLoader.h"
#include "MemoryLoggerOn.h"

#include <fstream>

namespace tim
{
    using namespace core;
namespace resource
{

MeshLoader::MeshLoader() {}

MeshLoader::~MeshLoader() {}

renderer::MeshBuffers* MeshLoader::importObj(const std::string& file, bool tangent)
{
    ObjBuffer buf;
    if(!loadObjData(file, buf))
        return nullptr;

    boost::container::map<renderer::VNC_Vertex, size_t> mapIndex;
    renderer::IndexBuffer* ib = new renderer::IndexBuffer();

    size_t nbVertex = computeObjVertexMap(buf, ib, mapIndex);

    if(buf.nbNormal && buf.nbTexCoord)
    {
        if(!tangent)
        {
            renderer::VertexBuffer* vb = new renderer::VertexBuffer();
            vb->createBuffer(renderer::VertexFormat::VNC, nbVertex);
            renderer::VNC_Vertex* vbuffer = reinterpret_cast<renderer::VNC_Vertex*>(vb->data());
            for(auto it : mapIndex)
            {
                vbuffer[it.second] = it.first;
            }
            buf.free();
            return new renderer::MeshBuffers({vb}, ib);
        }
        else
        {
            renderer::VertexBuffer* vb = new renderer::VertexBuffer();
            vb->createBuffer(renderer::VertexFormat::VNCT, nbVertex);
            renderer::VNCT_Vertex* vbuffer = reinterpret_cast<renderer::VNCT_Vertex*>(vb->data());
            for(auto it : mapIndex)
            {
                vbuffer[it.second] = {it.first.v, it.first.n, it.first.c, vec3()};
            }
            buf.free();
            return new renderer::MeshBuffers({vb}, ib);
        }
    }
    else if(buf.nbNormal)
    {
        renderer::VertexBuffer* vb = new renderer::VertexBuffer();
        vb->createBuffer(renderer::VertexFormat::VN, nbVertex);
        renderer::VN_Vertex* vbuffer = reinterpret_cast<renderer::VN_Vertex*>(vb->data());
        for(auto it : mapIndex)
        {
            vbuffer[it.second] = {it.first.v,it.first.n};
        }
        buf.free();
        return new renderer::MeshBuffers(vb, ib);
    }
    else if(buf.nbTexCoord)
    {
        renderer::VertexBuffer* vb = new renderer::VertexBuffer();
        vb->createBuffer(renderer::VertexFormat::VC, nbVertex);
        renderer::VC_Vertex* vbuffer = reinterpret_cast<renderer::VC_Vertex*>(vb->data());
        for(auto it : mapIndex)
        {
            vbuffer[it.second] = {it.first.v,it.first.c};
        }
        buf.free();
        return new renderer::MeshBuffers({vb}, ib);
    }
    else
    {
        renderer::VertexBuffer* vb = new renderer::VertexBuffer();
        vb->createBuffer(renderer::VertexFormat::V, nbVertex);
        vec3* vbuffer = reinterpret_cast<vec3*>(vb->data());
        for(auto it : mapIndex)
        {
            vbuffer[it.second] = it.first.v;
        }
        buf.free();
        return new renderer::MeshBuffers({vb}, ib);
    }
}

renderer::MeshBuffers* MeshLoader::createMeshBuffers(const float* vdata, size_t nbv,
                                                     const unsigned int* idata, size_t nbi,
                                                     renderer::VertexFormat format)
{
    size_t size;
    switch(format)
    {
        case renderer::VertexFormat::V: size = nbv*3; break;
        case renderer::VertexFormat::VC: size = nbv*5; break;
        case renderer::VertexFormat::VN: size = nbv*6; break;
        case renderer::VertexFormat::VNC: size = nbv*8; break;
        case renderer::VertexFormat::VNCT: size = nbv*11; break;
    }

    renderer::VertexBuffer* vb = new renderer::VertexBuffer();
    vb->createBuffer(format, nbv);
    for(size_t i=0 ; i<size ; i++)
        vb->data()[i]=vdata[i];

    renderer::IndexBuffer* ib = new renderer::IndexBuffer();

    ib->createBuffer(nbi);
    for(size_t i=0 ; i<nbi ; i++)
        ib->data()[i]=idata[i];

    return new renderer::MeshBuffers({vb}, ib);
}

bool MeshLoader::loadObjData(const std::string& file, ObjBuffer& buffer)
{
    {
        std::ifstream readNbV(file);
        buffer.nbVertex=0;
        buffer.nbNormal=0;
        buffer.nbTexCoord=0;
        buffer.nbIndex=0;
        std::string s;
        while(readNbV.good())
        {
            readNbV >> s;
            if(s=="v")
                buffer.nbVertex++;
            else if(s=="vn")
                buffer.nbNormal++;
            else if(s=="vt")
                buffer.nbTexCoord++;
            else if(s=="f")
                buffer.nbIndex+=3;
        }

        if(!buffer.nbVertex || !buffer.nbIndex)
            return false;
    }

    int nbSlash=2;
    if(buffer.nbNormal == 0 && buffer.nbTexCoord == 0)
        nbSlash=0;
    else if(buffer.nbNormal == 0)
        nbSlash=1;

    buffer.vbuffer = new vec3[buffer.nbVertex];
    buffer.ibuffer = new uivec3[buffer.nbIndex];

    if(buffer.nbNormal)
        buffer.nbuffer = new vec3[buffer.nbNormal];

    if(buffer.nbTexCoord)
        buffer.tbuffer = new vec2[buffer.nbTexCoord];

    std::ifstream sfile(file);
    std::string str;

    size_t vindex=0, nindex=0, tindex=0, iindex=0;

    while(sfile.good())
    {
        sfile >> str;

        if(str=="v" || str=="vn")
        {
            std::string xyz[3];
            vec3 v;
            for(size_t i=0 ; i<3 ; i++)
            {
                sfile >> xyz[i];
                if(!StringUtils(xyz[i]).isNumber())
                {

                    buffer.free();
                    return false;
                }
                else v[i] = StringUtils(xyz[i]).toFloat();
            }

            if(str=="v")
            {
                buffer.vbuffer[vindex] = v;
                vindex++;
            }
            else
            {
                buffer.nbuffer[nindex] = v;
                nindex++;
            }
        }
        else if(str=="vt")
        {
            std::string str;
            for(size_t i=0 ; i<2 ; i++)
            {
                sfile >> str;
                if(!StringUtils(str).isNumber())
                {
                    buffer.free();
                    return false;
                }
                else
                    buffer.tbuffer[tindex][i] = StringUtils(str).toFloat();
            }
            tindex++;
        }
        else if(str=="f")
        {
            for(size_t i=0 ; i<3 ; i++)
            {
                sfile >> str;
                bool b=true;
                buffer.ibuffer[iindex] = parseObjIndex(str, b, nbSlash);
                if(!b || buffer.ibuffer[iindex].x()>buffer.nbVertex || (buffer.ibuffer[iindex].x()==0 && buffer.nbVertex)
                      || buffer.ibuffer[iindex].z()>buffer.nbNormal || (buffer.ibuffer[iindex].z()==0 && buffer.nbNormal)
                      || buffer.ibuffer[iindex].y()>buffer.nbTexCoord || (buffer.ibuffer[iindex].y()==0 && buffer.nbTexCoord))
                {
                    buffer.free();
                    return false;
                }
                iindex++;
            }
        }
    }

    if(iindex != buffer.nbIndex || nindex != buffer.nbNormal ||
       vindex != buffer.nbVertex || tindex != buffer.nbTexCoord)
       {
           buffer.free();
           return false;
       }


    return true;
}

size_t MeshLoader::computeObjVertexMap(ObjBuffer& buf, renderer::IndexBuffer* ib,
                                       boost::container::map<renderer::VNC_Vertex, size_t>& mapIndex)
{
    size_t curIndex=0;
    ib->createBuffer(buf.nbIndex);

    renderer::VNC_Vertex vnc;
    for(size_t i=0 ; i<buf.nbIndex ; i++)
    {
        vnc.v = buf.vbuffer[buf.ibuffer[i].x()-1];
        vnc.n=vec3(); vnc.c=vec2();
        if(buf.nbNormal)
            vnc.n = buf.nbuffer[buf.ibuffer[i].z()-1];
        if(buf.nbTexCoord)
            vnc.c = buf.tbuffer[buf.ibuffer[i].y()-1];

        auto it = mapIndex.find(vnc);
        if(it == mapIndex.end())
        {
            mapIndex[vnc] = curIndex;
            ib->data()[i] = curIndex;
            curIndex++;
        }
        else
        {
            ib->data()[i] = it->second;
        }
    }

    return curIndex;
}

uivec3 MeshLoader::parseObjIndex(const std::string& str, bool& ok, int nbSlash)
{
    if(std::count(str.begin(), str.end(), '/') != nbSlash)
    {
        ok=false;
        return uivec3();
    }

    uivec3 res;

    std::string buf;
    size_t index=0;

    for(size_t i=0 ; i<3 ; i++)
    {
        while(index < str.size() && str[index] != '/')
        {
            buf+=str[index];
            index++;
        }
        if(!buf.empty())
        {
            if(!StringUtils(buf).isNumber())
            {
                ok=false;
                return uivec3();
            }
            res[i] = StringUtils(buf).toInt();
        }
        buf.clear(); index++;
    }
    return res;
}

}
}
