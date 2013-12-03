
#include <boost/atomic.hpp>
#include "scene/scene.h"

#include "core/MemoryLoggerOn.h"

using namespace tim::scene;
using namespace tim::core;



class MyTransformable : public Transformable
{
public:
    MyTransformable(SceneManager* sc) : Transformable(sc)
    {
        _volume.accurate=false;
        _volume.sphere = Sphere({Rand::frand({-20000,20000}),Rand::frand({-20000,20000}),Rand::frand({-20000,20000})}, 1);
        _volume.obb = OrientedBox(_volume.sphere.toBox(), mat4::IDENTITY());
    }

    virtual ~MyTransformable() {}
};

class TransformableFactory
{
public:
    Transformable* operator()(SceneManager* scene)
    {
        return new MyTransformable(scene);
    }
};



class AddObject
{

public:

    void doSomething(SceneManager* scene)
    {
        TransformableFactory factory;

        for(int i=0 ; i<100000 ; i++)
        {
            {
                Transformable* t = scene->addTransformable(factory);
                boost::lock_guard<decltype(_lock)> g(_lock);
                _result.push_back(t);
            }
        }
    }

    void remove(SceneManager* scene)
    {
        while(1)
        {
            Transformable* toDel=nullptr;
            {
                if(_result.size()>index)
                {
                    toDel=_result[index];
                    index++;
                }
                else return;
            }
            if(toDel)
                delete toDel;
        }
    }

    const boost::container::vector<Transformable*>& result() const { return _result; }
    int ind() const { return index; }

private:
    boost::mutex _lock;
    boost::container::vector<Transformable*> _result;
    int index=0;
};

struct delObject
{
    void operator()()
    {

    }
};


int main(int argc, char** argv)
{
    tim::core::init();
    {
        SceneManager scene;

        AddObject ao;

        boost::thread t1(&AddObject::doSomething, &ao, &scene);
        boost::thread t2(&AddObject::doSomething, &ao, &scene);
        /*boost::thread t3(&AddObject::doSomething, &ao, &scene);
        boost::thread t4(&AddObject::doSomething, &ao, &scene);
        boost::thread t5(&AddObject::doSomething, &ao, &scene);
        boost::thread t6(&AddObject::doSomething, &ao, &scene);
        boost::thread t7(&AddObject::doSomething, &ao, &scene);
        boost::thread t8(&AddObject::doSomething, &ao, &scene);*/

        t1.join();
        t2.join();
        /*t3.join();
        t4.join();
        t5.join();
        t6.join();
        t7.join();
        t8.join();*/

        out("start del\n");
        boost::thread del(&AddObject::remove, &ao, &scene);
        //system("pause");
        del.join();
    }
    tim::core::quit();

    return 0;
}

//#include <vector>
//#include <windows.h>
//#include <ctime>
//#include <iostream>

//    srand(time(NULL));
//    int res=0;
//
//size_t total=0;
//    system("pause");
//    for(int j=0 ; j<100 ; j++)
//    {
//        Test *okok=new Test[10000000];
//    size_t tim= GetTickCount();
//    for(int i=0 ; i<10000000 ; i++)
//    {
//        res += okok[i].getx();
//    }
//    tim=GetTickCount()-tim;
//    delete[] okok;
//    std::cout << j<<std::endl;
//    total += tim;
//    }
//
//size_t total2=0;
//
//    for(int j=0 ; j<100 ; j++)
//    {
//        Test *okok=new Test[10000000];
//    size_t tim= GetTickCount();
//    for(int i=0 ; i<10000000 ; i++)
//    {
//        res += okok[i].gety();
//    }
//    tim=GetTickCount()-tim;
//
//    delete[] okok;
//
//    std::cout << j<<std::endl;
//    total2 += tim;
//    }
//std::cout << res<<" total : "<<total*0.01<<"  all:"<<total << std::endl;
//    std::cout << res<<" total2 : "<<total2*0.01<<"  all2:"<<total2 << std::endl;
//system("pause");

