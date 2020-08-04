#version 420
#define BlendMultiply(base, blend) 		(base * blend)
#define BlendAverage(base, blend) 		((base + blend) / 2.0)
#define BlendSoftLightf(base, blend) 	((blend < 0.5) ? (2.0 * base * blend + base * base * (1.0 - 2.0 * blend)) : (sqrt(base) * (2.0 * blend - 1.0) + 2.0 * base * (1.0 - blend)))

in vec4 fColour;	
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;

uniform vec4 eyeLocation;
uniform vec4 diffuseColour;				
uniform vec4 specularColour;
uniform vec4 wireframeColour;		// Used to draw debug (or unlit) objects

uniform bool bWireframeMode;
uniform bool bIsSkyBox;
uniform bool bIsImposter;
uniform bool bNormalPassMode;
uniform bool bAlbedoPassMode;
uniform bool bUseTransparentMap;
uniform bool bNoTexture;
uniform bool bBaseTexture;
uniform bool bDiffuseColourBlend;
uniform bool bColourBlend;
uniform bool bMultiplyBlend;
uniform bool bAverageBlend;
uniform bool bSoftlightBlend;
uniform bool bNoLighting;

// Texture samplers
uniform sampler2D textSamp00;
uniform sampler2D textSamp01;
uniform sampler2D textSamp02;
uniform sampler2D textSamp03;
uniform samplerCube skyBox;
uniform sampler2D secondPassColourTexture;

//uniform vec4 tex_0_3_ratio;		// x = 0, y = 1, z = 2, w = 3
uniform vec4 tex_0_3_tile;		// x = 0, y = 1, z = 2, w = 3, Repeat texture

uniform int passNumber;

out vec4 pixelColour;			// RGB A   (0 to 1) 

// Fragment shader
struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 1;		// REMEMBER TO CHANGE THIS VALUE WHILE MODIFYING LIGHT CONFIG
uniform sLight theLights[NUMBEROFLIGHTS];  	// 80 uniforms

vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular );

vec3 RGBToHSL(vec3 color);
vec3 HSLToRGB(vec3 hsl);
float HueToRGB(float f1, float f2, float hue);
vec3 BlendColor(vec3 base, vec3 blend);

