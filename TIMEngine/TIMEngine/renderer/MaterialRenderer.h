#ifndef MATERIALRENDERER_H
#define MATERIALRENDERER_H

#include "AbstractRenderer.h"
#include "MaterialInstance.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

    class MaterialRenderer : public AbstractRenderer
    {
    public:
        MaterialRenderer(const uivec2&);
        MaterialRenderer(const boost::container::vector<Texture*>&, Texture* depthBuf=nullptr);
        virtual ~MaterialRenderer();

        void setMatrix(const mat4&, const mat4&);
        void setViewMatrix(const mat4&);
        void setCliPlane(const boost::container::vector<vec4>&);
        void setTime(float, float);

        const vec3& cameraPos() const;

        void draw(Material*) const;

        template <class T> // T an iterator of MaterialInstance
        size_t render(T itBegin, T itEnd, size_t pass=0);

    private:
        mat4 _view, _proj;
        mat4 _projView, _invView, _invViewProj;
        vec3 _cameraPos;
        float _frameTime, _time;
        boost::container::vector<vec4> _clipPlane;

        static const size_t INSTANCING = 128;
        mat4 _matrixPool[INSTANCING];

        void drawPool(size_t, Material*);
    };

    inline void MaterialRenderer::setMatrix(const mat4& proj, const mat4& view)
    {
        _proj=proj;
        _view=view;
        _projView=proj*view;
        _invView=view.inverted();
        _invViewProj=_projView.inverted();
        _cameraPos = _view.inverted().translation().down<1>();
    }

    inline void MaterialRenderer::setViewMatrix(const mat4& view)
    {
        setMatrix(_proj, view);
    }

    inline void MaterialRenderer::setCliPlane(const boost::container::vector<vec4>& cp)
    {
        _clipPlane=cp;
    }

    inline const vec3& MaterialRenderer::cameraPos() const
    {
        return _cameraPos;
    }

    inline void MaterialRenderer::setTime(float ftime, float time)
    {
        _frameTime=ftime;
        _time = time;
    }

    template <class T>
    size_t MaterialRenderer::render(T itBegin, T itEnd, size_t pass)
    {
        if(itBegin == itEnd)
            return 0;

        size_t nbDraw=0, indexPool=0;
        Material* m=nullptr;
        Material* lastMat=(*itBegin)->material(pass);

        while(itBegin != itEnd)
        {
            m=(*itBegin)->material(pass);
            if(m != lastMat || indexPool == INSTANCING)
            {
                if(lastMat && lastMat->bind())
                {
                    drawPool(indexPool, lastMat);
                    nbDraw++;
                }
                indexPool=0;
            }

            if((*itBegin)->matrix())
                _matrixPool[indexPool] = *((*itBegin)->matrix());
            else
                _matrixPool[indexPool] = mat4::IDENTITY();

            lastMat=m;
            indexPool++;
            itBegin++;
        }

        if(lastMat && lastMat->bind())
        {
            drawPool(indexPool, lastMat);
            nbDraw++;
        }
        return nbDraw;
    }
}
}
#include "MemoryLoggerOff.h"

#endif // MATERIALRENDERER_H
