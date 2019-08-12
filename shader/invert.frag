
uniform sampler2D Texture;
varying vec4 TEX0;// COMPAT_VARYING vec4 TEX0;
// fragment compatibility #defines
#define Source Texture
#define vTexCoord TEX0.xy

void main()
{
  vec3 color = texture2D(Source, vTexCoord).rgb;
  gl_FragColor = vec4(1.0 - color.rgb, 1.0);
}
