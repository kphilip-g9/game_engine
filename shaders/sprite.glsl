#vert
#version 400 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec4 a_color;

uniform vec2  u_camera_position;
uniform vec2  u_camera_offset;
uniform float u_camera_scale;
uniform vec2  u_resolution;

out vec4 v_color;
out vec2 v_uv;

void main()
{
    vec2 p = a_pos.xy;
    p -= u_camera_position;
    p *= u_camera_scale;
    p += u_camera_offset;

    vec2 ndc;
    ndc.x = (p.x / u_resolution.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (p.y / u_resolution.y) * 2.0;

    gl_Position = vec4(ndc, a_pos.z, 1.0);
    v_uv        = a_uv;
    v_color     = a_color;
}

#frag
#version 330 core

in vec4  v_color;
in vec2  v_uv;

out vec4 Frag_Color;

void main()
{
    Frag_Color = v_color;
}
