#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) 
{
	sleep(10);
	if (argv[1] == NULL)
	{
		printf("\nI gots no arguments on me! Also %s\n", argv[0]);

	}
	else if(argv[2] == NULL)
	{
		printf("\nI gots %d arguments on me! ==> %s\n",argc-1, argv[1]);
	}
	else
	{
		printf("\nI gots %d arguments on me! ==> %s and %s\n",argc-1,argv[1],argv[2]);	
	}
	return (52);
} 
