#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord; // Value between 0 and 1 in x and y Direction
in vec4 fragColor; // Given Color by DrawTexture

// Input uniform values
uniform sampler2D texture0; // Given Texture by DrawTexture
uniform vec4 colDiffuse; //WHITE

uniform vec4 flash_color;

uniform float flash_alpha;

// Output fragment color
out vec4 finalColor;

void main() {
    vec4 texture_color = texture(texture0, fragTexCoord);

    if(texture_color.a > 0.0){
        finalColor = mix(texture_color * colDiffuse * fragColor, flash_color / 255, flash_alpha);
    }
    else{
        finalColor = vec4(0.0,0.0,0.0,0.0);
    }
}
