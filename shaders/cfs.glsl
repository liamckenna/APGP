#version 430 core

in vec2 tex_coords;

uniform sampler2D accum_color_tex; // Accumulated color texture
uniform sampler2D accum_alpha_tex; // Accumulated alpha texture

out vec4 FragColor;

void main() {
    vec3 accum_color = texture(accum_color_tex, tex_coords).rgb;
    float accum_alpha = texture(accum_alpha_tex, tex_coords).r;

    //vec3 color = texture(accum_color_tex, tex_coords).rgb;
    //FragColor = vec4(color, 1.0); // Directly render accum_color_tex
    //float alpha = texture(accum_alpha_tex, tex_coords).r;
    //FragColor = vec4(alpha, alpha, alpha, 1.0); // Directly render accum_color_tex
    //FragColor = vec4(tex_coords, 0.0, 1.0); // Gradient visualization
    // Normalize the accumulated color
    vec3 final_color = accum_color / max(accum_alpha, 1e-5);
    FragColor = vec4(final_color, 1.0);
    //FragColor = vec4(final_color, accum_alpha);
}
