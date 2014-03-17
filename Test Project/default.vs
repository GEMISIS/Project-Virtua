#version 330

varying vec2 texture_coordinate;

in vec3 vertexPosition_modelspace;
uniform mat4 translation;

layout(location = 1) in vec2 texCoords;

out vec2 fragTexCoords;

void main(){
    // Output position of the vertex, in clip space : translation * position
    vec4 v = vec4(vertexPosition_modelspace,1); // Transform an homogeneous 4D vector, remember ?
    gl_Position = translation * v;

	fragTexCoords = texCoords;
}

