#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "Timath.h"
#include "StringUtils.h"

#include "MemoryLoggerOn.h"

namespace tim
{
namespace core
{

    template <class T, size_t N>
    class Vector
    {
    public:
        Vector() { for(size_t i=0;i<N;i++)_val[i]=T(); }
        Vector(const T& val) { for(size_t i=0;i<N;i++)_val[i]=val; }
        Vector(const T val[]) { for(size_t i=0;i<N;i++)_val[i]=val[i]; }
        template<size_t A> Vector(const Vector<T,A>& v) : Vector() { for(size_t i=0;i<std::min(N,A);i++)_val[i]=v[i]; }

        Vector(std::initializer_list<T> l)
        {
            auto it=l.begin();
            for(size_t i=0;it!=l.end() && i<N;i++)
            {
                _val[i]=*it;
                it++;
            }
        }

        const T& x() const { return _val[0]; }
        const T& y() const { static_assert(N>1, "Invalid component access."); return _val[1]; }
        const T& z() const { static_assert(N>2, "Invalid component access."); return _val[2]; }
        const T& w() const { static_assert(N>3, "Invalid component access."); return _val[3]; }

        T& x() { return _val[0]; }
        T& y() { static_assert(N>1, "Invalid component access."); return _val[1]; }
        T& z() { static_assert(N>2, "Invalid component access."); return _val[2]; }
        T& w() { static_assert(N>3, "Invalid component access."); return _val[3]; }

        Vector& set(const T& v, int i) { _val[i] = v; return *this; }

        Vector& operator=(const Vector& v) { for(size_t i=0;i<N;i++)_val[i]=v[i]; return *this; }
        bool operator==(const Vector& v) const { for(size_t i=0;i<N;i++){if(_val[i]!=v[i])return false;} return true; }
        bool operator!=(const Vector& v) const { return !((*this)==v); }

        Vector operator+(const Vector& v) const { Vector vec; for(size_t i=0;i<N;i++)vec.set(_val[i]+v[i], i); return vec; }
        Vector operator-(const Vector& v) const { Vector vec; for(size_t i=0;i<N;i++)vec.set(_val[i]-v[i], i); return vec; }
        Vector operator*(const Vector& v) const { Vector vec; for(size_t i=0;i<N;i++)vec.set(_val[i]*v[i], i); return vec; }
        Vector operator/(const Vector& v) const { Vector vec; for(size_t i=0;i<N;i++)vec.set(_val[i]/v[i], i); return vec; }

        Vector& operator+=(const Vector& v)  { for(size_t i=0;i<N;i++)_val[i]+=v[i]; return *this; }
        Vector& operator-=(const Vector& v)  { for(size_t i=0;i<N;i++)_val[i]-=v[i]; return *this; }
        Vector& operator*=(const Vector& v)  { for(size_t i=0;i<N;i++)_val[i]*=v[i]; return *this; }
        Vector& operator/=(const Vector& v)  { for(size_t i=0;i<N;i++)_val[i]/=v[i]; return *this; }

        Vector operator+(const T& v) const { Vector vec; for(size_t i=0;i<N;i++)vec.set(_val[i]+v, i); return vec; }
        Vector operator-(const T& v) const { Vector vec; for(size_t i=0;i<N;i++)vec.set(_val[i]-v, i); return vec; }
        Vector operator*(const T& v) const { Vector vec; for(size_t i=0;i<N;i++)vec.set(_val[i]*v, i); return vec; }
        Vector operator/(const T& v) const { Vector vec; for(size_t i=0;i<N;i++)vec.set(_val[i]/v, i); return vec; }

        Vector& operator+=(const T& v) { for(size_t i=0;i<N;i++)_val[i]+=v; return *this; }
        Vector& operator-=(const T& v) { for(size_t i=0;i<N;i++)_val[i]-=v; return *this; }
        Vector& operator*=(const T& v) { for(size_t i=0;i<N;i++)_val[i]*=v; return *this; }
        Vector& operator/=(const T& v) { for(size_t i=0;i<N;i++)_val[i]/=v; return *this; }

        Vector operator-() const { return *this * -1; }

        T& operator[](size_t i) { return _val[i]; }
        const T& operator[](size_t i) const { return _val[i]; }

        T dot(const Vector<T,N> & v) const
        {
             T res=0;
             for(size_t i=0;i<N;i++) res+=(_val[i]*v[i]);
             return res;
        }

