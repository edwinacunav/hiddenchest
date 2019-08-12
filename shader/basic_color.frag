
uniform sampler2D texture;
uniform lowp float red;
uniform lowp float green;
uniform lowp float blue;
varying vec2 v_texCoord;
const vec3 GRAY = vec3(.299, .587, .114);

void main()
{
  vec4 frag = texture2D(texture, v_texCoord);
  float gray = dot(frag.rgb, GRAY);
  frag.rgb = mix(frag.rgb, vec3(gray), 1.0);
  vec3 color = vec3(gray) * vec3(red, green, blue);
  frag.rgb = mix(frag.rgb, color.rgb, 0.8);
  gl_FragColor = frag;
}
