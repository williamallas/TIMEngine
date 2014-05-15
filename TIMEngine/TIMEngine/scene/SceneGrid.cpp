#ifndef USE_OCTREE

#include "SceneGrid.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace scene
{

const float SceneGrid::CELL_SIZE_X=SceneGrid::GRID_SIZE/SceneGrid::NB_CELL_X;
const float SceneGrid::CELL_SIZE_Y=SceneGrid::GRID_SIZE/SceneGrid::NB_CELL_Y;
const float SceneGrid::CELL_SIZE_Z=SceneGrid::GRID_SIZE/SceneGrid::NB_CELL_Z;
const float SceneGrid::GRID_SIZE = 4096;

SceneGrid::SceneGrid(const vec3& box, SceneManager* scene)
{
    _box=Box({box-vec3(GRID_SIZE/2)}, {box+vec3(GRID_SIZE/2)});
    _minBox = _box.min();
    _grid=new Cell[NB_CELLS];
    _scene = scene;

    for(size_t i=0 ; i<NB_CELLS ; ++i)
    {
        uivec3 index = mapIndex(i);
        vec3 findex=vec3(index.x(), index.y(), index.z())*vec3(CELL_SIZE_X, CELL_SIZE_Y, CELL_SIZE_Z);

        _grid[i].setBox(Box(findex+_minBox, findex+_minBox+vec3(CELL_SIZE_X, CELL_SIZE_Y, CELL_SIZE_Z)));
        _grid[i].setParent(this);
    }
}

SceneGrid::~SceneGrid()
{
    delete[] _grid;
}

Intersection SceneGrid::insert(Transformable* obj)
{
    Intersection inter;
    if(obj->volume().obb)
        inter = _box.collide(obj->volume().box());
    else
        inter = _box.collide(obj->volume().sphere);

    if(inter != OUTSIDE)
    {
        inject(obj);
        _allObject.push_back(obj);
        obj->grids().push_back({this, _allObject.size()-1});
    }

    return inter;
}

void SceneGrid::inject(Transformable* obj)
{
    if(obj->volume().sphere.radius() > std::min(std::min(CELL_SIZE_X, CELL_SIZE_Y), CELL_SIZE_Z))
        insertBig(obj);
    else insertSmall(obj);
}

void SceneGrid::remove(size_t index)
{
    std::swap(_allObject[index], _allObject[_allObject.size()-1]);

    for(size_t i=0 ; i<_allObject[index]->grids().size() ; ++i)
    {
        if(_allObject[index]->grids()[i].container == this)
        {
            _allObject[index]->grids()[i].index = index;
            break;
        }
    }
    _allObject.pop_back();
}

void SceneGrid::insertBig(Transformable* obj)
{
    if(obj->volume().obb)
    {
        for(size_t i=0 ; i<NB_CELLS ; ++i)
        {
            if(_grid[i].box().collide(obj->volume().box()) != OUTSIDE)
                _grid[i].addTransformable(obj);
        }
    }
    else
    {
        for(size_t i=0 ; i<NB_CELLS ; ++i)
        {
            if(_grid[i].box().collide(obj->volume().sphere) != OUTSIDE)
                _grid[i].addTransformable(obj);
        }
    }

}

void SceneGrid::insertSmall(Transformable* obj)
{
    vec3 center = obj->volumeCenter();

    for(size_t i=0 ; i<3 ; ++i)
    {
        center[i] = std::max(_box.min()[i]+0.01f, center[i]);
        center[i] = std::min(_box.max()[i]-0.01f, center[i]);
    }

    uivec3 index = mapIndex(center);
    ivec3 i_index = {(int)index.x(), (int)index.y(), (int)index.z()};

    Intersection inter;
    size_t indexCell;

    if(obj->volume().obb)
    {
        indexCell = mapIndex(uivec3(static_cast<size_t>(i_index.x()),
                                    static_cast<size_t>(i_index.y()),
                                    static_cast<size_t>(i_index.z())));
        inter = _grid[indexCell].box().collide(obj->volume().box());

        switch(inter)
        {
        case INSIDE:
            _grid[indexCell].addTransformable(obj);
            return;
        case INTERSECT:
            _grid[indexCell].addTransformable(obj);
        case OUTSIDE:
            break;
        }

        for(int i=-1 ; i<=1 ; ++i)
            for(int j=-1 ; j<=1 ; ++j)
                for(int k=-1 ; k<=1 ; ++k)
        {
            if(i==0&&j==0&&k==0)
                continue;
            else if(i_index.x()+i < 0 || i_index.x()+i >= NB_CELL_X ||
               i_index.y()+j < 0 || i_index.y()+j >= NB_CELL_Y ||
               i_index.z()+k < 0 || i_index.z()+k >= NB_CELL_Z)
                continue;

            indexCell = mapIndex(uivec3(static_cast<size_t>(i_index.x()+i),
                                        static_cast<size_t>(i_index.y()+j),
                                        static_cast<size_t>(i_index.z()+k)));
            inter = _grid[indexCell].box().collide(obj->volume().box());

            switch(inter)
            {
            case INSIDE:
                _grid[indexCell].addTransformable(obj);
                return;
            case INTERSECT:
                _grid[indexCell].addTransformable(obj);
            case OUTSIDE:
                break;
            }
        }
    }
    else
    {
        indexCell = mapIndex(uivec3(static_cast<size_t>(i_index.x()),
                                    static_cast<size_t>(i_index.y()),
                                    static_cast<size_t>(i_index.z())));

        inter = _grid[indexCell].box().collide(obj->volume().sphere);

        switch(inter)
        {
        case INSIDE:
            _grid[indexCell].addTransformable(obj);
            return;
        case INTERSECT:
            _grid[indexCell].addTransformable(obj);
        case OUTSIDE:
            break;
        }

        for(int i=-1 ; i<=1 ; ++i)
            for(int j=-1 ; j<=1 ; ++j)
                for(int k=-1 ; k<=1 ; ++k)
        {
            if(i==0&&j==0&&k==0)
                continue;
            else if(i_index.x()+i < 0 || i_index.x()+i >= NB_CELL_X ||
               i_index.y()+j < 0 || i_index.y()+j >= NB_CELL_Y ||
               i_index.z()+k < 0 || i_index.z()+k >= NB_CELL_Z)
                continue;

            indexCell = mapIndex(uivec3(static_cast<size_t>(i_index.x()+i),
                                        static_cast<size_t>(i_index.y()+j),
                                        static_cast<size_t>(i_index.z()+k)));
            inter = _grid[indexCell].box().collide(obj->volume().sphere);

            switch(inter)
            {
            case INSIDE:
                _grid[indexCell].addTransformable(obj);
                return;
            case INTERSECT:
                _grid[indexCell].addTransformable(obj);
            case OUTSIDE:
                break;
            }
        }
    }
}

std::string SceneGrid::str() const
{
    std::string res=std::string("SceneGrid:")+_box.str()+"\n";
    for(size_t i=0 ; i<NB_CELL_X*NB_CELL_Y*NB_CELL_Z ; ++i)
    {
        if(_grid[i].container().size())
            res+=mapIndex(i).str()+":"+StringUtils(_grid[i].container().size()).str()+"\n";
    }
    return res;
}

}
}

#endif
