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

 const char* vTerrainShaderSrcGeometryPass =
/** Terrain Vertex shader source : GeometryPass */
"#version 330 \n\
 invariant gl_Position; \n\
 #define INSTANCING 128 \n\
 in vec3 vertex; \n\
 \n\
 uniform mat4 projView; \n\
 uniform mat4 model[INSTANCING]; \n\
 uniform sampler2D texture0; \n\
 uniform sampler2D texture1; \n\
 uniform vec4 userData[1]; // coordx coordy sizePatch zsclae \n\
 smooth out vec3 norm; \n\
 smooth out vec3 vertexWorld; \n\
 smooth out vec3 texCoord; \n\
 \n\
 #define textureSize 512.0 \n\
 #define texelSize 1.0 / 512.0 \n\
 \n\
 vec4 texture_bilinear(sampler2D tex, vec2 uv) \n\
 { \n\
     vec2 f = fract(uv.xy * textureSize); \n\
     vec4 t00 = texture(tex, uv); \n\
     vec4 t10 = texture(tex, uv + vec2(texelSize, 0.0)); \n\
     vec4 tA = mix(t00, t10, f.x); \n\
     vec4 t01 = texture(tex, uv + vec2(0.0, texelSize)); \n\
     vec4 t11 = texture(tex, uv + vec2(texelSize, texelSize)); \n\
     vec4 tB = mix(t01, t11, f.x); \n\
     return mix(tA, tB, f.y); \n\
 } \n\
 \n\
 void main() \n\
 { \n\
    vertexWorld=vec3(model[gl_InstanceID]*vec4(vertex,1)); \n\
    vec2 coord = ((vertexWorld.xy - userData[0].z*userData[0].xy + userData[0].z*0.5)/userData[0].z); \n\
    coord.x=clamp(coord.x, 0, 0.9999);\n\
    coord.y=clamp(coord.y, 0, 0.9999);\n\
    vec3 height3=texture(texture0, coord).xyz; \n\
    int z = int(height3.x*255.f + height3.y*255.f*255.f + height3.z*255.f*255.f*255.f); \n\
    vertexWorld.z = (z/pow(2.f, 24)); \n\
    texCoord = vec3(coord, vertexWorld.z); \n\
    vertexWorld.z *= userData[0].w; \n\
    norm = normalize(texture(texture1, coord).xyz*2-1); \n\
    gl_Position=projView*vec4(vertexWorld,1); \n\
   \n\
 }";

const char* pTerrainShaderSrcGeometryPass =
"#version 330 \n\
 \n\
 uniform vec4 color; \n\
 uniform vec4 material; \n\
 uniform float exponent; \n\
 uniform sampler2D texture1; \n\
 uniform sampler2DArray texture2; \n\
 uniform vec2 textureScale2; \n\
 smooth in vec3 norm; \n\
 smooth in vec3 texCoord; \n\
 in vec3 vertexWorld; \n\
 uniform vec3 cameraWorld; \n\
 layout(location=0) out vec4 outColor0; \n\
 \n\
 vec4 read(sampler2DArray tex, int level) \n\
 { \n\
    const float B=0.7; const float DB=0.05; \n\
    \n\
    if(norm.z > B+DB) return texture(tex, vec3(texCoord.xy*textureScale2.x,level*2)); \n\
    else if(norm.z < B-DB) return texture(tex, vec3(texCoord.xy*textureScale2.x,1+level*2)); \n\
    else \n\
    { \n\
        float coef=(norm.z-(B-DB))/(2*DB); \n\
        return (texture(tex, vec3(texCoord.xy*textureScale2.x,level*2))*coef + texture(tex, vec3(texCoord.xy*textureScale2.x,1+level*2))*(1.f-coef)); \n\
    } \n\
 } \n\
 \n\
 vec4 read(sampler2DArray tex) \n\
 { \n\
    const float D=0.05; \n\
    const int STEP=3; \n\
    for(int i=0 ; i<STEP ; ++i) \n\
    { \n\
        float tmp = (i+1.f)/STEP; \n\
        if(texCoord.z < tmp - D) \n\
            return read(tex, i); \n\
        else if(texCoord.z > tmp + D) \n\
            continue; \n\
        else  \n\
        { \n\
            float c=(texCoord.z-tmp+D)/(2*D); \n\
            return read(tex, i+1)*c+read(tex, i)*(1-c); \n\
        } \n\
 \n\
    } \n\
 } \n\
 \n\
 void main() \n\
 { \n\
    vec4 col=color; \n\
    vec3 dLightTestColor=vec3(0.2,1,1); \n\
    vec3 dLightTestDir=normalize(vec3(-0.5,0.5,-0.7)); \n\
    outColor0 = col*dLightTestColor.r*material.r; // ambient \n\
    outColor0 += col*max(0,dot(norm, -dLightTestDir))*dLightTestColor.g*material.g; // diffuse \n\
    \n\
	vec3 vDir = normalize(cameraWorld - vertexWorld); \n\
	outColor0 += col*material.z*pow(max(dot(reflect(dLightTestDir, norm), vDir),0), exponent); \n\
    outColor0 += col*material.w; \n\
    \n\
    outColor0 *= read(texture2); \n\
    outColor0.a=col.a; \n\
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

const uint quad_mesh_index_4[4] = { 0,1,2,3 };




}
}
