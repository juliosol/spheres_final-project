// ==================================================================
#version 330 core
// Read in our attributes stored from our vertex buffer object
// We explicitly state which is the vertex information
// (The first 3 floats are positional data, we are putting in our vector)
layout(location=0)in vec2 position; 
layout(location=1)in vec2 texCoord; // Our third attribute - texture coordinates.

// Export our Fragment Position computed in world space
out vec2 v_texCoord;

void main()
{
    v_texCoord = texCoord;
    gl_Position = vec4(position, 0.0, 1.0);
}
// ==================================================================
