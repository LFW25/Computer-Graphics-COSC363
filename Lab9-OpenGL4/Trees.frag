#version 330

uniform sampler2D texRoad;
uniform sampler2D texTree;

uniform int objIndex;

in vec2 TexCoord;
in float diffTerm;
out vec4 outputColor;

void main() 
{
	if (objIndex == 0)  //Road
	{ 
	   vec4 roadColor =  texture(texRoad, TexCoord);

	   if (roadColor.a == 0) {
		discard;
	   }

	   outputColor = roadColor;

      }
	 else if (objIndex == 1)  //Trees
      {
		vec4 treeColor = texture(texTree, TexCoord);
		if (treeColor.a == 0 || treeColor.g == 0) {
			discard;
		}
		outputColor = treeColor;
	}


}
	
