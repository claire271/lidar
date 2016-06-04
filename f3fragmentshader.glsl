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
  
  /*
  res.r = f3(tcoord);

  //Calculating sharpened
  res.b =
    -f3(tcoord - vec2(0,2.0/height)) + f3(tcoord - vec2(0,1.0/height)) +
    res.r +
    -f3(tcoord + vec2(0,2.0/height)) + f3(tcoord + vec2(0,1.0/height));
  //res.b =
    //f3(tcoord - vec2(0,1.0/height))/2.0 +
    ////res.r +
    //f3(tcoord + vec2(0,1.0/height))/2.0;
  res.b /= 3.0;

  //Diagnostics output
  vec4 tex4v = texture2D(tex4,tcoord.xy + vec2(0,0));
  res.g = tex4v.g;
  res.a = 1.0;
  */

  vec4 tex1vt3 = texture2D(tex1,tcoord - vec2(0,3.0/height));
  vec4 tex1vt2 = texture2D(tex1,tcoord - vec2(0,2.0/height));
  vec4 tex1vt1 = texture2D(tex1,tcoord - vec2(0,1.0/height));
  vec4 tex1v = texture2D(tex1,tcoord);
  vec4 tex1vb1 = texture2D(tex1,tcoord + vec2(0,1.0/height));
  vec4 tex1vb2 = texture2D(tex1,tcoord + vec2(0,2.0/height));
  vec4 tex1vb3 = texture2D(tex1,tcoord + vec2(0,3.0/height));

  res.r =
    -0.0 * tex1vt3.r + -2.0 * tex1vt2.r +
    2.0 * tex1vt1.r + 3.0 * tex1v.r + 2.0 * tex1vb1.r +
    -2.0 * tex1vb2.r + -0.0 * tex1vt3.r;
  res.r /= 9.0;
  res.g = res.r;
  res.b = res.r;
  res.a = 1.0;

  gl_FragColor = clamp(res,vec4(0),vec4(1));
}
