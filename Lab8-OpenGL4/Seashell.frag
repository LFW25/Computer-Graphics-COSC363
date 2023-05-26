#version 330

in vec4 oColor;
in vec4 material;
in float diffTerm;
in float silTerm;

out vec4 outputColor;

void main()
{
	if (abs(silTerm) < 0.2) {
		outputColor = vec4(0.01, 0.01, 0.01, 1.0);

	} else if (diffTerm < 0.2) {
		outputColor = vec4(0.16, 0.09, 0.06, 1.0);

	} else {
		outputColor = material;
	}
}
