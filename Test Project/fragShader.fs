#version 150

uniform sampler2D sampler0;
in vec4 fragColor;
in vec2 fragTexCoords;

out vec4 color;

void main()
{
    color = fragColor;
}

