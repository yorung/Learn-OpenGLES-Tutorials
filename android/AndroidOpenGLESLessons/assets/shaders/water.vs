precision mediump float;
attribute vec3 vPosition;
attribute float upperHeight;
attribute float leftHeight;
varying vec2 texcoord;
varying vec2 position;
varying vec3 normal;
varying vec4 color;
uniform mat4 matW;
uniform mat4 matV;
uniform mat4 matP;

const float airToWater = 1.0 / 1.33333;
const vec3 camDir = vec3(0, 0, -1);
const float waterDepth = 0.2;

void main() {
	mat4 matWV = matV * matW;
	vec3 leftP = vec3(vPosition.x - 2.0 / 50.0, leftHeight, vPosition.z);
	vec3 upperP = vec3(vPosition.x, upperHeight, vPosition.z - 2.0 / 50.0);
	gl_Position = matWV * vec4(vPosition.xyz, 1) * matP;
	normal = normalize(cross(vPosition - leftP, upperP - vPosition)) * mat3(matW);
//	normal = vec3(0, 0, 1);

	vec3 rayDir = refract(camDir, normal, airToWater);

	vec3 bottom = rayDir * waterDepth / rayDir.z;

	position = vPosition.xz;
	texcoord = (vPosition.xz + bottom.xy) * vec2(0.5, -0.5) + vec2(0.5, 0.5);

	float mask = dot(normal, vec3(0, 0, 1));
	color = vec4(1, 1, 1, 1.0 - mask);
}
