#version 450 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform int screenWidth;
uniform int screenHeight;

float EDGE_THRESHOLD_MIN = 0.0312;
float EDGE_THRESHOLD_MAX = 0.125;
int ITERATIONS = 12;
float QUALITY[12] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0};
float SUBPIXEL_QUALITY = 0.75;

float rgb2luma(vec3 rgb)
{
    return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

vec3 textureBilinear(sampler2D tex, vec2 uv, vec2 texSize)
{
    vec2 texelCoord = uv * texSize - 0.5;

    vec2 base = floor(texelCoord);
    vec2 f    = fract(texelCoord);

    vec2 uv00 = (base + vec2(0.5, 0.5)) / texSize;
    vec2 uv10 = (base + vec2(1.5, 0.5)) / texSize;
    vec2 uv01 = (base + vec2(0.5, 1.5)) / texSize;
    vec2 uv11 = (base + vec2(1.5, 1.5)) / texSize;

    vec3 c00 = texture(tex, uv00).rgb;
    vec3 c10 = texture(tex, uv10).rgb;
    vec3 c01 = texture(tex, uv01).rgb;
    vec3 c11 = texture(tex, uv11).rgb;

    vec3 row0 = mix(c00, c10, f.x);
    vec3 row1 = mix(c01, c11, f.x);

    return mix(row0, row1, f.y);
}

void main() {
    vec3 colorCenter = texture(screenTexture, TexCoord).rgb;

    float lumaCenter = rgb2luma(colorCenter);

    vec2 inverseScreenSize = {1.0/screenWidth, 1.0/screenHeight};

    float lumaDown = rgb2luma(textureOffset(screenTexture, TexCoord, ivec2(0, -1)).rgb);
    float lumaUp = rgb2luma(textureOffset(screenTexture, TexCoord, ivec2(0, 1)).rgb);
    float lumaLeft = rgb2luma(textureOffset(screenTexture, TexCoord, ivec2(-1, 0)).rgb);
    float lumaRight = rgb2luma(textureOffset(screenTexture, TexCoord, ivec2(1, 0)).rgb);

    float lumaMin = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
    float lumaMax = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));

    float lumaRange = lumaMax - lumaMin;

    if(lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX))
    {
        FragColor = vec4(colorCenter, 1.0);
        return;
    }

    float lumaDownLeft = rgb2luma(textureOffset(screenTexture,TexCoord,ivec2(-1,-1)).rgb);
    float lumaUpRight = rgb2luma(textureOffset(screenTexture,TexCoord,ivec2(1,1)).rgb);
    float lumaUpLeft = rgb2luma(textureOffset(screenTexture,TexCoord,ivec2(-1,1)).rgb);
    float lumaDownRight = rgb2luma(textureOffset(screenTexture,TexCoord,ivec2(1,-1)).rgb);

    float lumaDownUp = lumaDown + lumaUp;
    float lumaLeftRight = lumaLeft + lumaRight;

    float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
    float lumaDownCorners = lumaDownLeft + lumaDownRight;
    float lumaRightCorners = lumaDownRight + lumaUpRight;
    float lumaUpCorners = lumaUpRight + lumaUpLeft;

    float edgeHorizontal =  abs(-2.0 * lumaLeft + lumaLeftCorners)  + abs(-2.0 * lumaCenter + lumaDownUp ) * 2.0    + abs(-2.0 * lumaRight + lumaRightCorners);
    float edgeVertical =    abs(-2.0 * lumaUp + lumaUpCorners)      + abs(-2.0 * lumaCenter + lumaLeftRight) * 2.0  + abs(-2.0 * lumaDown + lumaDownCorners);

    bool isHorizontal = (edgeHorizontal >= edgeVertical);

    float luma1 = isHorizontal ? lumaDown : lumaLeft;
    float luma2 = isHorizontal ? lumaUp : lumaRight;

    float gradient1 = luma1 - lumaCenter;
    float gradient2 = luma2 - lumaCenter;

    bool is1Steepest = abs(gradient1) >= abs(gradient2);

    float gradientScaled = 0.25*max(abs(gradient1), abs(gradient2));

    float stepLength = isHorizontal ? inverseScreenSize.y : inverseScreenSize.x;

    float lumaLocalAverage = 0.0;

    if (is1Steepest)
    {
        stepLength *= -1.0;
        lumaLocalAverage = 0.5 * (luma1 + lumaCenter);
    }
    else lumaLocalAverage = 0.5 * (luma2 + lumaCenter);

    vec2 currentUV = TexCoord;

    if (isHorizontal) currentUV.y += stepLength * 0.5;
    else              currentUV.x += stepLength * 0.5;

    vec2 offset = isHorizontal ? vec2(inverseScreenSize.x, 0.0) : vec2(0.0, inverseScreenSize.y);

    vec2 uv1 = currentUV - offset;
    vec2 uv2 = currentUV + offset;

    vec2 texSize = vec2(screenWidth, screenHeight);

    float lumaEnd1 = rgb2luma(textureBilinear(screenTexture, uv1, texSize));
    float lumaEnd2 = rgb2luma(textureBilinear(screenTexture, uv2, texSize));
    lumaEnd1 -= lumaLocalAverage;
    lumaEnd2 -= lumaLocalAverage;

    bool reached1 = abs(lumaEnd1) >= gradientScaled;
    bool reached2 = abs(lumaEnd2) >= gradientScaled;
    bool reachedBoth = reached1 && reached2;

    if (!reached1) uv1 -= offset;
    if (!reached2) uv2 += offset;

    if (!reachedBoth)
    {
        for (int i = 2; i < ITERATIONS; i++)
        {
            if (!reached1)
            {
                lumaEnd1 = rgb2luma(textureBilinear(screenTexture, uv1, texSize));
                lumaEnd1 -= lumaLocalAverage;
            }
            if (!reached2)
            {
                lumaEnd2 = rgb2luma(textureBilinear(screenTexture, uv2, texSize));
                lumaEnd2 -= lumaLocalAverage;
            }

            reached1 = abs(lumaEnd1) >= gradientScaled;
            reached2 = abs(lumaEnd2) >= gradientScaled;
            reachedBoth = reached1 && reached2;

            if (!reached1) uv1 -= offset * QUALITY[i];
            if (!reached2) uv2 += offset * QUALITY[i];

            if (reachedBoth) break;
        }
    }

    float distance1 = isHorizontal ? (TexCoord.x - uv1.x) : (TexCoord.y - uv1.y);
    float distance2 = isHorizontal ? (uv2.x - TexCoord.x) : (uv2.y - TexCoord.y);

    bool isDirection1 = distance1 < distance2;
    float distanceFinal = min(distance1, distance2);

    float edgeLength = distance1 + distance2;

    float pixelOffset = -1.0 * distanceFinal / edgeLength + 0.5;

    bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;

    bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;

    float finalOffset = correctVariation ? pixelOffset : 0.0;

    float lumaAverage = (1.0/12.0) * (2.0 * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);

    float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter) / lumaRange, 0.0, 1.0);
    float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
    float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;

    finalOffset = max(finalOffset, subPixelOffsetFinal);

    vec2 finalUV = TexCoord;

    if (isHorizontal)   finalUV.y += finalOffset * stepLength;
    else                finalUV.x += finalOffset * stepLength;


    vec3 finalColor = textureBilinear(screenTexture, finalUV, texSize);

    FragColor = vec4(finalColor, 1.0);
}
