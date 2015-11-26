varying vec2 tcoord;
uniform sampler2D on;
uniform sampler2D off;
void main(void) 
{
    vec4 on_vec = texture2D(on,tcoord);
    vec4 off_vec = texture2D(off,tcoord);

    vec4 res;
    res.r = on_vec.r - off_vec.r;
    res.g = on_vec.g - off_vec.g;
    res.b = on_vec.b - off_vec.b;
    //res.a = on_vec.a - off_vec.a;
    res.a = .5;

    //gl_FragColor = texture2D(res,tcoord);
    gl_FragColor = clamp(res,vec4(0),vec4(1));
}
