#version 330 core

uniform vec3 camera_position;
uniform vec3 light_position;
uniform int shading_mode;      //1 = Flat, 2 = Smooth
uniform float light_strength;
uniform int phong_exponent;
uniform vec4 triangle_colors[25];  //Triangle colors for flat shading
uniform vec3 triangle_normals[25]; //Triangle normals for flat shading

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 frag_position[];
in vec3 vertex_normal[];

out vec3 geom_position;
out vec3 geom_normal;
out vec4 face_color;  //Pass through the color for flat shading


void main() {
    for (int i = 0; i < 3; ++i) {
        if (shading_mode == 1) {
            //Flat shading: Use the normal and color of the triangle as a whole
            geom_normal = vertex_normal[0];
            face_color = triangle_colors[gl_PrimitiveIDIn];
            geom_position = frag_position[i];

            //Smooth shading (Phong shading)

            //Initialize lighting components
            vec3 ambient = vec3(0.3);  //Ambient lighting strength
            vec3 diffuse = vec3(1.0);  //Diffuse component (Lambertian reflection)
            vec3 specular = vec3(1.0); //Specular component (Phong reflection)

            //Adjust light strength
            float adjustedStrength = light_strength / 100.0;

            //Calculate distance attenuation
            float distance = length(light_position - geom_position);
            float attenuation = adjustedStrength / (distance * distance);

            //Calculate normalized direction vectors
            vec3 lightDir = normalize(light_position - geom_position);   //From surface to light
            vec3 viewDir = normalize(camera_position - geom_position);   //From surface to camera
            vec3 norm = normalize(geom_normal);                         //Normalize surface normal

            //Lambertian reflection (Diffuse)
            float diff = max(dot(norm, lightDir), 0.0);
            diffuse = diff * vec3(1.0);  //White diffuse light
            diffuse *= attenuation;      //Apply attenuation



            //Combine ambient, diffuse, and specular components
            vec3 lighting = ambient + diffuse;

            //Final color = face color modulated by lighting
            vec3 finalColor = face_color.rgb * lighting;

            //Clamp the result to avoid exceeding 1.0
            finalColor = min(finalColor, vec3(1.0));

            //Output the final color
            face_color = vec4(finalColor, face_color.a);

        } else if (shading_mode == 2) {
            //Smooth shading: Pass interpolated per-vertex normal and position
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

        //Emit the vertex with the passed data
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}
