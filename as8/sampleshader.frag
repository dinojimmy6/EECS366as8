//Shader for Phong Illuminations and Phong shading

uniform vec3 AmbientContribution,DiffuseContribution,SpecularContribution;
uniform float exponent;
uniform int selection1;
varying vec3 vNormal, vLight, vView, vHalfway, vLightTSpace, vViewTSpace, vHalfwayTSpace;

uniform sampler2D tex;

vec3 AmbientComponent(void)
{
   return vec3(AmbientContribution + 0.1);
}

vec3 DiffuseComponent(vec3 normal, vec3 light)
{
   return vec3(DiffuseContribution * max(0.0, dot(normal, light)));
}

vec3 SpecularComponent(vec3 normal, vec3 halfway)
{   
      // Approximation to the specular reflection using the halfway vector
      return vec3(SpecularContribution * pow(max(0.0, dot(normal, halfway)), exponent));  
}

/*
===============================================================================
   Phong Shading: Fragment Program
===============================================================================
*/
const vec2 size = vec2(2.0,0.0);
const ivec3 off = ivec3(-1,0,1);
void main(void)
{
      
   // Phong Illumination Model
   vec4 texcolor = texture2D(tex, gl_TexCoord[0].st);
   

    float s11 = texcolor.x;
    float s01 = textureOffset(tex, gl_TexCoord[0].st, off.xy).x;
    float s21 = textureOffset(tex, gl_TexCoord[0].st, off.zy).x;
    float s10 = textureOffset(tex, gl_TexCoord[0].st, off.yx).x;
    float s12 = textureOffset(tex, gl_TexCoord[0].st, off.yz).x;
    vec3 va = normalize(vec3(size.xy,s21-s01));
    vec3 vb = normalize(vec3(size.yx,s12-s10));
    vec3 bump = cross(va,vb);
	//vec3 bump = normalize(texcolor.rgb*2.0 - 1.0);
	vec4 color;
	if(selection1 < 9) {
		color = (vec4(AmbientComponent(), 1.0) + vec4(DiffuseComponent(vNormal, vLight), 1.0) * texcolor) +
                vec4(SpecularComponent(vNormal, vHalfway), 1.0);
	}
	else {
	
		color = (vec4(AmbientComponent(), 1.0) + vec4(DiffuseComponent(bump, vLightTSpace), 1.0)) +
                vec4(SpecularComponent(bump, vHalfwayTSpace), 1.0);
	}
   // Final color
   
   
   gl_FragColor = color;
}