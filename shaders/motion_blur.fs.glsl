#version 450 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform mat4 invViewProj;
uniform mat4 prevViewProj;
uniform int screenWidth;
uniform float deltaTime;

int SAMPLE_COUNT = 8;
float STRENGTH = 1;
float MAX_VELOCITY = 0.05;
float SHUTTER = 1.0 / 60.0;

void main() 
{
    float MIN_VELOCITY = 0.5 / float(screenWidth);
    
    float depth = texture(depthTexture, TexCoord).r;

    vec4 ndc;
    ndc.xy = TexCoord * 2.0 - 1.0;
    ndc.z  = depth * 2.0 - 1.0;
    ndc.w  = 1.0;

    vec4 worldPos = invViewProj * ndc;
    worldPos /= worldPos.w;

    vec4 prevClip = prevViewProj * vec4(worldPos.xyz, 1.0);
    prevClip /= prevClip.w;

    vec2 prevUV = prevClip.xy * 0.5 + 0.5;

    float dt = max(deltaTime, 1e-5); //guard the first frame where delta starts at 0
    vec2 velocity = (TexCoord - prevUV) * STRENGTH * (SHUTTER / dt);

    float speed = length(velocity);

    if (speed < MIN_VELOCITY)
    {
        FragColor = texture(screenTexture, TexCoord);
        return;
    }
    if (speed > MAX_VELOCITY)
    {
        velocity = velocity / speed * MAX_VELOCITY;
    }

    vec3 accumColor = vec3(0.0);
    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        float t = float(i) / float(SAMPLE_COUNT - 1);
        vec2 samplePos = TexCoord + velocity * (t - 0.5);

        accumColor += texture(screenTexture, samplePos).rgb;
    }
    vec3 avgColor = accumColor / SAMPLE_COUNT;

    FragColor = vec4(avgColor, 1.0);
}
