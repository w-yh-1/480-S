#version 330 core
in vec2 outCoord;
out vec4 FragColor;
uniform sampler2D texture;
void main(void)
{
    FragColor = texture2D(texture,outCoord);
}
