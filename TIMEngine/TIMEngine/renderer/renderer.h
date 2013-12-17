#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include "core.h"

#include "GLState.h"
#include "MeshBuffers.h"
#include "MetaShader.h"
#include "Shader.h"

namespace tim
{
    using namespace core;
namespace renderer
{
    bool init();
    bool close();
}
}

#endif // RENDERER_H_INCLUDED
