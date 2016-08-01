#version 330 core

const int numBlurPixelsPerSide = 2;	// 2 * numBlurPixelPerSide + 1 in the middle
const float sigma = 2.5;	// The sigma value for the gaussian function: higher value means more blur
							// A good value for 9x9 is around 3 to 5
							// A good value for 7x7 is around 2.5 to 4
							// A good value for 5x5 is around 2 to 3.5
const float twoPi = 6.283185307179586476925286766559f;

uniform sampler2D blurSampler;
uniform float blurSize;	// 1 / textureWidth or 1 / textureHeight
uniform vec2 direction;

in vec2 vsTextureCoord;
out vec4 fsColor;

void main()
{
	vec3 incrementalGaussian;
	incrementalGaussian.x = 1.0f / (sqrt(twoPi) * sigma);
	incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
	incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;

	float coefficientSum = 0.0;

	// Take the central sample first...
	vec4 average = texture(blurSampler, vsTextureCoord) * incrementalGaussian.x;
	coefficientSum += incrementalGaussian.x;
	incrementalGaussian.xy *= incrementalGaussian.yz;

	for (int i = 1; i <= numBlurPixelsPerSide; ++i)
	{
		average += texture(blurSampler, vsTextureCoord - i * blurSize * direction) * incrementalGaussian.x;
		average += texture(blurSampler, vsTextureCoord + i * blurSize * direction) * incrementalGaussian.x;
		coefficientSum += 2 * incrementalGaussian.x;
		incrementalGaussian.xy *= incrementalGaussian.yz;
	}

	fsColor = average / coefficientSum;
}
