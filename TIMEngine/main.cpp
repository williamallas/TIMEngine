
#include "scene/scene.h"

#include "core/MemoryLoggerOn.h"

using namespace tim::scene;
using namespace tim::core;


int main(int argc, char** argv)
{
    tim::core::init();
    {


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

