#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord; // Value between 0 and 1 in x and y Direction
in vec4 fragColor; // Given Color by DrawTexture

// Input uniform values
uniform sampler2D texture0; // Given Texture by DrawTexture
uniform sampler2D texture_emissive; // Emissive Map of the Texture
uniform vec4 colDiffuse; // WHITE

uniform vec3 emissive_color;

// Output fragment color
out vec4 finalColor;

void main() {
    vec4 diffuse_texture = texture(texture0, fragTexCoord);

    vec4 emissive_texture = texture(texture_emissive, fragTexCoord);

    if(emissive_texture.a > 0){
        finalColor = emissive_texture * colDiffuse * vec4(emissive_color / 255, emissive_texture.a) * fragColor;
    }
    else{
        finalColor = diffuse_texture * colDiffuse * fragColor;
    }
}
