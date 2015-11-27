varying vec2 tcoord;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
void main(void) 
{
    vec4 tex1v = texture2D(tex1,tcoord);
    vec4 tex2v = texture2D(tex2,tcoord);
    vec4 tex3v = texture2D(tex3,tcoord);

    vec4 res;
    res = abs(tex1v - tex2v) + abs(tex2v - tex3v) + abs(tex3v - tex1v);
    res.g = res.r;
    res.b = res.r;
    res.a = 1.0;

    gl_FragColor = clamp(res,vec4(0),vec4(1));
}
