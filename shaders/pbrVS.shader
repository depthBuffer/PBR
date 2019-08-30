#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT
{
	vec3 worldPos;
    vec2 texCoords;
	vec3 normal;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.worldPos = vec3(model * vec4(aPos, 1.0));
	vs_out.texCoords = aTexCoords;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vs_out.normal = normalMatrix * aNormal;

	gl_Position = projection * view * vec4(vs_out.worldPos, 1.0);
}