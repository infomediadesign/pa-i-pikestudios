#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord; // Value between 0 and 1 in x and y Direction
in vec4 fragColor; // Given Color by DrawTexture

// Input uniform values
uniform sampler2D texture0; // Given Texture by DrawTexture
uniform vec4 colDiffuse; //WHITE

uniform vec4 outline_color;

uniform vec2 texture_size;

// Output fragment color
out vec4 finalColor;

const float min_opacity = 0.0001;

void main() {
    vec4 texture_color = texture(texture0, fragTexCoord);

    if(texture_color.a < min_opacity){
            vec2 offset = vec2(1.0 / texture_size.x, 1.0 / texture_size.y);

            float left = texture(texture0, vec2(fragTexCoord.x - offset.x, fragTexCoord.y)).a;
            float right = texture(texture0, vec2(fragTexCoord.x + offset.x, fragTexCoord.y)).a;
            float up = texture(texture0, vec2(fragTexCoord.x, fragTexCoord.y - offset.y)).a;
            float down = texture(texture0, vec2(fragTexCoord.x, fragTexCoord.y + offset.y)).a;

            float alpha = step(min_opacity, left + right + up + down);

            finalColor = mix(texture_color, outline_color / 255, alpha);
        }
        else{
            finalColor = texture_color * colDiffuse * fragColor;
        }
}
