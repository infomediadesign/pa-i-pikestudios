#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord; // Value between 0 and 1 in x and y Direction
in vec4 fragColor; // Given Color by DrawTexture

// Input uniform values
uniform sampler2D texture0; // Given Texture by DrawTexture
uniform sampler2D texture1; // Normal map (bind manually via SetShaderValueTexture)
uniform vec4 colDiffuse; //WHITE

// Output fragment color
out vec4 finalColor;

// Wave parameters
uniform vec2 size;

uniform float time;

uniform vec2 main_freq;
uniform vec2 main_amp;
uniform vec2 main_vel;
uniform vec2 sub_freq;
uniform vec2 sub_amp;
uniform vec2 sub_vel;

// Sun / directional light parameters
uniform vec3 sun_direction;
uniform vec3 sun_color;
uniform float sun_intensity;

// Ambient light
uniform vec3 ambient_color;
uniform float ambient_intensity;

// Specular highlight
uniform float spec_intensity;
uniform float spec_power;

// Toggle for normal map lighting
uniform int use_lighting;

void main() {
    float pixelWidth = 1.0/size.x;
    float pixelHeight = 1.0/size.y;
    float boxLeft = 0.0;
    float boxTop = 0.0;

    vec2 p = fragTexCoord;

    p.x += (cos((fragTexCoord.y - boxTop) * main_freq.x /  pixelWidth + (time * main_vel.x)) * main_amp.x * pixelWidth) + (cos((fragTexCoord.y - boxTop) * sub_freq.x / pixelWidth + (time * sub_vel.x)) * sub_amp.x * pixelWidth);
    p.y += (sin((fragTexCoord.x - boxLeft) * main_freq.y / pixelHeight + (time * main_vel.y)) * main_amp.y * pixelHeight) + (sin((fragTexCoord.x - boxLeft) * sub_freq.y / pixelHeight + (time * sub_vel.y)) * sub_amp.y * pixelHeight);

    vec4 texColor = texture(texture0, p) * colDiffuse * fragColor;

    if (use_lighting > 0 && texColor.a > 0.01) {
        // Sample normal map with the same distorted UVs so lighting follows the waves
        vec3 normal = texture(texture1, p).rgb * 2.0 - 1.0;
        normal.y = -normal.y; // Flip Y: Laigther uses OpenGL convention (+Y up), screen space is +Y down
        normal = normalize(normal);

        // Diffuse lighting
        vec3 lightDir = normalize(sun_direction);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * sun_color * sun_intensity;

        // Specular lighting (Phong)
        vec3 viewDir = vec3(0.0, 0.0, 1.0);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), max(spec_power, 1.0));
        vec3 specular = spec * sun_color * spec_intensity;

        // Ambient lighting
        vec3 ambient = ambient_color * ambient_intensity;

        // Combine
        vec3 result = texColor.rgb * (diffuse + ambient) + specular;
        finalColor = vec4(result, texColor.a);
    } else {
        finalColor = texColor;
    }
}
