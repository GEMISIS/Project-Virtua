#version 150

uniform mat4 translation;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 texCoords;

out vec4 fragColor;
out vec2 fragTexCoords;

void main()
{
    vec4 v = vec4(vertexPosition, 1);
    gl_Position = v;

	fragColor = vec4(vertexColor, 1);
	fragTexCoords = texCoords;
}