void main()  
{
	if ( passNumber == 1 )	// 2nd pass
	{
		float bo = 0.01f;		// For "blurr offset"

		vec3 texRGB1 = texture( secondPassColourTexture, vec2(fUVx2.s + 0.0f, fUVx2.t + 0.0f) ).rgb;
		
		vec3 texRGB2 = texture( secondPassColourTexture, vec2(fUVx2.s - bo, fUVx2.t + 0.0f) ).rgb;
		vec3 texRGB3 = texture( secondPassColourTexture, vec2(fUVx2.s + bo, fUVx2.t + 0.0f) ).rgb;
		vec3 texRGB4 = texture( secondPassColourTexture, vec2(fUVx2.s + 0.0f, fUVx2.t - bo) ).rgb;
		vec3 texRGB5 = texture( secondPassColourTexture, vec2(fUVx2.s + 0.0f, fUVx2.t + bo) ).rgb;

		vec3 RGB = 0.5f * texRGB1 +
		           0.125f * texRGB2 +
				   0.125f * texRGB3 +
				   0.125f * texRGB4 +
				   0.125f * texRGB5;

		pixelColour.rgb = RGB;
		pixelColour.a = 1.0f;

		return;
	}

	if ( bWireframeMode )
	{
		pixelColour.rgb = wireframeColour.rgb;
		pixelColour.a = 1.0f;				// NOT transparent
		return;
	}
	
	if ( bNormalPassMode )
	{
		pixelColour.rgb = fNormal.xyz;
		pixelColour.a = 1.0f;				// NOT transparent
		return;
	}
	
	if ( bIsSkyBox )
	{
		// sample the skybox using the normal from the surface
		vec3 skyColour = texture( skyBox, fNormal.xyz ).rgb;
		pixelColour.rgb = skyColour.rgb;
		pixelColour.a = 1.0f;				// NOT transparent		
		return;
	}

	if ( bAlbedoPassMode )
	{
		vec3 texAlbedo_RGB = texture( textSamp00, fUVx2.st * tex_0_3_tile.x ).rgb;
		pixelColour.rgb = texAlbedo_RGB;
		pixelColour.a = 1.0f;				// NOT transparent
		return;
	}

	if ( bNoTexture )
	{	
		vec4 outColour = calcualteLightContrib( diffuseColour.rgb, fNormal.xyz, fVertWorldLocation.xyz, specularColour );
		pixelColour = outColour;
		pixelColour.a = diffuseColour.a;
		return;
	}

	if ( bIsImposter )
	{
		vec3 texRGB = texture( textSamp00, fUVx2.st ).rgb;
		pixelColour.rgb = texRGB.rgb;
		float grey = (texRGB.r + texRGB.g + texRGB.b)/3.0f;
		if ( grey < 0.15 ) 	{	discard;	}
		pixelColour.a = diffuseColour.a;
		return;
	}	

	// The 3rd texture as cutout map
	vec3 tex2_RGB = texture( textSamp02, fUVx2.st * tex_0_3_tile.z).rgb;
	float tex2_grey = (tex2_RGB.r + tex2_RGB.g + tex2_RGB.b)/3.0f;
	if ( tex2_grey < 0.1f )	{	discard;	}		// Drop black pixel

	vec3 tex0_RGB = texture( textSamp00, fUVx2.st * tex_0_3_tile.x ).rgb;
	
	vec3 texRGB;
	
	if ( bBaseTexture )
	{
		texRGB = tex0_RGB;
	}

	if ( bDiffuseColourBlend )
	{
		texRGB = BlendColor( tex0_RGB, diffuseColour.rgb );
	}

	// The 2nd texture as blend map
	vec3 tex1_RGB = texture( textSamp01, fUVx2.st * tex_0_3_tile.y).rgb;

	if ( bColourBlend )
	{
		texRGB = BlendColor( tex0_RGB, tex1_RGB );
	}
	
	if ( bMultiplyBlend )
	{
		texRGB = BlendMultiply( tex0_RGB, tex1_RGB );
	}
	
	if ( bAverageBlend )
	{
		texRGB = BlendAverage( tex0_RGB, tex1_RGB );
	}
	
	if ( bSoftlightBlend )
	{
		texRGB.r = BlendSoftLightf( tex0_RGB.r, tex1_RGB.r );
		texRGB.g = BlendSoftLightf( tex0_RGB.g, tex1_RGB.g );
		texRGB.b = BlendSoftLightf( tex0_RGB.b, tex1_RGB.b );
	}
	
	// Lighting computation
	vec4 outColour = calcualteLightContrib( texRGB.rgb, fNormal.xyz, fVertWorldLocation.xyz, specularColour );
	if ( bNoLighting )
	{
		pixelColour.rgb = texRGB.rgb;
	}
	else
	{
		pixelColour = outColour;
	}
	

	// The 4th texture as transparent map
	if ( bUseTransparentMap )
	{
		vec3 tex3_RGB = texture( textSamp03, fUVx2.st * tex_0_3_tile.w).rgb;
		float tex3_grey = (tex3_RGB.r + tex3_RGB.g + tex3_RGB.b)/3.0f;
		pixelColour.a = tex3_grey;
	}
	else
	{
		pixelColour.a = diffuseColour.a;
	}
	
}

vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			vec3 lightAmbientContrib = theLights[index].diffuse.rgb * 0.2;

			// Diffuse
			float diff = max( 0.0f, dot( -theLights[index].direction.xyz, norm ) );		// 0 to 1	
			vec3 lightDiffuseContrib = diff * theLights[index].diffuse.rgb;

			// Specular 			
			vec3 reflectDir = reflect( theLights[index].direction.xyz, norm );			
			vec3 toEyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);			
			vec3 lightSpecularContrib = pow( max(0.0f, dot(toEyeVector, reflectDir)), vertexSpecular.w )
			                   * theLights[index].specular.rgb;
			
			finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib)
								  + (vertexSpecular.rgb  * lightSpecularContrib )
								  + (vertexMaterialColour.rgb * lightAmbientContrib);
			continue;
			
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		// -1 to 1
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		// If it's negative, will clamp to 0 --- range from 0 to 1
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 		

		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * theLights[index].specular.rgb;
							   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{
			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )		
					
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb  * lightSpecularContrib.rgb );

	}//for(intindex=0...
	
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}


// Color Mode keeps the brightness of the base color and applies both the hue and saturation of the blend color.
vec3 BlendColor(vec3 base, vec3 blend)
{
	vec3 blendHSL = RGBToHSL(blend);
	return HSLToRGB(vec3(blendHSL.r, blendHSL.g, RGBToHSL(base).b));
}

vec3 RGBToHSL(vec3 color)
{
	vec3 hsl; // init to 0 to avoid warnings ? (and reverse if + remove first part)
	
	float fmin = min(min(color.r, color.g), color.b);    //Min. value of RGB
	float fmax = max(max(color.r, color.g), color.b);    //Max. value of RGB
	float delta = fmax - fmin;             //Delta RGB value

	hsl.z = (fmax + fmin) / 2.0; // Luminance

	if (delta == 0.0)		//This is a gray, no chroma...
	{
		hsl.x = 0.0;	// Hue
		hsl.y = 0.0;	// Saturation
	}
	else                                    //Chromatic data...
	{
		if (hsl.z < 0.5)
			hsl.y = delta / (fmax + fmin); // Saturation
		else
			hsl.y = delta / (2.0 - fmax - fmin); // Saturation
		
		float deltaR = (((fmax - color.r) / 6.0) + (delta / 2.0)) / delta;
		float deltaG = (((fmax - color.g) / 6.0) + (delta / 2.0)) / delta;
		float deltaB = (((fmax - color.b) / 6.0) + (delta / 2.0)) / delta;

		if (color.r == fmax )
			hsl.x = deltaB - deltaG; // Hue
		else if (color.g == fmax)
			hsl.x = (1.0 / 3.0) + deltaR - deltaB; // Hue
		else if (color.b == fmax)
			hsl.x = (2.0 / 3.0) + deltaG - deltaR; // Hue

		if (hsl.x < 0.0)
			hsl.x += 1.0; // Hue
		else if (hsl.x > 1.0)
			hsl.x -= 1.0; // Hue
	}

	return hsl;
}

vec3 HSLToRGB(vec3 hsl)
{
	vec3 rgb;
	
	if (hsl.y == 0.0)
		rgb = vec3(hsl.z); // Luminance
	else
	{
		float f2;
		
		if (hsl.z < 0.5)
			f2 = hsl.z * (1.0 + hsl.y);
		else
			f2 = (hsl.z + hsl.y) - (hsl.y * hsl.z);
			
		float f1 = 2.0 * hsl.z - f2;
		
		rgb.r = HueToRGB(f1, f2, hsl.x + (1.0/3.0));
		rgb.g = HueToRGB(f1, f2, hsl.x);
		rgb.b= HueToRGB(f1, f2, hsl.x - (1.0/3.0));
	}
	
	return rgb;
}

float HueToRGB(float f1, float f2, float hue)
{
	if (hue < 0.0)
		hue += 1.0;
	else if (hue > 1.0)
		hue -= 1.0;
	float res;
	if ((6.0 * hue) < 1.0)
		res = f1 + (f2 - f1) * 6.0 * hue;
	else if ((2.0 * hue) < 1.0)
		res = f2;
	else if ((3.0 * hue) < 2.0)
		res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
	else
		res = f1;
	return res;
}
