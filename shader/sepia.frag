
uniform sampler2D texture;
varying vec2 v_texCoord;
const vec3 GRAY = vec3(0.299, 0.587, 0.114);
const vec3 SEPIA = vec3(1.2, 1.0, 0.8);

void main()
{
  vec4 texColor = texture2D(texture, v_texCoord);
  float gray = dot(texColor.rgb, GRAY);
  vec3 sepia = vec3(gray) * SEPIA;
  texColor.rgb = mix(texColor.rgb, sepia, 0.8);
  gl_FragColor = texColor;
}