#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord; // Value between 0 and 1 in x and y Direction
in vec4 fragColor; // Given Color by DrawTexture

// Input uniform values
uniform sampler2D texture0; // Given Texture by DrawTexture
uniform vec4 colDiffuse; //WHITE

// Output fragment color
out vec4 finalColor;

uniform vec2 size;

uniform float time;

uniform vec2 main_freq;
uniform vec2 main_amp;
uniform vec2 main_vel;
uniform vec2 sub_freq;
uniform vec2 sub_amp;
uniform vec2 sub_vel;

void main() {
    float pixelWidth = 1.0/size.x;
    float pixelHeight = 1.0/size.y;
    float boxLeft = 0.0;
    float boxTop = 0.0;

    vec2 p = fragTexCoord;

    p.x += (cos((fragTexCoord.y - boxTop) * main_freq.x * pixelWidth + (time * main_vel.x)) * main_amp.x * pixelWidth) + (cos((fragTexCoord.y - boxTop) * sub_freq.x * pixelWidth + (time * sub_vel.x)) * sub_amp.x * pixelWidth);
    p.y += (sin((fragTexCoord.x - boxLeft) * main_freq.y * pixelHeight + (time * main_vel.y)) * main_amp.y * pixelHeight) + (sin((fragTexCoord.x - boxLeft) * sub_freq.y * pixelHeight + (time * sub_vel.y)) * sub_amp.y * pixelHeight);

    finalColor = texture(texture0, p) * colDiffuse * fragColor;
}
