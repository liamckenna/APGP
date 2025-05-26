#version 330 core

uniform vec3 camera_position;
uniform vec3 light_position;
uniform int shading_mode;
uniform float light_strength;
uniform int phong_exponent;
uniform vec4 triangle_colors[25];
uniform vec3 triangle_normals[25];

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 frag_position[];
in vec3 vertex_normal[];

out vec3 geom_position;
out vec3 geom_normal;
out vec4 face_color;


void main() {

    for (int i = 0; i < 3; ++i) {

        if (shading_mode == 1) { //1 = flat shading
            
            geom_normal = vertex_normal[0];
            face_color = triangle_colors[gl_PrimitiveIDIn];
            geom_position = frag_position[i];

            vec3 ambient = vec3(0.3);
            vec3 diffuse = vec3(1.0);
            vec3 specular = vec3(1.0);

            float adjustedStrength = light_strength / 100.0;

            float distance = length(light_position - geom_position);
            float attenuation = adjustedStrength / (distance * distance);

            vec3 lightDir = normalize(light_position - geom_position);
            vec3 viewDir = normalize(camera_position - geom_position);
            vec3 norm = normalize(geom_normal);

            float diff = max(dot(norm, lightDir), 0.0);
            diffuse = diff * vec3(1.0);
            diffuse *= attenuation;

            vec3 lighting = ambient + diffuse;

            vec3 finalColor = face_color.rgb * lighting;

            finalColor = min(finalColor, vec3(1.0));

            face_color = vec4(finalColor, face_color.a);

        } else if (shading_mode == 2) { //2 = smooth shading (pass data on to fs)
            
            geom_normal = vertex_normal[0];
            face_color = triangle_colors[gl_PrimitiveIDIn];
            geom_position = frag_position[i];

        } else if (shading_mode == 3) {

            geom_normal = vertex_normal[0];
            face_color = triangle_colors[gl_PrimitiveIDIn];
            geom_position = frag_position[i];

        } else if (shading_mode == 4) {

            geom_normal = triangle_normals[gl_PrimitiveIDIn];
            face_color = triangle_colors[gl_PrimitiveIDIn];
            geom_position = frag_position[i];

        }

        gl_Position = gl_in[i].gl_Position;
        EmitVertex();

    }

    EndPrimitive();

}
