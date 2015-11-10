#version 330

uniform vec4 fillColor;
uniform vec4 highLight;
uniform float ballSize;
in vec2 window_size;

out vec4 outColor;

vec4 checker(vec2 pos, vec4 color1, int scale);

// Extended by James Adams and Shiran Gazit.

void main(void)
{
	//Adjustable scale to change checkerboard sizing.
	int scale=10;
	//take the mod of the coordinates to account for scaling
	vec2 pos = mod(gl_FragCoord.xy,vec2(scale));
	vec2 frag_norm = (gl_FragCoord.xy / window_size) * 2 - 1;
    float dist = ((highLight.x - frag_norm.x) * (highLight.x - frag_norm.x))  +
                        ((highLight.y - frag_norm.y) * (highLight.y - frag_norm.y));
    dist = sqrt(dist);
    float m = dist/(pow(ballSize, 2));

	outColor = 1 - (m/2 * fillColor);
}



