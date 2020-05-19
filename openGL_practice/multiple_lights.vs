#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;  // normal vector is needed for the calculation of the diffuse lighting (the dot product of the normal vector and the light direction)
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
    // Good to know: Although we can also do lighting in the vertex shader since it is a lot more efficient due to the reason that there are generally a lot less vertixes than fragments so the expensive lighting calculations are done less frequently. However, the resulting color value in the vertex shader is the resulting color of that vertex only and the color values of that surrounding fragments are not affected. Thus, the result is less realistic unless large amount of vertices are considered.
    
    FragPos = vec3(model * vec4(aPos, 1.0)); // We need the fragment position value (FragPos) in world space in order to calculate the diffuse lighting. The way to get the fragment position in world space is to multiple it with the model matrix. Note: the model matrix is a 4x4 matrix due to the w coordinate value.
    Normal = mat3(transpose(inverse(model))) * aNormal; // we also need to convert the normal vector into the world space for the calculation of diffuse lighting. However, to prevent any un-uniformaly scaling that will distorts the perpendicular property of the normal vector, we need to use normal matrix (the transpose of the inverse of the upper-left corner(3x3) of the model matrix). Lastly, we also cast the matrix to a 3x3 matrix to ensure that it removes its translation propety (homogeneous coordinate) since the normal vector is a direction vector(w = 0) and translation has no effect on a direction vector.
    TexCoords = aTexCoords;
    
    // projection matrix tranforms coordinates with a specified range to normalized device coordinate between -1.0 and 1.0
    // projection matrix projects 3D coordinates to 2D normalized device coordinates after the perspective division is performed to divide x, y ,z axis by vector's homogeneous w component.
    gl_Position = projection * view * vec4(FragPos, 1.0);   // After the resulting vertex is assigned to gl_Position the perspective division and clipping will be performed automatically, and each resulting coordinate should be between -1.0f and 1.0f
}
