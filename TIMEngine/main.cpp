
#include <boost/atomic.hpp>
#include <boost/timer/timer.hpp>
#include "scene/scene.h"

#include "core/MemoryLoggerOn.h"

using namespace tim::scene;
using namespace tim::core;

class MyTransformable : public Transformable
{
public:
    MyTransformable(SceneManager* sc) : Transformable(sc)
    {
        _volume.sphere = Sphere({Rand::frand({-6000,6000}),Rand::frand({-6000,6000}),Rand::frand({-200,2000})}, 0.5);
    }
    boost::mutex _lock;

    virtual ~MyTransformable() {}
};

class MyTransformable2 : public Transformable
{
public:
    MyTransformable2(SceneManager* sc) : Transformable(sc)
    {
        static int x=-4;
        _volume.sphere = Sphere({x,1.2,1.2}, 0.1);
        x+=1;
    }

    virtual ~MyTransformable2() {}

    void set() { _volume.sphere = Sphere({4.6,4.5,4.5}, 0.1); }
};

class TransformableFactory
{
public:
    Transformable* operator()(SceneManager* scene)
    {
        return new MyTransformable(scene);
    }
};

class Transformable2Factory
{
public:
    Transformable* operator()(SceneManager* scene)
    {
        return new MyTransformable2(scene);
    }
};



class AddObject
{

public:

    void doSomething(SceneManager* scene)
    {
        TransformableFactory factory;

        for(int i=0 ; i<10000 ; i++)
        {
            Transformable* toMove=0;
            {
                Transformable* t = scene->addTransformable(factory,true);

                boost::lock_guard<decltype(_lock)> g(_lock);
                _result.push_back(t);
            }
        }
    }

    void benchMove(SceneManager* scene)
    {
        Transformable* t=0;
        for(size_t i=0 ; i<3000 ; i++)
        {

            {
                boost::lock_guard<decltype(_lock)> g(_lock);
                t = _result[Rand::rand({0,_result.size()-1})];
            }
            {
                scene->move(t);
            }
        }
    }

    const boost::container::vector<Transformable*>& result() const { return _result; }
    int ind() const { return index; }

private:
    boost::mutex _lock;
    boost::container::vector<Transformable*> _result;
    int index=0;
};


int main(int argc, char** argv)
{
    tim::core::init();
    {
        SceneManager scene;
        int ii=0;

        ii=ii*2;
        ii++;
        ii--;
        out(ii);

        TransformableFactory factory;
        Transformable **tar = new Transformable*[500000];
        out("init time:");
        {
            boost::timer::auto_cpu_timer t;

            for(int i=0 ; i<500000 ; i++)
                tar[i] = scene.addTransformable(factory);

            scene.flushOctree();
        }


        Transformable* tr=scene.addTransformable(factory);


        out("\nbench move: ");
        {
            boost::timer::auto_cpu_timer t;

            for(int i=0 ; i<30000 ; i++)
                tar[i]->move();

        }
        {
            boost::timer::auto_cpu_timer t;
            out("\nnb move:");out(scene.flushOctree());
        }

        scene.clearScene();
        out("\nScene:\n"+scene.str());

    }
    tim::core::quit();

    return 0;
}
