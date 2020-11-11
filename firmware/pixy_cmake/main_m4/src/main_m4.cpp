//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#include <string.h>
#include <stdio.h>
#include <debug.h>
#include <pixy_init.h>
#include <pixyvals.h>
#include <pixy_init.h>
#include <misc.h>
#include "camera.h"
#include "led.h"
#include "conncomp.h"
#include "line.h"
#include "exec.h"
#include "camera.h"
#include "param.h"
#include "serial.h"
#include "rcservo.h"
 

#include <new>

// M0 code 
#ifdef KEIL
const // so m0 program goes into RO memory
#include "../main_m0/m0_image.c"
#endif


Qqueue *g_qqueue = NULL;


int main(void)	 
{
    // insert a small delay so power supply can stabilize
	for (volatile uint32_t d=0; d<2500000; d++) {;}

#ifdef KEIL
 	pixyInit(SRAM3_LOC, &LR0[0], sizeof(LR0));
#else
	pixyInit();
#endif

	exec_init(g_chirpUsb);
	cam_init();

	// cc_init(g_chirpUsb);
	
		g_qqueue = new Qqueue;
	
		line_init(g_chirpUsb);
	ser_init(g_chirpUsb);
	exec_mainLoop();

}

