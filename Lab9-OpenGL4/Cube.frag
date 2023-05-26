#version 330

uniform sampler2D tSampler;

in vec2 TexCoord;
in float diffTerm;
out vec4 outputColor;

void main() 
{ 
    // outputColor = diffTerm * vec4(0, 1, 0, 1);   //Green
    vec4 texColor = texture(tSampler, TexCoord); // Polkadots

    if (texColor.r == 1 && texColor.g != 1) {
        discard;
    }

    outputColor = texColor;
}
