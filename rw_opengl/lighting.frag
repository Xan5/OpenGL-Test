#version 330 core
out vec4 color;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform sampler2D Texture1;
//uniform vec3 objectColor;

void main()
{
    // Ambient
    float ambientStrength = 0.30f;
    vec3 ambient = ambientStrength * vec3(texture(Texture1, TexCoord));
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * vec3(texture(Texture1, TexCoord));
    
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),32);
    vec3 specular = specularStrength * spec *  lightColor * (vec3(1.0f) - vec3(texture(Texture1, TexCoord)));
    
	color = vec4(ambient + diffuse + specular, 1.0f);  
}