#version 330 core
out vec4 FragColor;

// If we want to simulate several types of objects in OpenGL we have to define material properties specific to each object. When describing objects we can define a material color for each of the 3 lighting components: ambient, diffuse, and specular lighting.
struct Material {
    // Good to note: to refer to the material color properties of real-world objects, please go to:  http://devernay.free.fr/cours/opengl/materials.html
    
    // vec3 ambient    // the ambient material vector defines what color this object reflects under ambient lighting. This is usually the same as the object's color

    // GLSL has a built-in data-type for texture objects called a sampler that takes as a postfix the texture type we want e.g. sampler1D, sampler2D, sampler3D
    sampler2D diffuse;  // the diffuse material vector defines the color of the object under diffuse lighting. The diffuse color is set to the desired object's color. Here we use diffuse map instead, which is based on the texture for diffuse lighting
    sampler2D specular;  // the specular material vector sets the color impact a specular light has on the object (or possibly even reflect an object-specific specular highlight color)
    float shininess;  // the shininess impacts the scattering/radius of the specular highlight
};

struct DirLight {
    // Directional light is when a light source is far away the light rays coming from the light source are close to parallel to each other and it is normally used as a global light. A fine example of a directional light source is the sun.
    
    vec3 direction; // since all the light rays in directional light have the same direction, we can model a directional light by defining a light direction vector instead of a position vector, which is different from the point light and spot light
    
    // light sources also have different intensities for their ambient, diffuse and specular component respectively
    vec3 ambient;  // the ambient light is usually set to a low intensity becausewe don't want the ambient color to be too dominant
    vec3 diffuse;  // the diffuse component of a light source is usually set to the exact color we'd like a light to have
    vec3 specular; // the specular component is usually kept at vec3(1.0) shining at full intensity
};

struct PointLight {
    // a point light is a light source with a given position somewhere in a world that illuminates in all directions where the light rays fade out over distance. Think of light bulbs and torches as light casters that act as a point light.
    vec3 position;
    
    // to implement attenuation(note that attenuation is applied to both the point light and spot light), we will be needing 3 extra values: the constant, linear term, and a quadratic term for the formula: F_att = 1.0 / (K_c + K_l * d + K_q * d^2)
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    // a spot light only shoots light rays in a specific direction. The result is that only the objects within a certain radius of the spotlight's direction are lit and everything else stays dark. A good example of a spotlight would be a street lamp or a flashlight. A spotlight in OpenGL is represented by a world-space position, a direction and cutoff angle that specifies the radius of the spotlight. For each fragment we calculate if the fragment is between the spotlight's cutoff directions (in its cone) and if so, we lit the fragment. (Refer page 142 in tutorial)
    // so what we need to do is to calculate the dot product between the light direction and the spot direction and compare it with the cosine value of the cutoff angle
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;  // to create an effect of a smoothly-edged spotlight we want to simulate a spotlight having an inner and an outer cone. The outer cone gradually dims the light from the inner to the edges of the outer cone.
    
    // to implement attenuation(note that attenuation is applied to both the point light and spot light), we will be needing 3 extra values: the constant, linear term, and a quadratic term for the formula: F_att = 1.0 / (K_c + K_l * d + K_q * d^2)
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);  // NOTE: the view direction currently is from fragment position to the view position
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // phase 3: spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    
    // in the resulting fragment color, for each lighting component(ambient, diffuse, specular), it is all calculated with the formula: lightColor * lightingComponent * objectColor. Note: the lighting component such as ambient, diffuse, and specular in Phong lighting model is actually like a coefficient that adjusts the product of both the light color and object color
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction); // the lighting calculation expects the light direction to be a direction from the fragment towards the light source, but people generally prefer to specify a directional light pointing from the light source. Therefore we have to negate the global light direction vector to switch its direction.
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0); // calculate the dot product of the normal vector and the light direction so the bigger the doct product the darker diffuse component is. Note: the max() ensures the value will not be smaller than 0 since lighting for negative colors is not defined
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal); // specular lighting is based on the light's direction vector and the object's normal vectors, but this time it is also based on the view direction. Specular lighting is based on the reflection properties of light. We calculate a reflection vector by reflecting the light direction around the normal vector. Then we calculate the angular distance between this reflection vector and the view direction and the closer the angle between them, the greater the impact of the specular light. The viewer position in this case will just be the camera position
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    // note: the output of the texture function is the color of a texture at the texture coordinate
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));   // to sample the color of a texture we can use GLSL's built-in texture() function     // Note: we replace the material ambient color because it in almost all cases equal to the material diffuse color so there is no need to store it separately
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal); // the reflect function expects the first vector to point from the light source towards the fragment's position, but the lightDir currently is pointing from the fragment towards the light source
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // the higher the shininess value of an object, the more it properly reflects the light instead of scattering it all around and thus the smaller the highlight becomes. For reference, see the page 122 in tutorial
    
    // attenuation
    // Good to know: In the real world, lights are generally quite bright when standing close by, but the brightness of a light source diminishes quickly at the start and the remaining light intensity more slowly diminished over distance.
    float distance = length(light.position - fragPos);  // the length() from GLSL is used to calculate the distance (|d|) between two verteces
    // IMPORTANT: the formula of calculating attenuation is F_att = 1.0 / (K_c + K_l * d + K_q * d^2)
    // d represents the distance from the fragment to the light source. K_c is a constant term, K_l is a linear term and K_q is a quadratic term.
    // The constant term(K_c) is usually kept at 1.0, which is to make sure the resulting denominator never gets smaller than 1 since it would otherwise boost the intensity with certain distances. The linear term is multiplied with the distance value that reduces the intensity in a linear fashion. The quadratic term is multiplied with the quadrant of the distance and sets a quadratic decrease of intensity for the light source. The quadratic term will be less significant compared to the linear term when the distance is small, but gets much larger than the linear term as the distance grows. Thus, the light will diminish mostly at a linear fashion until the distance becomes large enough for the quadratic term to surpass the linear term and then the light intensity will decrease a lot faster.
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos); // the length() from GLSL is used to calculate the distance (|d|) between two verteces
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    
    float theta = dot(lightDir, normalize(-light.direction)); // lightDir is the direction from fragment postion to light source while the (-light.direction) is the direction the spotlight is aiming at, which is the front of camera in this case (IMPORTANT: note that we want the vectors to point towards the light source)
    float epsilon = light.cutOff - light.outerCutOff; // the formula of calculating intensity = (theta - the outer cone) / (cosine difference between the inner and the outer cone)
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); // intensity is used to create an effect of a smoothly-edged spotlight. Thus, it is like a factor
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
