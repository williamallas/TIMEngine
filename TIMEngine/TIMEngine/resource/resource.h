#ifndef RESOURCE_H_INCLUDED
#define RESOURCE_H_INCLUDED

#include "core/core.h"
#include "MetaShader.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "ResourceManager.h"
#include "ResourceSceneManager.h"
#include "Image.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{
    extern const char* vShaderSrcGeometryPass;
    extern const char* pShaderSrcGeometryPass;

    extern const char* vTerrainShaderSrcGeometryPass;
    extern const char* pTerrainShaderSrcGeometryPass;

    extern const unsigned char textureTest_4x4_1[16];
    extern const unsigned char textureTest_4x4_3[48];

    extern const float quad_mesh_vertex_4[20];
    extern const uint quad_mesh_index_4[4];
}
}
#include "MemoryLoggerOff.h"

#endif // RESOURCE_H_INCLUDED
