#version 330 core

uniform sampler2D sceneTex;

uniform int axis;

in Vertex {
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void) {
	float x;
	float y;
	float r = 15.0;
	float rr = r * r;
	float d;
	float w;
	float w0;
   	vec2 p = 0.5*(vec2(1.0,1.0)+IN.worldPos.xy);
    	vec4 col = vec4(0.5,0.75,1.0,0.20);
    	w0 = 0.5135/pow(r,0.96);
    	if (axis==0) {
		for (d=1.0/16.0,x=-r,p.x+=x*d;x<=r;x++,p.x+=d) { 
			w=w0*exp((-x*x)/(2.0*rr)); 
			col+=texture2D(sceneTex,p)*w; 
		}
	}
    	if (axis==1) {
		for (d=1.0/16.0,y=-r,p.y+=y*d;y<=r;y++,p.y+=d) { 
			w=w0*exp((-y*y)/(2.0*rr)); 
			col+=texture2D(sceneTex,p)*w; 
		}
	}
    	fragColor = col;
}