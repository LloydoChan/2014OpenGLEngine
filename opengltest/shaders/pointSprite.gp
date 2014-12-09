#version 330

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform mat4 viewProjectionMatrix;
uniform vec3 cameraPos;

out vec2 texCoord;

void main(){
  vec3 Pos      = gl_in[0].gl_Position.xyz;
  vec3 toCamera = normalize(cameraPos - Pos);
  vec3 up = vec3(0.0,1.0,0.0);
  vec3 right = cross(toCamera,up);
  
  Pos -= (right * 0.5);
    gl_Position = viewProjectionMatrix * vec4(Pos, 1.0);
    texCoord = vec2(0.0, 0.0);
    EmitVertex();

    Pos.y += 1.0;
    gl_Position = viewProjectionMatrix * vec4(Pos, 1.0);
    texCoord = vec2(0.0, 1.0);
    EmitVertex();

    Pos.y -= 1.0;
    Pos += right;
    gl_Position = viewProjectionMatrix * vec4(Pos, 1.0);
    texCoord = vec2(1.0, 0.0);
    EmitVertex();

    Pos.y += 1.0;
    gl_Position = viewProjectionMatrix * vec4(Pos, 1.0);
    texCoord = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}