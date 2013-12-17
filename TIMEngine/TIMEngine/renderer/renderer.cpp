#include "renderer.h"

#include "GL/glew.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

bool init()
{
    GLenum glewerr;
    if((glewerr=glewInit()) != GLEW_OK)
    {
        err("glewinit() failed : ")<<glewGetErrorString(glewerr)<<"\n";
        return false;
    }

    out("Initializing nothing on ")<<glGetString(GL_VENDOR)<<" "<<glGetString(GL_RENDERER)<<" using OpenGL "<<glGetString(GL_VERSION)<<"\n";
    err("Initializing nothing on ")<<glGetString(GL_VENDOR)<<" "<<glGetString(GL_RENDERER)<<" using OpenGL "<<glGetString(GL_VERSION)<<"\n";

    openGL.getHardwardProperties();
}

bool close()
{
    GLState::freeInstance();
}

}
}
