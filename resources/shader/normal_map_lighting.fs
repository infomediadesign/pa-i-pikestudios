#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;      // Diffuse texture (bound automatically by raylib)
uniform sampler2D texture1;      // Normal map (bind manually via SetShaderValueTexture)
uniform vec4 colDiffuse;         // WHITE (raylib built-in)

// Sun / directional light parameters
uniform vec3 sun_direction;      // Normalized direction TO the sun (e.g. vec3(-0.5, -0.7, 0.6))
uniform vec3 sun_color;          // Sun color as 0-1 floats (e.g. vec3(1.0, 0.95, 0.85))
uniform float sun_intensity;     // Diffuse intensity multiplier (e.g. 1.0)

// Ambient light
uniform vec3 ambient_color;      // Ambient color as 0-1 floats (e.g. vec3(0.15, 0.12, 0.18))
uniform float ambient_intensity; // Ambient intensity multiplier (e.g. 0.3)

// Specular highlight (optional, can set intensity to 0 to disable)
uniform float spec_intensity;    // Specular intensity (e.g. 0.3, or 0.0 to disable)
uniform float spec_power;        // Specular shininess / scatter (e.g. 16.0)

// Sprite rotation in radians (so the normal map rotates with the sprite)
uniform float sprite_rotation;

// Output fragment color
out vec4 finalColor;

void main()
{
    // Sample the diffuse texture
    vec4 texColor = texture(texture0, fragTexCoord) * colDiffuse * fragColor;

    // Early discard for fully transparent pixels
    if (texColor.a < 0.01)
        discard;

        // Sample the normal map and convert from [0,1] to [-1,1]
        vec3 normal = texture(texture1, fragTexCoord).rgb * 2.0 - 1.0;
        normal.y = -normal.y;  // Flip Y: Laigther uses OpenGL convention (+Y up), screen space is +Y down
        normal = normalize(normal);

    // Rotate the normal to match sprite rotation (rotation around Z axis)
    float cosA = cos(sprite_rotation);
    float sinA = sin(sprite_rotation);
    normal = vec3(
        normal.x * cosA - normal.y * sinA,
        normal.x * sinA + normal.y * cosA,
        normal.z
    );

    // --- Diffuse lighting ---
    // sun_direction should point FROM the surface TO the sun.
    // For a sun on the left edge of the map shining right: vec3(1.0, 0.0, 0.5) normalized
    // For a sun on the top-left:                           vec3(0.5, -0.7, 0.6) normalized
    // The Z component controls how "overhead" the sun is (higher = more top-down = flatter lighting)
    vec3 lightDir = normalize(sun_direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * sun_color * sun_intensity;

    // --- Specular lighting (Phong) ---
    // View direction for 2D is straight out of the screen
    vec3 viewDir = vec3(0.0, 0.0, 1.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), spec_power);
    vec3 specular = spec * sun_color * spec_intensity;

    // --- Ambient lighting ---
    vec3 ambient = ambient_color * ambient_intensity;

    // --- Combine ---
    vec3 result = texColor.rgb * (diffuse + ambient) + specular;

    finalColor = vec4(result, texColor.a);
}
