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
uniform bool cityLighting;

uniform vec3 viewPosition;
uniform vec3 ambientColour;
uniform vec3 lightPositions[4];
uniform vec3 lightColours[4];
uniform float lightIntensities[4];

void main()
{
    vec3 baseColour = useTexture
        ? texture(diffuseTexture, textureCoordinate).rgb
        : objectColour;
    vec3 normal = normalize(worldNormal);
    vec3 viewDirection = normalize(viewPosition - worldPosition);
    vec3 result = baseColour * ambientColour;

    for (int i = 0; i < 4; ++i)
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
    if (cityLighting)
    {
        // City-only moonlight and window illumination. Atlas alpha identifies
        // glass from the imported facade; no freestanding window rectangles.
        float moon = max(dot(normal, normalize(vec3(-.4,.8,-.6))), 0.0);
        result = baseColour * vec3(.30,.36,.50) * (.58 + .34 * moon) + vec3(.012,.018,.035);
        float glass = texture(diffuseTexture, textureCoordinate).a;
        // Window bays in the facade material, with dark mullions between them.
        vec2 facadeUV = abs(normal.x)>abs(normal.z) ? worldPosition.zy : worldPosition.xy;
        vec2 bay = fract(facadeUV*vec2(1.30,.90));
        glass *= step(.15,bay.x)*step(bay.x,.85)*step(.14,bay.y)*step(bay.y,.84);
        glass *= 1.0-step(.45,abs(normal.y));
        vec3 cell = floor(worldPosition * vec3(1.30,.90,1.30));
        float hash = fract(sin(dot(cell,vec3(12.9898,78.233,37.719))) * 43758.5453);
        vec3 pane = hash > .60 ? (hash > .90 ? vec3(.46,.72,.92) : vec3(.95,.66,.32))
                              : vec3(.030,.055,.095);
        result = mix(result, pane, glass);
        float haze = clamp((length(viewPosition-worldPosition)-25.0)/175.0,0.0,.34);
        result = mix(result,vec3(.035,.050,.082),haze);
    }
    colour = vec4(result, 1.0);
}
