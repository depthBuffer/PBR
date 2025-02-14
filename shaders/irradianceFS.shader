#version 330 core
out vec4 FragColor;

in vec3 worldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
	vec3 normal = normalize(worldPos);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, normal);
	up = cross(normal, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;

	vec3 Irradiance = vec3(0.0);
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			Irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	Irradiance = PI * Irradiance * (1.0 / nrSamples);

	FragColor = vec4(Irradiance, 1.0);
}