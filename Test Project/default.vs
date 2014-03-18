#version 150

uniform mat4 translation;

in vec2 vertexPosition;
in vec3 vertexColor;
in vec2 texCoords;

out vec4 fragColor;
out vec2 fragTexCoords;

out vec4 gl_Position;

void main()
{
    vec4 v = vec4(vertexPosition, 0, 1);
    gl_Position = translation * v;

	fragColor = vec4(vertexColor, 1);
	fragTexCoords = texCoords;
}

