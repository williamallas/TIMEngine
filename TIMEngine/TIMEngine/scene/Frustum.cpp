#include "Frustum.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace scene
{

Frustum::Frustum()
{
    //ctor
}

Frustum::~Frustum()
{
    //dtor
}

void Frustum::buildCameraFrustum(float fov, float ratio, const vec2& dist,
                                 const vec3& pos, const vec3& dir, const vec3& up,
                                 size_t maskPlan)
{
	float tang = tan(toRad(fov)*0.5) ;
	float nh = dist.x() * tang;
	float nw = nh * ratio;
	float fh = dist.y()  * tang;
	float fw = fh * ratio;

    vec3 nc,fc,X,Y,Z;

	Y = (dir-pos).normalized();
	X = up.cross(Y).normalized();
	Z = Y.cross(X);

	nc = (Y*dist.x()) + pos;
	fc = (Y*dist.y()) + pos;

    vec3 ntl = nc - X*nw + Z*nh;
    vec3 ntr = nc + X*nw + Z*nh;
    vec3 nbl = nc - X*nw - Z*nh;
    vec3 nbr = nc + X*nw - Z*nh;

    vec3 ftl = fc - X*fw + Z*fh;
    vec3 ftr = fc + X*fw + Z*fh;
    vec3 fbl = fc - X*fw - Z*fh;
    vec3 fbr = fc + X*fw - Z*fh;

    _plans.clear();

    if(!(maskPlan & BUILD_MASK(FrustumPlan::LEFT)))
        _plans.push_back(Plan(ntl,nbl,fbl));
    if(!(maskPlan & BUILD_MASK(FrustumPlan::RIGHT)))
        _plans.push_back(Plan(nbr,ntr,fbr));
    if(!(maskPlan & BUILD_MASK(FrustumPlan::UP)))
        _plans.push_back(Plan(ntr,ntl,ftl));
    if(!(maskPlan & BUILD_MASK(FrustumPlan::DOWN)))
        _plans.push_back(Plan(nbl,nbr,fbr));
    if(!(maskPlan & BUILD_MASK(FrustumPlan::NEAR)))
        _plans.push_back(Plan(ntl,ntr,nbr));
    if(!(maskPlan & BUILD_MASK(FrustumPlan::FAR)))
        _plans.push_back(Plan(ftr,ftl,fbl));
}

Intersection Frustum::collide(const Sphere& s) const
{
    Intersection result = INSIDE;
	float distance;

	for(size_t i=0; i<_plans.size(); ++i)
	{
		distance = _plans[i].distance(s.center());

        if (distance < -s.radius())
			return OUTSIDE;
		else if (distance < s.radius())
			result =  INTERSECT;
	}

    return result;
}

Intersection Frustum::collide(const Box& b) const
{
    Intersection result = INSIDE;
	for(int i=0; i < _plans.size(); ++i)
    {
		if(_plans[i].distance(getBoxVertexP(b, _plans[i].plan().down<1>())) < 0)
			return OUTSIDE;
		else if(_plans[i].distance(getBoxVertexN(b, _plans[i].plan().down<1>())) < 0)
            result = INTERSECT;
	}

	return result;
}

bool Frustum::collide(const vec3& p) const
{
    for(int i=0; i < _plans.size(); ++i)
    {
		if(_plans[i].distance(p) < 0)
			return false;
	}
	return true;
}

vec3 Frustum::getBoxVertexP(const Box& b, const vec3& n) const
{
    vec3 result;
    for(size_t i=0 ; i<3 ; ++i)
    {
        if (n[i] > 0) result.set(b.box()[i][1], i);
        else result.set(b.box()[i][0], i);
    }

	return result;
}

vec3 Frustum::getBoxVertexN(const Box& b, const vec3& n) const
{
    vec3 result;
    for(size_t i=0 ; i<3 ; ++i)
    {
        if (n[i] < 0) result.set(b.box()[i][1], i);
        else result.set(b.box()[i][0], i);
    }

	return result;
}

}
}
