varying vec2 tcoord;
uniform sampler2D tex4;

void main(void) 
{
    gl_FragColor = texture2D(tex4,tcoord);
}
