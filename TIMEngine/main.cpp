
#include "SDL.h"
#include "SDLTimer.h"

#include <boost/atomic.hpp>
#include <boost/timer/timer.hpp>
#include "interface/SceneManager.h"
#include "renderer/renderer.h"
#include "resource/resource.h"

#include "SDLInputManager.h"
#include "SDLTextureLoader.h"
#include "scene/SceneGrid.h"

using namespace tim::core;
using namespace tim::resource;
using namespace tim::renderer;
using namespace tim::interface;

#include "core/MemoryLoggerOn.h"

SDL_Window* pWindow;
SDL_GLContext contexteOpenGL;
void initContextSDL();
void delContextSDL();

bool comp(Material*m1, Material*m2){ return (*m1)<(*m2); }

vec3 polarToCartesian(const vec3& v)
{
    return vec3(cos(v.z())*sin(v.y()), sin(v.y())*sin(v.z()), cos(v.y()))*v.x();
}

class lol
{
    public:
    char l[123];
};

struct Task
{
    bool operator()()
    {
        float x=0;
        for(int i=0 ; i<10000000 ; i++)
            x+=sqrt(i);
        out(x)<<std::endl;
        return true;
    }
};

//struct Task2
//{
//    bool operator()()
//    {
//        boost::this_thread::sleep(boost::posix_time::milliseconds(400));
//        out("yooo\n");
//        return true;
//    }
//};

