#include <MKL25Z4.H>

void Delay (uint32_t dly) {
  volatile uint32_t t;

	for (t=dly*100; t>0; t--)
		;
}
// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
