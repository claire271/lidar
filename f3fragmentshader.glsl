varying vec2 tcoord;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;

uniform float width;
uniform float height;

float f3(vec2 pos) {
  vec4 tex1v = texture2D(tex1,pos);
  vec4 tex2v = texture2D(tex2,pos);
  vec4 tex3v = texture2D(tex3,pos);
  vec4 res;

  res = abs(tex1v - tex2v) + abs(tex2v - tex3v) + abs(tex3v - tex1v);

  return clamp(res.r - 0.5 * res.g - 0.5 * res.b,0.0,1.0);
}

float prs(float offset) {
  return texture2D(tex5,vec2(tcoord.x,1.0 - tcoord.y + offset)).r;
}

void main(void) 
{
  //Only once declarations
  vec4 res;

  //Factor with height
  //float factor = 1.0 / clamp(2.0 * (tcoord.y - 0.5),0.5,1.0);
  float factor = 1.0;

  //Initial f3 and color sub calculation
  res.r = f3(tcoord) * factor;
  
  //Calculating sharpened
  float sharpened = 
    -prs(-4.0/height) + -prs(-3.0/height) +
    -prs(-2.0/height) + prs(-1.0/height) +
    2.0 * prs(0.0) +
    prs(1.0/height) + -prs(2.0/height) +
    -prs(3.0/height) + -prs(4.0/height);

  vec4 tex5v = texture2D(tex5,vec2(tcoord.x,1.0-tcoord.y));
  res.b = 0.5 * clamp(sharpened,0.0,1.0) + 0.5 * tex5v.b;
  
  //Diagnostics output
  vec4 tex4v = texture2D(tex4,tcoord.xy + vec2(0,0));
  res.g = tex4v.g;
  
  res.a = 1.0;
  
  gl_FragColor = clamp(res,vec4(0),vec4(1));
}
