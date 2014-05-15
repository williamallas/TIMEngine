#ifndef SCENEGRID_H
#define SCENEGRID_H

#ifndef USE_OCTREE

#include "Transformable.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace scene
{
    class SceneManager;

    class SceneGrid
    {
        friend SceneManager;
    public:
        class Cell : public TransformableContainer
        {
        public:
            void setBox(const Box& b) { _box=b; }
            void addTransformable(Transformable* t)
            {
                _container.push_back(t);
                t->cells().push_back({(void*)this, _container.size()-1});
            }

            void remove(size_t indexInCell)
            {
                std::swap(_container[indexInCell], _container[_container.size()-1]);

                for(size_t i=0 ; i<_container[indexInCell]->cells().size() ; ++i)
                    if(_container[indexInCell]->cells()[i].container == this)
                    {
                        _container[indexInCell]->cells()[i].index = indexInCell;
                        break;
                    }

                _container.pop_back();
            }

            void setParent(SceneGrid* grid) { _parent=grid; }
            SceneGrid* parent() const { return _parent; }

        private:
            SceneGrid* _parent;
        };

        static const size_t NB_CELL_X=32, NB_CELL_Y=32, NB_CELL_Z=32;
        static const size_t NB_CELLS = NB_CELL_X*NB_CELL_Y*NB_CELL_Z;

        static const float CELL_SIZE_X, CELL_SIZE_Y, CELL_SIZE_Z;
        static const float GRID_SIZE;

        SceneGrid(const vec3&, SceneManager*);
        virtual ~SceneGrid();

        std::string str() const;
        const Box& box() const;

        static size_t mapIndex(const uivec3&);
        uivec3 mapIndex(const vec3&) const;
        static uivec3 mapIndex(size_t);

        Intersection insert(Transformable*);
        void inject(Transformable*);
        void remove(size_t);
        bool empty() const;
        size_t nbObject() const;

        template <class T>
        Intersection take(T& taker, size_t depthLimit) const;

        template <class T>
        Intersection take_brute(T& taker) const;

        template <class T>
        Intersection takeGrid(T& taker, size_t depthLimit) const;

        template <class T>
        Intersection takeOutGrid(T& taker, size_t depthLimit) const;

        template <class T>
        Intersection takeInGrid(T& taker, size_t depthLimit) const;

    private:

        Cell* _grid;
        Box _box;
        vec3 _minBox;
        SceneManager* _scene;
        boost::container::vector<Transformable*> _allObject;

        void insertBig(Transformable*);
        void insertSmall(Transformable*);

        template <class T>
        Intersection takeRec(T& taker, size_t depthLimit, const Vector2<uivec3>& minmax, size_t curDepth) const;

        template <class T>
        Intersection takeGridRec(T& taker, size_t depthLimit, const Vector2<uivec3>& minmax, size_t curDepth) const;

        template <class T>
        Intersection takeOutGridRec(T& taker, size_t depthLimit, const Vector2<uivec3>& minmax, size_t curDepth) const;

        template <class T>
        Intersection takeInGridRec(T& taker, size_t depthLimit, const Vector2<uivec3>& minmax, size_t curDepth) const;
    };

    inline const Box& SceneGrid::box() const
    {
        return _box;
    }

    inline bool SceneGrid::empty() const
    {
        return _allObject.empty();
    }

    inline size_t SceneGrid::nbObject() const
    {
        return _allObject.size();
    }

    inline size_t SceneGrid::mapIndex(const uivec3& pos)
    {
        return pos.x()*NB_CELL_Y*NB_CELL_Z + pos.y()*NB_CELL_Z + pos.z();
    }

    inline uivec3 SceneGrid::mapIndex(const vec3& pos) const
    {
        static const float INV_GRID_SIZE=1.0/GRID_SIZE;
        vec3 v=((pos-_minBox)*(vec3(NB_CELL_X, NB_CELL_Y, NB_CELL_Z)*INV_GRID_SIZE));
        return uivec3(v.x(), v.y(), v.z());
    }

    inline uivec3 SceneGrid::mapIndex(size_t index)
    {
        uivec3 res;
        res.z() = index%NB_CELL_Z;
        index = (index-res.z())/NB_CELL_Z;
        res.y() = index%NB_CELL_Y;
        res.x() = (index-res.y())/NB_CELL_Y;
        return res;
    }

    template <class T>
    Intersection SceneGrid::take(T& taker, size_t depthLimit) const
    {
        return takeRec(taker, depthLimit, Vector2<uivec3>({0,0,0},{NB_CELL_X, NB_CELL_Y, NB_CELL_Z}), 0);
    }

    template <class T>
    Intersection SceneGrid::takeGrid(T& taker, size_t depthLimit) const
    {
        return takeGridRec(taker, depthLimit, Vector2<uivec3>({0,0,0},{NB_CELL_X, NB_CELL_Y, NB_CELL_Z}), 0);
    }

    template <class T>
    Intersection SceneGrid::takeOutGrid(T& taker, size_t depthLimit) const
    {
        return takeOutGridRec(taker, depthLimit, Vector2<uivec3>({0,0,0},{NB_CELL_X, NB_CELL_Y, NB_CELL_Z}), 0);
    }

    template <class T>
    Intersection SceneGrid::takeInGrid(T& taker, size_t depthLimit) const
    {
        return takeInGridRec(taker, depthLimit, Vector2<uivec3>({0,0,0},{NB_CELL_X, NB_CELL_Y, NB_CELL_Z}), 0);
    }

    template <class T>
    Intersection SceneGrid::take_brute(T& taker) const
    {
        Box box(_minBox,
                {NB_CELL_X*CELL_SIZE_X+_minBox.x(), NB_CELL_Y*CELL_SIZE_Y+_minBox.y(), NB_CELL_Z*CELL_SIZE_Z+_minBox.z()});

        Intersection inter = taker.collide(box);

        if(inter != OUTSIDE)
        {
            for(Transformable* t : _allObject)
            {
                if(taker.filter(t))
                    taker.take(t);
            }
        }

        return inter;
    }

    template <class T>
    Intersection SceneGrid::takeRec(T& taker, size_t depthLimit, const Vector2<uivec3>& minmax, size_t curDepth) const
    {
        Box box({minmax[0].x()*CELL_SIZE_X+_minBox.x(), minmax[0].y()*CELL_SIZE_Y+_minBox.y(), minmax[0].z()*CELL_SIZE_Z+_minBox.z()},
                {minmax[1].x()*CELL_SIZE_X+_minBox.x(), minmax[1].y()*CELL_SIZE_Y+_minBox.y(), minmax[1].z()*CELL_SIZE_Z+_minBox.z()});

        Intersection inter = taker.collide(box);

        if(inter == INSIDE)
        {
            for(size_t i=minmax[0].x() ; i<minmax[1].x() ; ++i)
                for(size_t j=minmax[0].y() ; j<minmax[1].y() ; ++j)
                    for(size_t k=minmax[0].z() ; k<minmax[1].z() ; ++k)
            {
                size_t index=mapIndex(uivec3(i,j,k));
                for(Transformable* t : _grid[index].container())
                {
                    if(taker.filter(t))
                        taker.takeSure(t);
                }
            }

        }
        else if(curDepth >= depthLimit || minmax.x()+1 == minmax.y())
        {
            for(size_t i=minmax[0].x() ; i<minmax[1].x() ; ++i)
                for(size_t j=minmax[0].y() ; j<minmax[1].y() ; ++j)
                    for(size_t k=minmax[0].z() ; k<minmax[1].z() ; ++k)
            {
                size_t index=mapIndex(uivec3(i,j,k));
                for(Transformable* t : _grid[index].container())
                {
                    if(taker.filter(t))
                        taker.take(t);
                }
            }
        }
        else if(inter == INTERSECT)
        {
            uivec3 middle = (minmax[1]+minmax[0])/2;

            takeRec(taker, depthLimit, Vector2<uivec3>
                    (minmax[0], middle), curDepth+1);
            takeRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), minmax[0].y(), minmax[0].z()}, {minmax[1].x(), middle.y(), middle.z()}), curDepth+1);
            takeRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), middle.y(), minmax[0].z()}, {middle.x(), minmax[1].y(), middle.z()}), curDepth+1);
            takeRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), minmax[0].y(), middle.z()}, {middle.x(), middle.y(), minmax[1].z()}), curDepth+1);

            takeRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), middle.y(), middle.z()}, {middle.x(), minmax[1].y(), minmax[1].z()}), curDepth+1);
            takeRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), middle.y(), minmax[0].z()}, {minmax[1].x(), minmax[1].y(), middle.z()}), curDepth+1);
            takeRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), minmax[0].y(), middle.z()}, {minmax[1].x(), middle.y(), minmax[1].z()}), curDepth+1);
            takeRec(taker, depthLimit, Vector2<uivec3>
                    (middle, minmax[1]), curDepth+1);
        }

        return inter;
    }

    template <class T>
    Intersection SceneGrid::takeGridRec(T& taker, size_t depthLimit, const Vector2<uivec3>& minmax, size_t curDepth) const
    {
        Box box({minmax[0].x()*CELL_SIZE_X+_minBox.x(), minmax[0].y()*CELL_SIZE_Y+_minBox.y(), minmax[0].z()*CELL_SIZE_Z+_minBox.z()},
                {minmax[1].x()*CELL_SIZE_X+_minBox.x(), minmax[1].y()*CELL_SIZE_Y+_minBox.y(), minmax[1].z()*CELL_SIZE_Z+_minBox.z()});

        Intersection inter = taker.collide(box);

        if(inter == INSIDE)
        {
            for(size_t i=minmax[0].x() ; i<minmax[1].x() ; ++i)
                for(size_t j=minmax[0].y() ; j<minmax[1].y() ; ++j)
                    for(size_t k=minmax[0].z() ; k<minmax[1].z() ; ++k)
            {
                taker.takeInside(&_grid[mapIndex(uivec3(i,j,k))]);
            }
        }
        else if(inter == OUTSIDE)
        {
            for(size_t i=minmax[0].x() ; i<minmax[1].x() ; ++i)
                for(size_t j=minmax[0].y() ; j<minmax[1].y() ; ++j)
                    for(size_t k=minmax[0].z() ; k<minmax[1].z() ; ++k)
            {
                taker.takeOutside(&_grid[mapIndex(uivec3(i,j,k))]);
            }
        }
        else if(curDepth >= depthLimit || minmax.x()+1 == minmax.y())
        {
            for(size_t i=minmax[0].x() ; i<minmax[1].x() ; ++i)
                for(size_t j=minmax[0].y() ; j<minmax[1].y() ; ++j)
                    for(size_t k=minmax[0].z() ; k<minmax[1].z() ; ++k)
            {
                taker.takeIntersect(&_grid[mapIndex(uivec3(i,j,k))]);
            }
        }
        else
        {
            uivec3 middle = (minmax[1]+minmax[0])/2;

            takeGridRec(taker, depthLimit, Vector2<uivec3>
                    (minmax[0], middle), curDepth+1);
            takeGridRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), minmax[0].y(), minmax[0].z()}, {minmax[1].x(), middle.y(), middle.z()}), curDepth+1);
            takeGridRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), middle.y(), minmax[0].z()}, {middle.x(), minmax[1].y(), middle.z()}), curDepth+1);
            takeGridRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), minmax[0].y(), middle.z()}, {middle.x(), middle.y(), minmax[1].z()}), curDepth+1);

            takeGridRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), middle.y(), middle.z()}, {middle.x(), minmax[1].y(), minmax[1].z()}), curDepth+1);
            takeGridRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), middle.y(), minmax[0].z()}, {minmax[1].x(), minmax[1].y(), middle.z()}), curDepth+1);
            takeGridRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), minmax[0].y(), middle.z()}, {minmax[1].x(), middle.y(), minmax[1].z()}), curDepth+1);
            takeGridRec(taker, depthLimit, Vector2<uivec3>
                    (middle, minmax[1]), curDepth+1);
        }

        return inter;
    }

    template <class T>
    Intersection SceneGrid::takeOutGridRec(T& taker, size_t depthLimit, const Vector2<uivec3>& minmax, size_t curDepth) const
    {
        Box box({minmax[0].x()*CELL_SIZE_X+_minBox.x(), minmax[0].y()*CELL_SIZE_Y+_minBox.y(), minmax[0].z()*CELL_SIZE_Z+_minBox.z()},
                {minmax[1].x()*CELL_SIZE_X+_minBox.x(), minmax[1].y()*CELL_SIZE_Y+_minBox.y(), minmax[1].z()*CELL_SIZE_Z+_minBox.z()});

        Intersection inter = taker.collide(box);

        if(inter == INSIDE)
        {
            return INSIDE;
        }
        else if(inter == OUTSIDE)
        {
            for(size_t i=minmax[0].x() ; i<minmax[1].x() ; ++i)
                for(size_t j=minmax[0].y() ; j<minmax[1].y() ; ++j)
                    for(size_t k=minmax[0].z() ; k<minmax[1].z() ; ++k)
            {
                taker.take(&_grid[mapIndex(uivec3(i,j,k))]);
            }
        }
        else if(curDepth >= depthLimit || minmax.x()+1 == minmax.y())
        {
            for(size_t i=minmax[0].x() ; i<minmax[1].x() ; ++i)
                for(size_t j=minmax[0].y() ; j<minmax[1].y() ; ++j)
                    for(size_t k=minmax[0].z() ; k<minmax[1].z() ; ++k)
            {
                taker.takeIntersect(&_grid[mapIndex(uivec3(i,j,k))]);
            }
        }
        else
        {
            uivec3 middle = (minmax[1]+minmax[0])/2;

            takeOutGridRec(taker, depthLimit, Vector2<uivec3>
                    (minmax[0], middle), curDepth+1);
            takeOutGridRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), minmax[0].y(), minmax[0].z()}, {minmax[1].x(), middle.y(), middle.z()}), curDepth+1);
            takeOutGridRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), middle.y(), minmax[0].z()}, {middle.x(), minmax[1].y(), middle.z()}), curDepth+1);
            takeOutGridRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), minmax[0].y(), middle.z()}, {middle.x(), middle.y(), minmax[1].z()}), curDepth+1);

            takeOutGridRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), middle.y(), middle.z()}, {middle.x(), minmax[1].y(), minmax[1].z()}), curDepth+1);
            takeOutGridRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), middle.y(), minmax[0].z()}, {minmax[1].x(), minmax[1].y(), middle.z()}), curDepth+1);
            takeOutGridRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), minmax[0].y(), middle.z()}, {minmax[1].x(), middle.y(), minmax[1].z()}), curDepth+1);
            takeOutGridRec(taker, depthLimit, Vector2<uivec3>
                    (middle, minmax[1]), curDepth+1);
        }

        return inter;
    }

    template <class T>
    Intersection SceneGrid::takeInGridRec(T& taker, size_t depthLimit, const Vector2<uivec3>& minmax, size_t curDepth) const
    {
        Box box({minmax[0].x()*CELL_SIZE_X+_minBox.x(), minmax[0].y()*CELL_SIZE_Y+_minBox.y(), minmax[0].z()*CELL_SIZE_Z+_minBox.z()},
                {minmax[1].x()*CELL_SIZE_X+_minBox.x(), minmax[1].y()*CELL_SIZE_Y+_minBox.y(), minmax[1].z()*CELL_SIZE_Z+_minBox.z()});

        Intersection inter = taker.collide(box);

        if(inter == INSIDE)
        {
            for(size_t i=minmax[0].x() ; i<minmax[1].x() ; ++i)
                for(size_t j=minmax[0].y() ; j<minmax[1].y() ; ++j)
                    for(size_t k=minmax[0].z() ; k<minmax[1].z() ; ++k)
            {
                taker.take(&_grid[mapIndex(uivec3(i,j,k))]);
            }
        }
        else if(inter == OUTSIDE)
        {
            return OUTSIDE;
        }
        else if(curDepth >= depthLimit || minmax.x()+1 == minmax.y())
        {
            for(size_t i=minmax[0].x() ; i<minmax[1].x() ; ++i)
                for(size_t j=minmax[0].y() ; j<minmax[1].y() ; ++j)
                    for(size_t k=minmax[0].z() ; k<minmax[1].z() ; ++k)
            {
                taker.takeIntersect(&_grid[mapIndex(uivec3(i,j,k))]);
            }
        }
        else
        {
            uivec3 middle = (minmax[1]+minmax[0])/2;

            takeInGridRec(taker, depthLimit, Vector2<uivec3>
                    (minmax[0], middle), curDepth+1);
            takeInGridRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), minmax[0].y(), minmax[0].z()}, {minmax[1].x(), middle.y(), middle.z()}), curDepth+1);
            takeInGridRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), middle.y(), minmax[0].z()}, {middle.x(), minmax[1].y(), middle.z()}), curDepth+1);
            takeInGridRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), minmax[0].y(), middle.z()}, {middle.x(), middle.y(), minmax[1].z()}), curDepth+1);

            takeInGridRec(taker, depthLimit, Vector2<uivec3>
                    ({minmax[0].x(), middle.y(), middle.z()}, {middle.x(), minmax[1].y(), minmax[1].z()}), curDepth+1);
            takeInGridRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), middle.y(), minmax[0].z()}, {minmax[1].x(), minmax[1].y(), middle.z()}), curDepth+1);
            takeInGridRec(taker, depthLimit, Vector2<uivec3>
                    ({middle.x(), minmax[0].y(), middle.z()}, {minmax[1].x(), middle.y(), minmax[1].z()}), curDepth+1);
            takeInGridRec(taker, depthLimit, Vector2<uivec3>
                    (middle, minmax[1]), curDepth+1);
        }

        return inter;
    }

}
}
#include "MemoryLoggerOff.h"
#endif

#endif // SCENEGRID_H
