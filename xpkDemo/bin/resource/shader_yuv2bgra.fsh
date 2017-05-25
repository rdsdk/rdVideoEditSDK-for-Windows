//RGB_Alpha :  RGBA 的 Alpha值
//InputType	:  1003.0表示yuv420p ;1000.0表示RGBA数据

uniform float RGB_Alpha;				//alpha
uniform sampler2D textureUniformY;
uniform sampler2D textureUniformU;
uniform sampler2D textureUniformV;
uniform sampler2D textureUniformRGBA;
uniform sampler2D toneCurveTextureUniform;
varying vec2 textureCoordinate;
uniform float PixFormat;				//输入像素格式
uniform float fliter_id;				//滤镜id
uniform float texelWidthOffset;
uniform float texelHeightOffset;
uniform int beauty_level;
uniform int dark;
 
float beauty_filter_r = 0.0f;
float beauty_filter_g = 0.0f;
float beauty_filter_b = 0.0f;
float beauty_filter_a = 0.0f;

 float blurSize = 0.8f;
 
vec2 centerTextureCoordinate;
vec2 oneStepLeftTextureCoordinate;
vec2 twoStepsLeftTextureCoordinate;
vec2 oneStepRightTextureCoordinate;
vec2 twoStepsRightTextureCoordinate;
 
const  vec3 W = vec3(0.299,0.587,0.114);
const mat3 saturateMatrix = mat3(
		1.1102,-0.0598,-0.061,
		-0.0774,1.0826,-0.1186,
		-0.0228,-0.0228,1.1772);
int setBeautyLevel(int level)
{
	if(1 == level)
	{
		beauty_filter_r = 1.0f;
		beauty_filter_g = 1.0f;
		beauty_filter_b = 0.15f;
		beauty_filter_a = 0.15f;
	}
	else if(2 == level)
	{
		beauty_filter_r = 0.8f;
		beauty_filter_g = 0.9f;
		beauty_filter_b = 0.2f;
		beauty_filter_a = 0.2f;
	}
	else if(3 == level)
	{
		beauty_filter_r = 0.6f;
		beauty_filter_g = 0.8f;
		beauty_filter_b = 0.25f;
		beauty_filter_a = 0.25f;
	}
	else if(4 == level)
	{
		beauty_filter_r = 0.4f;
		beauty_filter_g = 0.7f;
		beauty_filter_b = 0.38f;
		beauty_filter_a = 0.3f;
	}
	else if(5 == level)
	{
		beauty_filter_r = 0.33f;
		beauty_filter_g = 0.63f;
		beauty_filter_b = 0.4f;
		beauty_filter_a = 0.35f;
	}
	else
	{
		beauty_filter_r = 1.0f;
		beauty_filter_g = 1.0f;
		beauty_filter_b = 0.15f;
		beauty_filter_a = 0.15f;
	
	}
	return 1;
}
float hardlight(float color)
{
	if(color <= 0.5)
	{
		color = color * color * 2.0;
	}
	else
	{
		color = 1.0 - ((1.0 - color)*(1.0 - color) * 2.0);
	}
	return color;
}

