#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// ── Lanterna (Spotlight) ──────────────────────────────────────────
struct Flashlight {
    vec3  position;
    vec3  direction;
    float innerCutOff;  // cos do ângulo interno
    float outerCutOff;  // cos do ângulo externo (penumbra)
    float constant;
    float linear;
    float quadratic;
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    bool  on;
};

// ── Luz interna da guarita ────────────────────────────────────────
struct PointLight {
    vec3  position;
    vec3  color;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 4
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

// ── Luz ambiente da cena (luar fraco) ────────────────────────────
struct AmbientLight {
    vec3 color;
    float intensity;
};

// ── Névoa ─────────────────────────────────────────────────────────
uniform float fogDensity;
uniform vec3  fogColor;

uniform Flashlight   flashlight;
uniform AmbientLight ambientLight;
uniform vec3         viewPos;
uniform sampler2D    texture_diffuse;

void main() {
    vec3 texColor = texture(texture_diffuse, TexCoord).rgb;
    vec3 norm     = normalize(Normal);
    vec3 viewDir  = normalize(viewPos - FragPos);

    // ── Luz ambiente da noite ──────────────────────────────────────
    vec3 result = ambientLight.color * ambientLight.intensity * texColor;

    // ── Lanterna ───────────────────────────────────────────────────
    if (flashlight.on) {
        vec3  lightDir = normalize(flashlight.position - FragPos);
        float theta    = dot(lightDir, normalize(-flashlight.direction));
        float epsilon  = flashlight.innerCutOff - flashlight.outerCutOff;
        float intensity = clamp((theta - flashlight.outerCutOff) / epsilon, 0.0, 1.0);

        // Atenuação por distância
        float dist        = length(flashlight.position - FragPos);
        float attenuation = 1.0 / (flashlight.constant
                                 + flashlight.linear    * dist
                                 + flashlight.quadratic * dist * dist);

        // Diffuse
        float diff    = max(dot(norm, lightDir), 0.0);
        vec3  diffuse = flashlight.diffuse * diff * texColor;

        // Specular
        vec3  reflectDir = reflect(-lightDir, norm);
        float spec       = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3  specular   = flashlight.specular * spec;

        result += (diffuse + specular) * intensity * attenuation;
    }

    // ── Luzes pontuais do teto (lâmpadas) ──────────────────────────
    for (int i = 0; i < pointLightCount; i++) {
        float d       = length(pointLights[i].position - FragPos);
        float att     = 1.0 / (pointLights[i].constant
                             + pointLights[i].linear    * d
                             + pointLights[i].quadratic * d * d);
        float diff    = max(dot(norm, normalize(pointLights[i].position - FragPos)), 0.0);
        result       += pointLights[i].color * diff * att * texColor;
    }

    // ── Névoa (exponencial) ───────────────────────────────────────
    float fogDist = length(viewPos - FragPos);
    float fogFactor = exp(-fogDensity * fogDensity * fogDist * fogDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    result = mix(fogColor, result, fogFactor);

    // Tom escuro da noite
    vec3 mapped = result / (result + vec3(1.0)); // Reinhard tone mapping
    FragColor = vec4(mapped, 1.0);

}