        T length2() const { T res=0; for(size_t i=0;i<N;i++)res+=(_val[i]*_val[i]); return res; }
        T length() const { return sqrt(length2()); }

        Vector& normalize() {(*this)/=length(); return *this; }
        Vector normalized() const { return (*this)/length(); }

        Vector cross(const Vector& v) const
        {
            Vector<T, N> res;
            for(size_t i=0; i < N; i++)
                res[i] = _val[(i+1) % N] * v[(i+2) % N] - _val[(i+2) % N] * v[(i+1) % N];
            return res;
        }

        template<size_t A>
        Vector<T,A> to() const
        {
            Vector<T,A> v;
            for(size_t i=0 ; i<std::min(A,N) ; i++) v[i]=_val[i];
            return v;
        }

        template<size_t A> Vector<T,N-A> down() const { static_assert(N-A>0, "Invalid vector size."); return to<N-A>(); }
        template<size_t A> Vector<T,N+A> extend() const { return to<N+A>(); }

        /* String util */
        std::string str() const
        {
            std::string str="Vector(";
            for(size_t i=0;i<N-1;i++)
                str+=StringUtils(_val[i]).str()+",";
            str += StringUtils(_val[N-1]).str()+")";
            return str;
        }
        friend std::ostream& operator<< (std::ostream& stream, const Vector& t) { stream << t.str(); return stream;}

    protected:
        T _val[N];
    };

    template<class T>
    class Vector4 : public Vector<T,4>
    {
    public:
        Vector4() : Vector<T,4>() {}
        Vector4(const T& x, const T& y, const T& z, const T& w) : Vector<T,4>() {  Vector<T,4>::set(x,0);
                                                                                   Vector<T,4>::set(y,1);
                                                                                   Vector<T,4>::set(z,2);
                                                                                   Vector<T,4>::set(w,3); }
        Vector4(const T& v) : Vector<T,4>(v) {}
        template<size_t A> Vector4(const Vector<T,A>& v) : Vector<T,4>(v) {}
        Vector4(const T val[]) : Vector<T,4>(val) {}
        Vector4(std::initializer_list<T> l) : Vector<T,4>(l) {}
    };

    template<class T>
    class Vector3 : public Vector<T,3>
    {
    public:
        Vector3() : Vector<T,3>() {}
        Vector3(const T& x, const T& y, const T& z) : Vector<T,3>() { Vector<T,3>::set(x,0);
                                                                      Vector<T,3>::set(y,1);
                                                                      Vector<T,3>::set(z,2); }
        Vector3(const T& v) : Vector<T,3>(v) {}
        template<size_t A> Vector3(const Vector<T,A>& v) : Vector<T,3>(v) {}
        Vector3(const T val[]) : Vector<T,3>(val) {}
        Vector3(std::initializer_list<T> l) : Vector<T,3>(l) {}
    };

    template<class T>
    class Vector2 : public Vector<T,2>
    {
    public:
        Vector2() : Vector<T,2>() {}
        Vector2(const T& x, const T& y) : Vector<T,2>() { Vector<T,2>::set(x,0);Vector<T,2>::set(y,1); }
        Vector2(const T& v) : Vector<T,2>(v) {}
        template<size_t A> Vector2(const Vector<T,A>& v) : Vector<T,2>(v) {}
        Vector2(const T val[]) : Vector<T,2>(val) {}
        Vector2(std::initializer_list<T> l) : Vector<T,2>(l) {}
    };

    typedef Vector4<float> vec4;
    typedef Vector4<int> ivec4;
    typedef Vector4<size_t> uivec4;
    typedef Vector4<double> dvec4;

    typedef Vector3<float> vec3;
    typedef Vector3<int> ivec3;
    typedef Vector3<size_t> uivec3;
    typedef Vector3<double> dvec3;

    typedef Vector2<float> vec2;
    typedef Vector2<int> ivec2;
    typedef Vector2<size_t> uivec2;
    typedef Vector2<double> dvec2;

    typedef Vector<float,1> vec1;
    typedef Vector<int,1> ivec1;
    typedef Vector<size_t,1> uivec1;
    typedef Vector<double,1> dvec1;

}
}

#include "MemoryLoggerOff.h"


#endif // VECTOR_H_INCLUDED
