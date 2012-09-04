#include <stdio.h>

char toString(int i)
{
	char numbers[5];
	for(int j=0;j<=2;j++)
	{	
		numbers[j] = (char)(((int)'0')+i%10);
		i = i/10;
	}
	
	// printf("%s\n",numbers);

	XTft_SetPosChar(&TftInstance, 0,0);
	XTft_SetColor(&TftInstance, 0x00ffffff,0x000000);
	for(int j=2;j>=0;j--)
	{	
		printf("%c\n",numbers[j]);
		//The code to print numbers[j] goes here, and this is not tested
		XTft_Write(&TftInstance,numbers[j]);
	}
}

int main()
{

	int i=389;
	int j=2;
	toString(i);

}