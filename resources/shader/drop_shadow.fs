#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;      // Diffuse texture (bound automatically by raylib)
uniform vec4 colDiffuse;         // WHITE (raylib built-in)

// Shadow parameters
uniform vec4 shadow_color;       // Shadow tint color with alpha (e.g. vec4(0.0, 0.0, 0.05, 0.4))

// Output fragment color
out vec4 finalColor;

void main()
{
    // Sample the diffuse texture to get the alpha (silhouette shape)
    vec4 texColor = texture(texture0, fragTexCoord) * colDiffuse * fragColor;

    // Discard fully transparent pixels so the shadow follows the sprite shape
    if (texColor.a < 0.01)
        discard;

    // Output a solid shadow color using the sprite's alpha as mask
    // The shadow is just the silhouette of the sprite filled with the shadow color
    finalColor = vec4(shadow_color.rgb, texColor.a * shadow_color.a);
}
