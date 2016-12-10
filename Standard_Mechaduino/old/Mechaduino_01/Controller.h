//Contains the TC5 Handler declaration


#ifndef __CONTROLLER_H__
#define  __CONTROLLER_H__

#define WAIT_TC16_REGS_SYNC(x) while(x->COUNT16.STATUS.bit.SYNCBUSY);


void TC5_Handler() __attribute__ ((optimize("O3")));


#endif
