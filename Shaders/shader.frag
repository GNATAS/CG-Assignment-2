#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 textureCoordinate;

out vec4 colour;

uniform sampler2D diffuseTexture;
uniform bool useTexture;
uniform vec3 objectColour;
uniform float emissiveStrength;
uniform float specularStrength;
uniform float shininess;

uniform vec3 viewPosition;
uniform vec3 ambientColour;
uniform vec3 lightPositions[3];
uniform vec3 lightColours[3];
uniform float lightIntensities[3];

void main()
{
    vec3 baseColour = useTexture
        ? texture(diffuseTexture, textureCoordinate).rgb
        : objectColour;
    vec3 normal = normalize(worldNormal);
    vec3 viewDirection = normalize(viewPosition - worldPosition);
    vec3 result = baseColour * ambientColour;

    for (int i = 0; i < 3; ++i)
    {
        vec3 lightOffset = lightPositions[i] - worldPosition;
        float distanceToLight = length(lightOffset);
        vec3 lightDirection = lightOffset / max(distanceToLight, 0.0001);
        float attenuation = 1.0 /
            (1.0 + 0.32 * distanceToLight + 0.20 * distanceToLight * distanceToLight);
        vec3 radiance = lightColours[i] * lightIntensities[i] * attenuation;

        float diffuse = max(dot(normal, lightDirection), 0.0);
        vec3 halfwayDirection = normalize(lightDirection + viewDirection);
        float specular = pow(max(dot(normal, halfwayDirection), 0.0), shininess)
                       * specularStrength;
        result += baseColour * radiance * diffuse + radiance * specular;
    }

    result += baseColour * emissiveStrength;
    colour = vec4(result, 1.0);
}
