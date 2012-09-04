/*
 -----------------------------------------------------------------------------
 -- Copyright (C) 2005 IMEC                                                  -
 --                                                                          -
 -- Redistribution and use in source and binary forms, with or without       -
 -- modification, are permitted provided that the following conditions       -
 -- are met:                                                                 -
 --                                                                          -
 -- 1. Redistributions of source code must retain the above copyright        -
 --    notice, this list of conditions and the following disclaimer.         -
 --                                                                          -
 -- 2. Redistributions in binary form must reproduce the above               -
 --    copyright notice, this list of conditions and the following           -
 --    disclaimer in the documentation and/or other materials provided       -
 --    with the distribution.                                                -
 --                                                                          -
 -- 3. Neither the name of the author nor the names of contributors          -
 --    may be used to endorse or promote products derived from this          -
 --    software without specific prior written permission.                   -
 --                                                                          -
 -- THIS CODE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''           -
 -- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED        -
 -- TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A          -
 -- PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR       -
 -- CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,             -
 -- SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT         -
 -- LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF         -
 -- USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND      -
 -- ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,       -
 -- OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT       -
 -- OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF       -
 -- SUCH DAMAGE.                                                             -
 --                                                                          -
 -----------------------------------------------------------------------------
 -----------------------------------------------------------------------------
 -- File           : threads_RR.c
 -----------------------------------------------------------------------------
 -- Description    : C code
 -- --------------------------------------------------------------------------
 -- Author         : Kristof Loots
 -- Date           : 14/09/2006
 -- Version        : V1.0
 -- Change history :
 -----------------------------------------------------------------------------
*/
#include "xmk.h"
#include <pthread.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xio.h"
#include "xtft.h"

#define TFT_DEVICE_ID XPAR_XPS_TFT_0_DEVICE_ID

#define DISPLAY_COLUMNS  640
#define DISPLAY_ROWS     480

#define GREEN 0x0000ff00
#define RED 0x00ff0000
#define BLUE 0x000000ff
#define WHITE 0x00ffffff

static XTft TftInstance;

pthread_t tid1, tid2;
volatile int taskrunning;
//volatile int x,y;


float distance(int j, int k, int l, int m){
return ((j-l)*(j-l)+(k-m)*(k-m));
}

int in_circle(int a, int b, int x, int y){
if (distance(a,b,x,y)>7*7)
{
return 0;
}
else
{
return 1;
}
}


int Draw_Circle(XTft *TftInstance, int x, int y, unsigned int col){
	int Xmin = x;
	int Ymin = y;
	int Index1,Index2;
		for (Index1 = Xmin-7; Index1 <= Xmin+7; Index1++) {
			for (Index2 = Ymin-7; Index2 <= Ymin+7; Index2++) {
				if (in_circle(Index1,Index2,x,y)==1) 	   XTft_SetPixel(TftInstance, Index1, Index2, col);

			}
		}
}


void* thread_func_1 () {
	while(1) {

		//xil_printf("In thread 1\r\n");
		int x,y;
		x=rand()%639 + 1;
		y=rand()%479 + 1;

		/* Print co-ordinates on screen*/

		//xil_printf("x is %d\r\n",x);
		//xil_printf("y is %d\r\n",y);



		//a=(char)x;
		//b=(char)(y);
		//XTft_SetPos(&TftInstance, 0,0);
		//XTft_SetPosChar(&TftInstance, 0,0);
		//XTft_Write(&TftInstance,'a');
		//XTft_Write(&TftInstance,',');
		//XTft_Write(&TftInstance,'b');

		XUartLite_SendByte(STDOUT_BASEADDRESS,x);
		sleep(100);
		XUartLite_SendByte(STDOUT_BASEADDRESS,y);
		sleep(400);
	}
}


void* thread_func_2 () {
	while(1) {
		int x,y;
		x= XUartLite_RecvByte(STDIN_BASEADDRESS);
		sleep(100);
		y= XUartLite_RecvByte(STDIN_BASEADDRESS);
		//xil_printf("Drawing circle at %d,%d\r\n",100,100);
		Draw_Circle(&TftInstance,x,y, BLUE);
		xil_printf("Circle drawn in thread 2\r\n");
		sleep(200);

	}
}


void main (void) {

	int Status;
	XTft_Config *TftConfigPtr;

	/*
	* Get address of the XTft_Config structure for the given device id.
	*/
	xil_printf("Get Xtft_Config Structure: ");
	TftConfigPtr = XTft_LookupConfig(XPAR_XPS_TFT_0_DEVICE_ID);
	if (TftConfigPtr == (XTft_Config *)NULL)
	{
		return XST_FAILURE;
		xil_printf("FAIL\r\n");
	}
	else
	{
		xil_printf("DONE\r\n");
		xil_printf("BaseAddress : 0x%X\r\n",TftConfigPtr->BaseAddress);
		TftConfigPtr->VideoMemBaseAddr = XPAR_DDR_SDRAM_MPMC_HIGHADDR - 2097151;
	}

	/*
	* Initialize all the TftInstance members and fills the screen with
	* default background color.
	*/
	xil_printf("Initialize TftInstance: ");
	Status = XTft_CfgInitialize(&TftInstance, TftConfigPtr, TftConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
		xil_printf("FAIL\r\n");
	}
	else
	{
		xil_printf("DONE\r\n");
	}

	XTft_SetFrameBaseAddr(&TftInstance, XPAR_DDR_SDRAM_MPMC_HIGHADDR - 2097151);
	xil_printf("VideoMemBaseAddr : 0x%X\r\n",TftConfigPtr->VideoMemBaseAddr);

	//Start Xilkernel
	xilkernel_main ();

	//Control does not reach here

}

void main_prog(void) {  // This thread is statically created (as configured in the kernel configuration) and has priority 0 (This is the highest possible)

	int ret;

	print("-- Entering main() --\r\n");

	/* Initialize white background*/
	XTft_FillScreen(&TftInstance,  0,  0,639,479,0x00ffffff);


	//xil_printf(" Writing background white\r\n");
	//Draw_Circle(&TftInstance, GREEN); //testing drawCircle
	//xil_printf("  Made background white!\r\n");
	//start thread 1

	ret = pthread_create (&tid1, NULL, (void*)thread_func_1, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching thread_func_1...\r\n", ret);
	}
	else
	{
		xil_printf ("Thread 1 launched with ID %d \r\n",tid1);
	}

	//start thread 2
	ret = pthread_create (&tid2, NULL, (void*)thread_func_2, NULL);
	if (ret != 0)
	{
		xil_printf ("-- ERROR (%d) launching thread_func_2...\r\n", ret);
	}
	else
	{
		xil_printf ("Thread 2 launched with ID %d \r\n",tid2);
	}

}

