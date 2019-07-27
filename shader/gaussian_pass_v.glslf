#version 440

//Horizontal gaussian blur leveraging hardware filtering for fewer texture lookups.
uniform vec3 resolution;
uniform sampler2D tex;

vec3 ColorFetch(vec2 coord)
{
 	return texture(tex, coord).rgb;   
}

float weights[5] = float[](0.19638062,0.29675293,0.09442139,0.01037598,0.00025940);
float offsets[5] = float[](0.00000000,1.41176471,3.29411765,5.17647059,7.05882353);


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{    
    vec2 uv = fragCoord.xy / resolution.xy;
    
    vec3 color = vec3(0.0);
    float weightSum = 0.0;

    color += ColorFetch(uv) * weights[0];
    weightSum += weights[0];

    for(int i = 1; i < 5; i++)
    {
        vec2 offset = vec2(offsets[i]) / resolution.xy;
        color += ColorFetch(uv + offset * vec2(0.0, 0.5)) * weights[i];
        color += ColorFetch(uv - offset * vec2(0.0, 0.5)) * weights[i];
        weightSum += weights[i] * 2.0;
    }

    color /= weightSum;

    fragColor = vec4(color,1.0);
}

void main() {
	mainImage(gl_FragColor, gl_FragCoord.xy);
}