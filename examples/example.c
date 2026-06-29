#include <alcvx.h>

int main(int argc, char** argv)
{
	vec3_t a = {1,0,0};
	vec3_t b = {0,1,0};
	vec4_t c = vcross3(a,b);
	vout("",a,"",4,true); eout();
	vout("",b,"",4,true); eout();
	vout("",c,"",4,true); eout();
	exit(EXIT_SUCCESS);
}

