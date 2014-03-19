#version 150

uniform mat4 translation;

in vec3 vertexPosition;
in vec3 vertexColor;
in vec2 texCoords;

out vec4 gl_Position;
out vec4 fragColor;
out vec2 fragTexCoords;

void main()
{
    vec4 v = vec4(vertexPosition, 1);
    gl_Position = translation * v;

	fragColor = vec4(1,0,0, 1);
	fragTexCoords = texCoords;
}

