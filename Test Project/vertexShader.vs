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
    vec4 v = rotMatrix * vec4(vertexPosition, 1);
	mat4 perspective = perspTranslation * perspCore;
    gl_Position = perspective * viewTranslation * v;

	fragColor = vec4(vertexColor, 1);
	fragTexCoords = texCoords;
}

