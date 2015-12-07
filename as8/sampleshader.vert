   
uniform vec3 AmbientContribution,DiffuseContribution,SpecularContribution;
uniform float exponent;
uniform int selection1;
attribute float tangx, tangy, tangz;

varying vec3 vNormal, vLight, vView, vHalfway, vLightTSpace, vViewTSpace, vHalfwayTSpace;

//uniform sampler2D tex;
/*
===============================================================================
   Phong Shading: Vertex Program
===============================================================================
*/

void main(void)
{
   // Transform vertex position to view space
   
   vec3 pos = vec3( gl_ModelViewMatrix * gl_Vertex );
   
   // Compute normal, light and view vectors in view space
   
   vNormal   = normalize(gl_NormalMatrix * gl_Normal);
   vLight    = normalize(vec3(gl_LightSource[0].position)- pos);
   vView     = normalize(-pos);
   
   // Compute the halfway vector if the halfway approximation is used   
   vec3 tang = vec3(tangx, tangy, tangz);
   vec3 bin = cross(tang, vNormal);
   //if(mode == 9 || mode == 10) {
	 mat3 TBN = transpose(mat3(tang, bin, vNormal));
	vLightTSpace = TBN * vLight;
	 vViewTSpace = TBN * vView;
	 vHalfwayTSpace = normalize(vLightTSpace + vViewTSpace);
   //}
   vHalfway  = normalize(vLight + vView );
	float boo = selection1;
   
   gl_TexCoord[0] = gl_MultiTexCoord0;

   gl_Position = ftransform();
   


}