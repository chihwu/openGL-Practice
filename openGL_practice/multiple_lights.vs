#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// IMPORTANT CONCEPT: at the end of each vertex shader run, OpenGL expects the coordinates to be within a specific range and any coordinate that falls outside this range is clipped and that is also when we enter the clip space. So at the end of the vertex shader call, we have applied model, view, and projection matrix.
void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    
    // projection matrix tranforms coordinates with a specified range to normalized device coordinate between -1.0 and 1.0
    // projection matrix projects 3D coordinates to 2D normalized device coordinates after the perspective division is performed to divide x, y ,z axis by vector's homogeneous w component.
    gl_Position = projection * view * vec4(FragPos, 1.0);   // After the resulting vertex is assigned to gl_Position the perspective division and clipping will be performed automatically, and each resulting coordinate should be between -1.0f and 1.0f
}
