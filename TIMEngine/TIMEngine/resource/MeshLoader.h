#ifndef MESHLOADER_H
#define MESHLOADER_H

#include "core/core.h"
#include "renderer/renderer.h"
#include <fstream>

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

    class MeshLoader
    {
    public:
        static renderer::MeshBuffers* importObj(const std::string&, bool);
        static renderer::MeshBuffers* createMeshBuffers(const float*, size_t, const uint*, size_t, renderer::VertexFormat);

        static renderer::MeshBuffers* importMeshTim(const std::string&);
        static void exportTim(renderer::MeshBuffers*, const std::string&);

        static renderer::MeshBuffers* genGrid(const vec2&, uint, float virtualZ=0);

    private:
        MeshLoader();
        virtual ~MeshLoader();

        struct ObjBuffer
        {
            vec3 *vbuffer=nullptr, *nbuffer=nullptr;
            vec2 *tbuffer=nullptr;
            uivec3 *ibuffer=nullptr;
            size_t nbVertex=0, nbNormal=0, nbTexCoord=0, nbIndex=0;
            renderer::VertexFormat format=renderer::VertexFormat::VN;

            void free()
            {
                delete[] vbuffer; vbuffer=nullptr;
                delete[] nbuffer; nbuffer=nullptr;
                delete[] tbuffer; tbuffer=nullptr;
                delete[] ibuffer; ibuffer=nullptr;
                nbVertex=0; nbNormal=0; nbTexCoord=0; nbIndex=0;
            }
        };

        static bool loadObjData(const std::string&, ObjBuffer&);
        static size_t computeObjVertexMap(ObjBuffer&, renderer::IndexBuffer*, boost::container::map<renderer::VNC_Vertex, size_t>&);
        static uivec3 parseObjIndex(const std::string&, bool&, int);

        static void writeTim(std::ostream&, renderer::MeshBuffers*);
        static renderer::MeshBuffers* readMeshTim(std::istream&);

        template<class T>
        static void write(std::ostream&, const T&);

        template<class T>
        static void read(std::istream&, T&);
    };

    template<class T>
    void MeshLoader::write(std::ostream& stream, const T& data)
    {
        stream.write(reinterpret_cast<const char*>(&data), sizeof(data));
    }

    template<class T>
    void MeshLoader::read(std::istream& stream, T& data)
    {
        stream.read(reinterpret_cast<char*>(&data), sizeof(data));
    }

}
}
#include "MemoryLoggerOff.h"

#endif // MESHLOADER_H
