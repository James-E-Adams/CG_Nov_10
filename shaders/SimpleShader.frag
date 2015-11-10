#version 330

uniform vec4 fillColor;

out vec4 outColor;

vec4 checker(vec2 pos, vec4 color1, int scale);

// Extended by James Adams and Shiran Gazit.

void main(void)
{
	//Adjustable scale to change checkerboard sizing.
	int scale=10;
	//take the mod of the coordinates to account for scaling
	vec2 pos = mod(gl_FragCoord.xy,vec2(scale));

	//work out which colour to shade, and assign it.
	//outColor=checker(pos, vec4(0.0, 0.0, 0.0, 1.0), scale);
	outColor=fillColor;
}




/** Function to decide which colour to shade a certain fragment, based on its position.
**/
vec4 checker(vec2 pos, vec4 color1, int scale)
{
	float scale_factor = scale/2.0;

	if ((pos.x > scale_factor)&&(pos.y > scale_factor)) {
		return fillColor;
	}
	if ((pos.x < scale_factor)&&(pos.y < scale_factor)) {
		return fillColor;
	}
	if ((pos.x < scale_factor)&&(pos.y > scale_factor)) {
		return vec4(0.0, 0.0, 0.0, 1.0);
	}
	if ((pos.x > scale_factor)&&(pos.y < scale_factor)) {
		return vec4(0.0, 0.0, 0.0, 1.0);
	}
}