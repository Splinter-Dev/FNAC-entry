#include <string>
#include <raylib.h>
#include "light.hh"
#include "shader.hh"

Shader toonShader;
int ambientLoc;
int viewPosLoc;

const char * toon_vs = R"(
#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

// NOTE: Add your custom variables here

void main()
{
    // Send vertex attributes to fragment shader
    fragPosition = vec3(matModel*vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    fragNormal = normalize(vec3(matNormal*vec4(vertexNormal, 1.0)));

    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}
)";

const char * toon_fs = R"(
#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
out vec4 finalColor;

#define MAX_LIGHTS 4
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1

struct Light {
    int enabled;
    int type;       // 0 = directional, 1 = point
    vec3 position;
    vec3 target;    // only for directional lights
    vec4 color;     // ignored for pure-color toon shading
    vec3 intensity; // scalar intensity in .r channel
    vec3 radius;    // only used for point lights
};

uniform Light lights[MAX_LIGHTS];
uniform vec4 ambient;
uniform vec3 viewPos;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPosition);

    // Base color from texture, diffuse, and vertex color
    vec3 baseColor = texelColor.rgb * colDiffuse.rgb * fragColor.rgb;

    // Ambient scalar (single channel)
    float ambientIntensity = ambient.r;
    vec3 ambientTerm = baseColor * ambientIntensity;

    vec3 diffuseTerm = vec3(0.0);
    vec3 specularTerm = vec3(0.0);

    const int steps = 3; // Toon shading steps

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].enabled == 1)
        {
            vec3 lightDir;
            float attenuation = 1.0;

            if (lights[i].type == LIGHT_DIRECTIONAL)
            {
                // Directional light: get direction from target
                lightDir = -normalize(lights[i].target - lights[i].position);
            }
            else
            {
                // Point light: direction + distance attenuation
                vec3 L = lights[i].position - fragPosition;
                float distance = length(L);
                lightDir = normalize(L);

                float radiusLen = length(lights[i].radius);
                attenuation = clamp(1.0 - distance / radiusLen, 0.0, 1.0);
            }

            float NdotL = max(dot(normal, lightDir), 0.0);

            // Toon diffuse quantization
            float toonDiffuse = floor(NdotL * float(steps)) / float(steps);

            // Scalar light intensity (preserve object color)
            // float diffuseIntensity = toonDiffuse * attenuation * lights[i].intensity.r;
            // diffuseTerm += baseColor * diffuseIntensity;
            vec3 lightCol = lights[i].color.rgb * lights[i].intensity; // multiply by intensity
            float diffuseIntensity = toonDiffuse * attenuation;
            diffuseTerm += baseColor * diffuseIntensity * lightCol;


            // Specular highlights (white)
            if (NdotL > 0.0)
            {
                float spec = pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), 26.0);
                spec = (spec > 0.5) ? 1.0 : 0.0;
                specularTerm += vec3(spec * 0.1 * attenuation); // add brightness equally to all channels
            }
        }
    }

    vec3 finalColorRGB = clamp(ambientTerm + diffuseTerm + specularTerm, 0.0, 1.0);
    // finalColor = vec4(pow(finalColorRGB, vec3(1.0 / 2.2)), texelColor.a);
    finalColor = vec4(clamp(finalColorRGB, 0.0, 1.0), texelColor.a);
}
)";

void initShaders() {
    toonShader = LoadShaderFromMemory(toon_vs, toon_fs);

    // Get some required shader locations
    toonShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(toonShader, "viewPos");

    // Tell raylib which uniform in the shader corresponds to the diffuse texture
    toonShader.locs[SHADER_LOC_MAP_DIFFUSE] = GetShaderLocation(toonShader, "texture0");

    ambientLoc = GetShaderLocation(toonShader, "ambient");
    viewPosLoc = GetShaderLocation(toonShader, "viewPos");

    Vector4 defaultAmbient = DEFAULT_AMBIENT;
    Vector3 defaultViewPos = DEFAULT_VIEW_POS;

    SetShaderValue(toonShader, ambientLoc, &defaultAmbient, SHADER_UNIFORM_VEC4);
    SetShaderValue(toonShader, viewPosLoc, &defaultViewPos, SHADER_UNIFORM_VEC3);
}

// Send a single light to the shader
static void sendLight(Shader &shader, int index, const Light &light) {
    std::string prefix = "lights[" + std::to_string(index) + "].";
    int enabled = light.enabled ? 1 : 0;
    int type = light.type;
    Vector3 intensity = Vector3{ light.intensity, light.intensity, light.intensity };
    Vector3 radius = Vector3{ light.radius, light.radius, light.radius };

    // Convert Color to float
    Vector4 colorF = { light.color.r/255.0f, light.color.g/255.0f, light.color.b/255.0f, light.color.a/255.0f };

    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "enabled").c_str()), &enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "type").c_str()), &type, SHADER_UNIFORM_INT);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "position").c_str()), &light.position, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "target").c_str()), &light.target, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "color").c_str()), &colorF, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "intensity").c_str()), &intensity, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, GetShaderLocation(shader, (prefix + "radius").c_str()), &radius, SHADER_UNIFORM_VEC3);
}

// Send all lights to the shader
void sendLightsToShader(Shader &shader, const std::vector<Light> &lights) {
    for (int i = 0; i < (int)lights.size(); i++) {
        sendLight(shader, i, lights[i]);
    }
}
