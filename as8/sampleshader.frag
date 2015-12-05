//Shader for Phong Illuminations and Phong shading

uniform vec3 AmbientContribution,DiffuseContribution,SpecularContribution;
uniform float exponent;
varying vec3 vNormal, vLight, vView, vHalfway;

uniform sampler2D tex;

vec3 AmbientComponent(void)
{
   return vec3(AmbientContribution + 0.1);
}

vec3 DiffuseComponent(void)
{
   return vec3(DiffuseContribution * max(0.0, dot(vNormal, vLight)));
}

vec3 SpecularComponent(void)
{   
      // Approximation to the specular reflection using the halfway vector
      
      return vec3(SpecularContribution * pow(max(0.0, dot(vNormal, vHalfway)), exponent));  
}

/*
===============================================================================
   Phong Shading: Fragment Program
===============================================================================
*/

void main(void)
{
      
   // Phong Illumination Model
   vec4 texcolor = texture2D(tex, gl_TexCoord[0].st);
   vec4 color = (vec4(AmbientComponent(), 1.0) + vec4(DiffuseComponent(), 1.0) * texcolor) +
                vec4(SpecularComponent(), 1.0);  
   // Final color
   
   
   gl_FragColor = color;
}