#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int arc, const char* argv[])
{

	char buf[1024];
	strcpy(buf, argv[1]);
	printf("You wrote: \n %s \n", buf);
	return 0;
}