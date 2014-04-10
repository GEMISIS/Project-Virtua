#version 150

uniform mat4 perspCore;
uniform mat4 perspTranslation;
uniform mat4 viewTranslation;
uniform mat4 rotMatrix;

in vec3 vertexPosition;
in vec3 vertexColor;
in vec2 texCoords;

out vec4 gl_Position;
out vec4 fragColor;
out vec2 fragTexCoords;

void main()
{
	mat4 view = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	mat4 mvp = (perspCore * perspTranslation) * (view * viewTranslation) * (rotMatrix);
    vec4 v = vec4(vertexPosition, 1);
    gl_Position = mvp * v;

	fragColor = vec4(vertexColor, 1);
	fragTexCoords = texCoords;
}

