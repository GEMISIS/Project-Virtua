#version 150

uniform mat4 mvp;

in vec3 vertexPosition;
in vec3 vertexColor;
in vec2 texCoords;

out vec4 gl_Position;
out vec4 fragColor;
out vec2 fragTexCoords;

void main()
{
    vec4 v = vec4(vertexPosition, 1);
    gl_Position = mvp * v;

	fragColor = vec4(vertexColor, 1);
	fragTexCoords = texCoords;
}

