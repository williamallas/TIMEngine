#include "resource.h"

#include "MemoryLoggerOn.h"
#define STR(s) #s
namespace tim
{
    using namespace core;
namespace resource
{

/******************/
/** GeometryPass **/
/******************/

const char* vShaderSrcGeometryPass =
/** Vertex shader source : GeometryPass */
"#version 330 \n\
 #define INSTANCING 128 \n\
 invariant gl_Position; \n\
 in vec3 vertex; \n\
 in vec3 normal; \n\
 smooth out vec3 in_normal; \n\
 \n\
#ifdef USE_TEXTURE \n\
 in vec2 texCoord; \n\
 out vec2 texCoord0; \n\
#endif \n\
 \n\
#ifdef DIRECTIONAL_LIGHT_TEST \n\
 smooth out vec3 vertexWorld; \n\
#endif \n\
 \n\
#ifndef NO_MATRIX \n\
 uniform mat4 projView; \n\
 uniform mat4 model[INSTANCING]; \n\
#endif \n\
 \n\
 void main() \n\
 { \n\
#ifndef NO_MATRIX \n\
    gl_Position=projView*model[gl_InstanceID]*vec4(vertex,1); \n\
#else \n\
    gl_Position=vec4(vertex,1); \n\
#endif \n\
 \n\
#ifdef USE_TEXTURE \n\
    texCoord0=texCoord; \n\
#endif \n\
#ifndef NO_MATRIX \n\
        mat3 normalMat=mat3(model[gl_InstanceID]); \n\
    #ifdef ACCURATE_NORMAL \n\
        normalMat = transpose(inverse(normalMat)); \n\
    #endif \n\
        in_normal=normalize(normalMat*normal); \n\
     \n\
    #ifdef DIRECTIONAL_LIGHT_TEST \n\
        vertexWorld=vec3(model[gl_InstanceID]*vec4(vertex,1)); \n\
    #endif \n\
#else \n\
    in_normal=normalize(normal); \n\
#endif \n\
 \n\
 }";

const char* pShaderSrcGeometryPass =
/** Pixel shader source : GeometryPass */
"#version 330 \n\
 \n\
 in vec3 in_normal; \n\
#ifdef USE_TEXTURE \n\
 in vec2 texCoord0; \n\
 uniform sampler2D texture0; \n\
#endif \n\
 \n\
#ifdef DIRECTIONAL_LIGHT_TEST \n\
 in vec3 vertexWorld; \n\
 uniform vec3 cameraWorld; \n\
#endif \n\
 \n\
 uniform vec4 color; \n\
 uniform vec4 material; \n\
 uniform float exponent; \n\
 layout(location=0) out vec4 outColor0; \n\
#ifdef DEFERRED \n\
 layout(location=1) out vec4 outColor1; \n\
 layout(location=2) out vec4 outColor2; \n\
#endif \n\
 \n\
 void main() \n\
 { \n\
    outColor0=color; \n\
#ifdef USE_TEXTURE \n\
    outColor0 *= texture(texture0, texCoord0); \n\
#endif \n\
 \n\
#ifdef DIRECTIONAL_LIGHT_TEST \n\
    vec4 col=outColor0; \n\
    vec3 dLightTestColor=vec3(0.2,1,1); \n\
    vec3 dLightTestDir=normalize(vec3(-0.5,0.5,-0.7)); \n\
    outColor0 = col*dLightTestColor.r*material.r; // ambient \n\
    outColor0 += col*max(0,dot(in_normal, -dLightTestDir))*dLightTestColor.g*material.g; // diffuse \n\
    \n\
	vec3 vDir = normalize(cameraWorld - vertexWorld); \n\
	outColor0 += col*material.z*pow(max(dot(reflect(dLightTestDir, in_normal), vDir),0), exponent); \n\
    outColor0 += col*material.w; \n\
    outColor0.a=col.a; \n\
#endif \n\
#ifdef DRAW_NORMAL \n\
 outColor0.xyz=in_normal*0.5+0.5; \n\
#endif \n\
#ifdef DEFERRED \n\
 outColor1 = vec4(in_normal*0.5+0.5, clamp(exponent/1024, 0, 1)); \n\
 outColor2 = material; \n\
#endif \n\
 }";

 const unsigned char textureTest_4x4_1[16] = {0,0, 255,255,
                                              0,0, 255,255,
                                              255,255, 0,0,
                                              255,255, 0,0};
#define C1 255,255,255
#define C0 0,0,0
 const unsigned char textureTest_4x4_3[48] = {C0,C0, C1,C1,
                                              C0,C0, C1,C1,
                                              C1,C1, C0,C0,
                                              C1,C1, C0,C0};
#undef C1
#undef C0

const float quad_mesh_vertex_4[20] = { -1,1,0, 0,1, 1,1,0, 1,1,
                                       1,-1,0, 1,0, -1,-1,0, 0,0 };

const unsigned int quad_mesh_index_4[4] = { 0,1,2,3 };




}
}
