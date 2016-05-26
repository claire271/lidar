varying vec2 tcoord;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;

uniform float width;
uniform float height;

float f3(vec2 pos) {
  vec4 tex1v = texture2D(tex1,pos);
  vec4 tex2v = texture2D(tex2,pos);
  vec4 tex3v = texture2D(tex3,pos);
  vec4 res;

  res = abs(tex1v - tex2v) + abs(tex2v - tex3v) + abs(tex3v - tex1v);
  res.r -= res.g * 0.5 + res.b * 0.5;

  return res.r;
}

void main(void) 
{
  //Only once declarations
  vec4 res;
  
  res.r = f3(tcoord);

  //Calculating sharpened
  res.b =
    -f3(tcoord - vec2(0,2.0/height)) + f3(tcoord - vec2(0,1.0/height)) +
    res.r +
    -f3(tcoord + vec2(0,2.0/height)) + f3(tcoord + vec2(0,1.0/height));
  res.b /= 3.0;

  //Diagnostics output
  vec4 tex4v = texture2D(tex4,tcoord.xy + vec2(0,0));
  res.g = tex4v.g;
  res.a = 1.0;

  gl_FragColor = clamp(res,vec4(0),vec4(1));
}
