#version 330 core
layout(location=0)in vec3 position;
layout(location=1)in vec3 normals;
layout(location=2)in vec2 texCoord;
layout(location=3)in vec3 tangents;
layout(location=4)in vec3 bitangents;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos; // Fragment position computed in world space
out vec3 myNormal; // Export normal data and read it into frag shader
out vec2 v_texCoord; // Export texture coordinates if we have them

void main(){
    FragPos = vec3(model * vec4(position, 1.0f));

    gl_Position = projection * view * vec4(FragPos, 1.0);

    myNormal = mat3(transpose(inverse(model))) * normals; // normals;

    v_texCoord = texCoord;
}