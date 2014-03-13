#ifndef RESOURCEHANDLER_H_INCLUDED
#define RESOURCEHANDLER_H_INCLUDED

#include <boost/shared_ptr.hpp>

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{
    template <class R, class Loader>
    class ResourceHandler
    {
    public:
        ResourceHandler(Loader* loader)
        {
            _loader = loader;
        }

        virtual ~ResourceHandler() {}

        Option<boost::shared_ptr<R>> get(const std::string& name)
        {
            auto it = _resource.find(name);
            if(it == _resource.end())
            {
                R* res = _loader->load(name);

                if(!res)
                    return Option<boost::shared_ptr<R>>();
                else
                {
                    boost::weak_ptr<R> wPtr = boost::weak_ptr<R>(res);
                    _resource[name] = wPtr;
                    return Option<boost::shared_ptr<R>>(wPtr.lock());
                }
            }
            else
            {
                if(boost::shared_ptr<R> ptr = it->lock())
                    return ptr;
                else
                {
                    R* res = _loader->load(name);

                    if(!res)
                        return Option<boost::shared_ptr<R>>();
                    else
                    {
                        boost::weak_ptr<R> wPtr = boost::weak_ptr<R>(res);
                        _resource[name] = wPtr;
                        return Option<boost::shared_ptr<R>>(wPtr.lock());
                    }
                }
            }
        }


    private:
        boost::container::map<std::string, boost::weak_ptr<R>> _resource;
        Loader* _loader=nullptr;
    };


}
}
#include "MemoryLoggerOff.h"


#endif // RESOURCEHANDLER_H_INCLUDED
