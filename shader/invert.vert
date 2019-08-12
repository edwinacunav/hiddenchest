
/* #if __VERSION__ >= 130
#define COMPAT_VARYING out
#define COMPAT_ATTRIBUTE in
#define COMPAT_TEXTURE texture
#else
#define COMPAT_VARYING varying
#define COMPAT_ATTRIBUTE attribute
#define COMPAT_TEXTURE texture2D
#endif */

#ifdef GL_ES
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif

attribute vec4 VertexCoord;//COMPAT_ATTRIBUTE vec4 VertexCoord;
attribute vec4 COLOR;//COMPAT_ATTRIBUTE vec4 COLOR;
attribute vec4 TexCoord;//COMPAT_ATTRIBUTE vec4 TexCoord;
varying vec4 COL0;//COMPAT_VARYING vec4 COL0;
varying vec4 TEX0;//COMPAT_VARYING vec4 TEX0;

uniform mat4 projMat;
uniform COMPAT_PRECISION int FrameDirection;
uniform COMPAT_PRECISION int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;

// vertex compatibility #defines
#define vTexCoord TEX0.xy
#define SourceSize vec4(TextureSize, 1.0 / TextureSize) //either TextureSize or InputSize
#define outsize vec4(OutputSize, 1.0 / OutputSize)

void main()
{
  gl_Position = projMat * VertexCoord;
  COL0 = COLOR;
  TEX0.xy = TexCoord.xy;
}
