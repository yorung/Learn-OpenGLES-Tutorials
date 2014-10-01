precision highp float;
varying vec2 position;
varying vec3 normal;
varying vec2 texcoord;
varying vec4 color;
uniform sampler2D bg1;
uniform sampler2D bg2;
uniform sampler2D spheremap;
uniform float time;
uniform float transferTime;

const float loopTime = 20.0;
const float PI2 = 3.1415926 * 2.0;
const float transferDuration = 0.4166;

void main() {
	float dist1 = length(position + vec2(0.5, 0.5));
	float dist2 = length(position - vec2(0.5, 0.5));

	float radTimeUnit = time / loopTime * PI2;
	vec2 coord = vec2(texcoord.x, texcoord.y + sin(dist1 * 8.7 + radTimeUnit * 25.0) / 800.0 + sin(dist2 * 10.0 + radTimeUnit * 48.0) / 800.0);

	vec4 c1 = texture2D(bg1, coord);
	vec4 c2 = texture2D(bg2, coord);
	float delaymap = dot(position, vec2(-0.7071, -0.7071)) * 0.7071 * 0.5 + 0.5;
	vec4 bg = mix(c1, c2, clamp((transferTime - delaymap) / transferDuration, 0.0, 1.0));

	vec3 normalForSample = normal;
	vec4 skyColor = texture2D(spheremap, normalForSample.xy * vec2(0.5, -0.5) + vec2(0.5, 0.5));
	gl_FragColor = mix(bg, skyColor + 0.5, color.w);
}