int main(int argc, char** argv)
{
    tim::core::init();
    {
        initContextSDL();
        tim::renderer::init();

        err("Hardward specs:\n")<<openGL.strHardward()<<std::endl;

        ThreadPool pool(2);
        ResourceManager resManager(&pool, (new tim::SDLTextureLoader));

        resManager.loadXmlDataInformation("datapath.xml");

        MeshBuffers* quadMesh = MeshLoader::createMeshBuffers(quad_mesh_vertex_4, 4, quad_mesh_index_4, 4, VertexFormat::VC);

        quadMesh->setPrimitive(VertexMode::QUADS);
        quadMesh->vertexBuffer()->uploadOnGpu(true, false);
        quadMesh->indexBuffer()->uploadOnGpu(true, false);
        resManager.addMesh("quad", quadMesh);

        Material matTexture;
        matTexture.setShader(resManager.getShader("drawQuad"));
        matTexture.setDepthTest(false);
        matTexture.setWriteDepth(false);
        matTexture.setNbTexture(1);
        matTexture.setMesh(resManager.getMesh("quad"));
        matTexture.setCullFace(false);

        MaterialRenderer matRenderer({},nullptr);
        vec3 posView={0,0,0}, dirView={1,0,0};
        matRenderer.setMatrix(mat4::Projection(70,1600/900.0,0.1,300),
                              mat4::View(posView,dirView,{0,0,1}));

        SceneManager scene;
        boost::container::vector<RenderableObject*> allObject;
        boost::container::vector<RenderableObject*> mObject;

        const float SIZE_CUBE=120;
        for(int i=0 ; i<10000 ; i++)
        {
                allObject.push_back(scene.addRenderableObject(
                          mat4::Translation({Rand::frand({-SIZE_CUBE,SIZE_CUBE}),
                                             Rand::frand({-SIZE_CUBE,SIZE_CUBE}),
                                             Rand::frand({-SIZE_CUBE,SIZE_CUBE})}))
                                    ->addElement(resManager.getMaterialPass("Sphere")));
            if(i<3000)
            {
                mObject.push_back(allObject[i]);
            }
        }

        allObject.push_back(scene.addRenderableObject(mat4::Translation({0,0,0}))
                                    ->addElement(resManager.async_getMaterialPass("Terrain")));

        bool continu=true;
        tim::SDLInputManager input;

        float time=0, totalTime=0;
        tim::SDLTimer frameTimer;

        float angleX=0, angleZ=30, length=30;

        boost::container::vector<SceneObject*> drawObj;
        Camera cam;
        cam.fov=70;
        cam.ratio=1600/900.0;
        cam.clipDist={0.1,4000};
        cam.up={0,0,1};

        Texture::Parameter colorBufParam;
        colorBufParam.intern = Texture::Parameter::InternFormat::RGBA;
        boost::container::vector<Texture*> fboBuf(3);
        fboBuf[0] = Texture::genTexture2D({1600,900}, (const unsigned char*)nullptr, 0, colorBufParam);
        fboBuf[1] = Texture::genTexture2D({1600,900}, (const unsigned char*)nullptr, 0, colorBufParam);
        fboBuf[2] = Texture::genTexture2D({1600,900}, (const unsigned char*)nullptr, 0, colorBufParam);

        colorBufParam.intern = Texture::Parameter::InternFormat::DEPTHCOMPONENT;
        matTexture.setTexture(fboBuf[0],1);

        FrameBuffer fbo(fboBuf, Texture::genTexture2D({1600,900}, (const unsigned char*)nullptr, 0, colorBufParam));

        while(continu)
        {
            frameTimer.reset();

            input.getEvent();
            if(input.quitState() || input.keyState(SDLK_ESCAPE).pressed)
                continu=false;

            if(!input.keyState(SDLK_q).pressed)
            {
                drawObj.clear();
                tim::SDLTimer timer;
                scene.frustumTest(cam, drawObj, SceneObject::ObjectTypeMask::MASK_RENDERABLE, true);
                out("frustum:")<<timer.ellapsed()<<"ms"<<std::endl;
            }

            {
                tim::SDLTimer timer;
                openGL.bindFrameBuffer(0);
                openGL.clearColor({0.1,0.1,0.1,0.1});
                openGL.clearDepth();
                out("clear buffer:")<<timer.ellapsed()<<" ms\n";
            }
            {
                tim::SDLTimer timer;
                SceneManager::drawObjects(drawObj, MaterialPass::COLOR_PASS, &matRenderer);
                out("draw:")<<timer.ellapsed()<<" ms\n";
            }

            if(input.keyState(SDLK_e).pressed)
                matRenderer.draw(&matTexture);

            {
                tim::SDLTimer timer;
                SDL_GL_SwapWindow(pWindow);
                out("swapbuffer:")<<timer.ellapsed()<<" ms\n";
            }

            angleX += -50*time*input.mouseRel().x();
            angleZ += -50*time*input.mouseRel().y();
            angleZ = std::min(89.0f, std::max(-89.0f, angleZ));
            vec3 dirvec = {cosf(angleZ*PI/180)*cosf(angleX*PI/180), cosf(angleZ*PI/180)*sinf(angleX*PI/180), sinf(angleZ*PI/180)};

            if(input.keyState(SDLK_w).pressed)
                posView+=dirvec*time*30;
            else if(input.keyState(SDLK_s).pressed)
                posView-=dirvec*time*30;

            if(input.keyState(SDLK_a).pressed)
                posView-=dirvec.cross({0,0,1}).normalized()*time*30;
            else if(input.keyState(SDLK_d).pressed)
                posView+=dirvec.cross({0,0,1}).normalized()*time*30;

            length+=input.mouseWheel().y()*time*1000;

            cam.pos=posView;
            cam.dir=posView+dirvec;
            matRenderer.setMatrix(mat4::Projection(cam.fov,cam.ratio,cam.clipDist.x(),cam.clipDist.y()),
                                  mat4::View(cam.pos,cam.dir,cam.up));

            {
                tim::SDLTimer timer;
                for(size_t i=0 ; i<mObject.size() ; i++)
                {
                    auto m=mObject[i]->matrix();
                    m.translate({0,0,sin(totalTime*10)*time*10});
                    mObject[i]->setMatrixNoScale(m);
                    mObject[i]->move();
                }
                out("move:")<<timer.ellapsed()<<"ms, nb:"<<mObject.size()<<std::endl;
            }

            if(input.keyState(SDLK_r).firstPress)
            {
                scene.addRenderableObject(mat4::Translation(posView))->addElement(resManager.async_getMaterialPass("Tank"));
            }

            if(input.keyState(SDLK_c).firstPress && allObject.size()>1000)
            {
                for(size_t i=0 ; i<1000 ; i++)
                {
                    allObject[allObject.size()-1]->remove();
                    allObject.pop_back();
                    mObject.clear();
                }
            }

            {
                tim::SDLTimer timer;
                resManager.flush(time);
                scene.endScene();
                out("endScene():")<<timer.ellapsed()<<"ms" <<std::endl;
            }

            if(input.keyState(SDLK_m).firstPress)
                err(resManager.printUsedGpuMemory());

            GL_ASSERT();

            {
                float t=frameTimer.ellapsed();
                out("fps:")<<1000.0/t<< "  time(ms):"<<t<<std::endl<<std::endl;
                time = t*0.001;
                totalTime+=t*0.001;
            }

            if(input.keyState(SDLK_p).firstPress)
                system("pause");

        }

        scene.clearScene();

        tim::renderer::close();
        delContextSDL();

    }
    tim::core::quit();

    return 0;
}

void initContextSDL()
{
    /* Initialisation simple */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        out("Échec de l'initialisation de la SDL"); out(SDL_GetError()); out("\n");
        system("pause");
        return;
    }

//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


    pWindow = SDL_CreateWindow("SDL2",SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      1600,
                                      900,
                                      SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    contexteOpenGL = SDL_GL_CreateContext(pWindow);

    //SDL_ShowCursor(SDL_DISABLE);
    //SDL_SetWindowGrab(pWindow, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    if(contexteOpenGL == 0)
    {
        out(SDL_GetError());out("\n");
        system("pause");
        return;
    }
    openGL.setViewPort({0,0},{1600,900});
}

void delContextSDL()
{
    SDL_GL_DeleteContext(contexteOpenGL);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}
