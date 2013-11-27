#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	volatile int i;
	for (i = 0; i < 100; i++)
		printf("i = %d\n", i);

		sleep(3);

		return 0;
}