void main()
{
	float brightness = 0.45f; 
	vec4 textureColor;
	vec4 outColor;
	vec4 U;
	vec4 V;

	if(PixFormat == 1000.0f)
	{
		//RGB32
		if(1 == dark )
		{
			int multiplier = 0;
 			int GAUSSIAN_SAMPLES = 9;	
 			vec2 blurStep;
 			vec2 blurCoordinates[9];
			vec2 singleStepOffset = vec2(texelWidthOffset, texelHeightOffset);
     
 			for (int i = 0; i < GAUSSIAN_SAMPLES; i++) {
 				multiplier = (i - ((GAUSSIAN_SAMPLES - 1) / 2));
				// Blur in x (horizontal)
				blurStep = float(multiplier) * singleStepOffset;
 				blurCoordinates[i] = textureCoordinate.xy + blurStep;
 			}
 			vec4 sum = vec4(0.0);
         
			sum += texture2D(textureUniformRGBA, blurCoordinates[0]) * 0.05;
			sum += texture2D(textureUniformRGBA, blurCoordinates[1]) * 0.09;
			sum += texture2D(textureUniformRGBA, blurCoordinates[2]) * 0.12;
			sum += texture2D(textureUniformRGBA, blurCoordinates[3]) * 0.15;
			sum += texture2D(textureUniformRGBA, blurCoordinates[4]) * 0.18;
			sum += texture2D(textureUniformRGBA, blurCoordinates[5]) * 0.15;
			sum += texture2D(textureUniformRGBA, blurCoordinates[6]) * 0.12;
			sum += texture2D(textureUniformRGBA, blurCoordinates[7]) * 0.09;
			sum += texture2D(textureUniformRGBA, blurCoordinates[8]) * 0.05;
			//加暗
			textureColor = vec4((sum.rgb * vec3(brightness)), sum.a);	
		}
		else
		{
			textureColor = vec4(texture2D(textureUniformRGBA,textureCoordinate));
			textureColor.a = textureColor.a*RGB_Alpha;	
		}
		
	}
	else
	{		
		//YUV420  TO  RGB32
// 		textureColor = vec4((texture2D(textureUniformY, textureCoordinate).r - 16./255.) * 1.164);
// 		U = vec4(texture2D(textureUniformU, textureCoordinate).r - 128./255.);
// 		V = vec4(texture2D(textureUniformV, textureCoordinate).r - 128./255.);
// 		textureColor += V * vec4(1.596, -0.813, 0, 0);
// 		textureColor += U * vec4(0, -0.392, 2.017, 0);
		
		if(1 == dark)
		{
			//模糊
			int multiplier = 0;
 			int GAUSSIAN_SAMPLES = 9;	
 			vec2 blurStep;
 			vec2 blurCoordinates[9];
			vec2 singleStepOffset = vec2(texelWidthOffset, texelHeightOffset);
     
 			for (int i = 0; i < GAUSSIAN_SAMPLES; i++) {
 				multiplier = (i - ((GAUSSIAN_SAMPLES - 1) / 2));
				// Blur in x (horizontal)
				blurStep = float(multiplier) * singleStepOffset;
 				blurCoordinates[i] = textureCoordinate.xy + blurStep;
 			}
 			vec4 sum = vec4(0.0);
         
			sum = vec4((texture2D(textureUniformY, blurCoordinates[0]).r - 16./255.) * 1.164);
			U = vec4(texture2D(textureUniformU, blurCoordinates[0]).r - 128./255.);
			V = vec4(texture2D(textureUniformV, blurCoordinates[0]).r - 128./255.);
			sum += V * vec4(1.596, -0.813, 0, 0);
			sum += U * vec4(0, -0.392, 2.017, 0);
			sum *=0.05;
			
			
			vec4 sum1 = vec4((texture2D(textureUniformY, blurCoordinates[1]).r - 16./255.) * 1.164);
			U = vec4(texture2D(textureUniformU, blurCoordinates[1]).r - 128./255.);
			V = vec4(texture2D(textureUniformV, blurCoordinates[1]).r - 128./255.);
			sum1 += V * vec4(1.596, -0.813, 0, 0);
			sum1 += U * vec4(0, -0.392, 2.017, 0);
			sum1 *=0.09;
			
			vec4 sum2 = vec4((texture2D(textureUniformY, blurCoordinates[2]).r - 16./255.) * 1.164);
			U = vec4(texture2D(textureUniformU, blurCoordinates[2]).r - 128./255.);
			V = vec4(texture2D(textureUniformV, blurCoordinates[2]).r - 128./255.);
			sum2 += V * vec4(1.596, -0.813, 0, 0);
			sum2 += U * vec4(0, -0.392, 2.017, 0);
			sum2 *=0.12;
			
			vec4 sum3 = vec4((texture2D(textureUniformY, blurCoordinates[3]).r - 16./255.) * 1.164);
			U = vec4(texture2D(textureUniformU, blurCoordinates[3]).r - 128./255.);
			V = vec4(texture2D(textureUniformV, blurCoordinates[3]).r - 128./255.);
			sum3 += V * vec4(1.596, -0.813, 0, 0);
			sum3 += U * vec4(0, -0.392, 2.017, 0);
			sum3 *=0.15;
			
			vec4 sum4 = vec4((texture2D(textureUniformY, blurCoordinates[4]).r - 16./255.) * 1.164);
			U = vec4(texture2D(textureUniformU, blurCoordinates[4]).r - 128./255.);
			V = vec4(texture2D(textureUniformV, blurCoordinates[4]).r - 128./255.);
			sum4 += V * vec4(1.596, -0.813, 0, 0);
			sum4 += U * vec4(0, -0.392, 2.017, 0);
			sum4 *=0.18;
			
			vec4 sum5 = vec4((texture2D(textureUniformY, blurCoordinates[5]).r - 16./255.) * 1.164);
			U = vec4(texture2D(textureUniformU, blurCoordinates[5]).r - 128./255.);
			V = vec4(texture2D(textureUniformV, blurCoordinates[5]).r - 128./255.);
			sum5 += V * vec4(1.596, -0.813, 0, 0);
			sum5 += U * vec4(0, -0.392, 2.017, 0);
			sum5 *=0.15;
			
			
			vec4 sum6 = vec4((texture2D(textureUniformY, blurCoordinates[6]).r - 16./255.) * 1.164);
			U = vec4(texture2D(textureUniformU, blurCoordinates[6]).r - 128./255.);
			V = vec4(texture2D(textureUniformV, blurCoordinates[6]).r - 128./255.);
			sum6 += V * vec4(1.596, -0.813, 0, 0);
			sum6 += U * vec4(0, -0.392, 2.017, 0);
			sum6 *=0.12;
			
			vec4 sum7 = vec4((texture2D(textureUniformY, blurCoordinates[7]).r - 16./255.) * 1.164);
			U = vec4(texture2D(textureUniformU, blurCoordinates[7]).r - 128./255.);
			V = vec4(texture2D(textureUniformV, blurCoordinates[7]).r - 128./255.);
			sum7 += V * vec4(1.596, -0.813, 0, 0);
			sum7 += U * vec4(0, -0.392, 2.017, 0);
			sum7 *=0.09;
	
	
			vec4 sum8 = vec4((texture2D(textureUniformY, blurCoordinates[7]).r - 16./255.) * 1.164);
			U = vec4(texture2D(textureUniformU, blurCoordinates[7]).r - 128./255.);
			V = vec4(texture2D(textureUniformV, blurCoordinates[7]).r - 128./255.);
			sum8 += V * vec4(1.596, -0.813, 0, 0);
			sum8 += U * vec4(0, -0.392, 2.017, 0);
			sum8 *=0.05;

			sum += sum1 + sum2 + sum3 + sum4 + sum5 + sum6 +sum7 + sum8;
			//加暗
			textureColor = vec4((sum.rgb * vec3(brightness)), sum.a);
		}
		else
		{
			textureColor = vec4((texture2D(textureUniformY, textureCoordinate).r - 16./255.) * 1.164);
			U = vec4(texture2D(textureUniformU, textureCoordinate).r - 128./255.);
			V = vec4(texture2D(textureUniformV, textureCoordinate).r - 128./255.);
			textureColor += V * vec4(1.596, -0.813, 0, 0);
			textureColor += U * vec4(0, -0.392, 2.017, 0);
		}
		
		textureColor.a = RGB_Alpha;
	}

	if(fliter_id == 0.0f)
	{
		//没有滤镜
		gl_FragColor = textureColor;
	}
	else if (fliter_id == 1.0f)
	{
		//美颜
		vec2 blurCoordinates[24];
		vec2 singleStepOffset = vec2(texelWidthOffset,texelHeightOffset);
		
		//设置滤镜等级
		setBeautyLevel(beauty_level);
	 
		blurCoordinates[0] = textureCoordinate.xy + singleStepOffset * vec2(0.0, -10.0);
		blurCoordinates[1] = textureCoordinate.xy + singleStepOffset * vec2(0.0, 10.0);
		blurCoordinates[2] = textureCoordinate.xy + singleStepOffset * vec2(-10.0, 0.0);
		blurCoordinates[3] = textureCoordinate.xy + singleStepOffset * vec2(10.0, 0.0);
	
		blurCoordinates[4] = textureCoordinate.xy + singleStepOffset * vec2(5.0, -8.0);
		blurCoordinates[5] = textureCoordinate.xy + singleStepOffset * vec2(5.0, 8.0);
		blurCoordinates[6] = textureCoordinate.xy + singleStepOffset * vec2(-5.0, 8.0);
		blurCoordinates[7] = textureCoordinate.xy + singleStepOffset * vec2(-5.0, -8.0);
	
		blurCoordinates[8] = textureCoordinate.xy + singleStepOffset * vec2(8.0, -5.0);
		blurCoordinates[9] = textureCoordinate.xy + singleStepOffset * vec2(8.0, 5.0);
		blurCoordinates[10] = textureCoordinate.xy + singleStepOffset * vec2(-8.0, 5.0);
		blurCoordinates[11] = textureCoordinate.xy + singleStepOffset * vec2(-8.0, -5.0);
	
		blurCoordinates[12] = textureCoordinate.xy + singleStepOffset * vec2(0.0, -6.0);
		blurCoordinates[13] = textureCoordinate.xy + singleStepOffset * vec2(0.0, 6.0);
		blurCoordinates[14] = textureCoordinate.xy + singleStepOffset * vec2(6.0, 0.0);
		blurCoordinates[15] = textureCoordinate.xy + singleStepOffset * vec2(-6.0, 0.0);
	
		blurCoordinates[16] = textureCoordinate.xy + singleStepOffset * vec2(-4.0, -4.0);
		blurCoordinates[17] = textureCoordinate.xy + singleStepOffset * vec2(-4.0, 4.0);
		blurCoordinates[18] = textureCoordinate.xy + singleStepOffset * vec2(4.0, -4.0);
		blurCoordinates[19] = textureCoordinate.xy + singleStepOffset * vec2(4.0, 4.0);

		blurCoordinates[20] = textureCoordinate.xy + singleStepOffset * vec2(-2.0, -2.0);
		blurCoordinates[21] = textureCoordinate.xy + singleStepOffset * vec2(-2.0, 2.0);
		blurCoordinates[22] = textureCoordinate.xy + singleStepOffset * vec2(2.0, -2.0);
		blurCoordinates[23] = textureCoordinate.xy + singleStepOffset * vec2(2.0, 2.0);


		float sampleColor = texture2D(textureUniformRGBA, textureCoordinate).g * 22.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[0]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[1]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[2]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[3]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[4]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[5]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[6]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[7]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[8]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[9]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[10]).g;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[11]).g;

		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[12]).g * 2.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[13]).g * 2.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[14]).g * 2.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[15]).g * 2.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[16]).g * 2.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[17]).g * 2.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[18]).g * 2.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[19]).g * 2.0;
	
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[20]).g * 3.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[21]).g * 3.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[22]).g * 3.0;
		sampleColor += texture2D(textureUniformRGBA, blurCoordinates[23]).g * 3.0;
			
		sampleColor = sampleColor / 62.0;

		//vec4 textureColor = vec4(texture2D(textureUniformRGBA,textureCoordinate));
		vec3 centralColor = textureColor.rgb;//texture2D(textureUniformRGBA, textureCoordinate).rgb;		

		float highpass = centralColor.g - sampleColor + 0.5;

		for(int i = 0; i < 5;i++)
		{
			highpass = hardlight(highpass);
		}
		float lumance = dot(centralColor, W);
	
		float alpha = pow(lumance, beauty_filter_r);
		
 		vec3 smoothColor = centralColor + (centralColor-vec3(highpass))*alpha*0.1;
 		
 
 		smoothColor.r = clamp(pow(smoothColor.r, beauty_filter_g),0.0,1.0);
 		smoothColor.g = clamp(pow(smoothColor.g, beauty_filter_g),0.0,1.0);
 		smoothColor.b = clamp(pow(smoothColor.b, beauty_filter_g),0.0,1.0);
 		
 		vec3 lvse = vec3(1.0)-(vec3(1.0)-smoothColor)*(vec3(1.0)-centralColor);
		vec3 bianliang = max(smoothColor, centralColor);
		vec3 rouguang = 2.0*centralColor*smoothColor + centralColor*centralColor - 2.0*centralColor*centralColor*smoothColor;

		gl_FragColor = vec4(mix(centralColor, lvse, alpha), 1.0);
		gl_FragColor.rgb = mix(gl_FragColor.rgb, bianliang, alpha);
		gl_FragColor.rgb = mix(gl_FragColor.rgb, rouguang, beauty_filter_b);

		vec3 satcolor = gl_FragColor.rgb * saturateMatrix;
		gl_FragColor.rgb = mix(gl_FragColor.rgb, satcolor, beauty_filter_a);

	}
 	else 
	{
		//acv曲线
		float redCurveValue = texture2D(toneCurveTextureUniform, vec2(textureColor.r, 0)).r;
		float greenCurveValue = texture2D(toneCurveTextureUniform, vec2(textureColor.g, 0)).g;
		float blueCurveValue = texture2D(toneCurveTextureUniform, vec2(textureColor.b, 0)).b;
		gl_FragColor = vec4(redCurveValue, greenCurveValue, blueCurveValue,textureColor.a);
	}

}