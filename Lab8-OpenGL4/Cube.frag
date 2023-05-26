#version 330

uniform sampler2D tSampler1;
uniform sampler2D tSampler2;

in vec2 TexCoord;
in float diffTerm;
out vec4 outputColor;

void main() 
{
	vec4 texColor1 = texture(tSampler1, TexCoord);  //Get colour from texture
	vec4 texColor2 = texture(tSampler2, TexCoord);

	outputColor = diffTerm * (1.0*texColor1 + 0.5*texColor2);  //Apply lighting term.
}
