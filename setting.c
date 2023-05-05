#include <nds32_intrinsic.h>
#include "WT58F2C9.h"
#include "gpio.h"


void delay1(unsigned int nCount)
{
   volatile unsigned int i;
   for (i=0;i<nCount;i++);
}


extern void DRV_Printf(char *pFmt, U16 u16Val);


inline void GIE_ENABLE();

void DRV_EnableHWInt (void)
{
	/* enable SW0, HW0 and HW1 */
	__nds32__mtsr(0x10003, NDS32_SR_INT_MASK);
	/* Enable SW0 */
	//__nds32__mtsr(0x10000, NDS32_SR_INT_MASK);
	/* Enable global interrupt */
	GIE_ENABLE();
}

void DRV_BlockIntDisable(void)
{
	// Disable all interrupt
	OUTW(rINT0_IE0_ENABLE, 0x0000);
	OUTW(rINT0_IE1_ENABLE, 0x0000);
	OUTW(rINT0_IE2_ENABLE, 0x0000);
	OUTW(rINT1_IE0_ENABLE, 0x0000);
	OUTW(rINT1_IE1_ENABLE, 0x0000);
	OUTW(rINT1_IE2_ENABLE, 0x0000);
}

void DRV_IntInitial(void)
{
	// Disable all interrupt
	DRV_BlockIntDisable();

	// Enable all HW interrupt
	DRV_EnableHWInt();				//Enable global Hardware interrupt, Assembly command

	// Enable default Block interrupt
	//DRV_BlockIntEnable();			//Enable each block device interrupt

}

#define rSYS_OPTION1	(0x00200004)

void DRV_SysXtal(U8 u8XtalMode)
{

	#if(EXTERNAL_XTAL == XTAL_MODE)
		//-----External Crystal
		//-----24MHz
		OUTW(rSYS_OPTION1,((INW(rSYS_OPTION1)&0xFFFFFF00) | 0x0012)); //Use HXTAL and divide 2
		//-----Crystal 12MHz
		//OUTW(rSYS_OPTION1, (INW(rSYS_OPTION1) | 0x000A));

		//OUTW(rSYS_OPTION1,(INW(rSYS_OPTION1) | (XTAL<<2) | (HSE_OSC_ON <<1)));
		//OUTW(rSYS_CLOCK_SELECT,(INW(rSYS_CLOCK_SELECT) | 0x0001));
	#else
		//-----Internal RC
		//-----24MHz
		//OUTW(rSYS_OPTION1,(INW(rSYS_OPTION1) | 0x0012));
		//-----Crystal 12MHz
		//OUTW(rSYS_OPTION1, (INW(rSYS_OPTION1) | 0x000A));

		//OUTW(rSYS_OPTION1,(INW(rSYS_OPTION1) | (XTAL<<2)));
		//OUTW(rSYS_CLOCK_SELECT,(INW(rSYS_CLOCK_SELECT));
	#endif
	//-----MCU Clock Output Test
	//OUTW(rSYS_OPTION3,0x0090);
}

void OS_PowerOnDriverInitial(void)
{
	//========================= << Typedef Initial  >>
	//SYS_TypeDefInitial();
	//========================= << System Clock Initial >>
	//-----External Crystal
	DRV_SysXtal(EXTERNAL_XTAL);
	//========================= << Interrupt Initial >>
	DRV_IntInitial();
	//========================= << GPIO Initial >>
	//DRV_GpioInitial();
	//========================= << UART Initial >>
	//DRV_UartInitial();
	//========================= << Timer Initial >>
	//DRV_TimerInitial(TIMER_0, SIMPLE_TIMER);
	//DRV_TimerInitial(TIMER_1, SIMPLE_TIMER);
	//========================= << PWM Initial >>
	//DRV_PwmInitial();
	//========================= << SPI Initial >>
	//DRV_SpiInitial(SPI_CH2);
	//========================= << Watchdog Initial >>
}



int main()
{
	OS_PowerOnDriverInitial();
//	七段顯示器 A[15:13]第幾位數顯示
//  switch A[11:8]開關輸入
//  LED B[15:0]
//  七段顯示器 D[15:8][7:0]數字顯示
//  A port input
//  B port output
//  D port output
	GPIO_PTA_PADINSEL = 0x0000;// [15:13]output [11:8]input ok
	GPIO_PTA_CFG = 0x0000;// [15:13]output [11:8]input ok
	GPIO_PTA_DIR = 0x0FFF;// [15:13]output [11:8]input ok

	GPIO_PTB_PADINSEL = 0x0000;//LED ok
	GPIO_PTB_CFG = 0xFFFF;//LED ok
	GPIO_PTB_DIR = 0x0000;//LED ok

	GPIO_PTD_PADINSEL = 0x0000;//ok
	GPIO_PTD_CFG = 0x0000;//ok
	GPIO_PTD_DIR = 0x0000;//ok
	int LED=0x0001;
	int segindex[8] = {Digit_1,Digit_2,Digit_3,Digit_4,Digit_5,Digit_6,Digit_7,Digit_8};
	int segnum[12] = {Number_0,Number_1,Number_2,Number_3,Number_4,Number_5,Number_6,Number_7,Number_8,Number_9,Number_H,0x0000};
	int date[8] = {Number_2,Number_0,Number_2,Number_3,Number_0,Number_3,Number_2,Number_3};
	int H_flash[8] = {0x7676,0x7676,0x7676,0x7676,0x0000,0x0000,0x0000,0x0000};
	int SW18=0,SW19=0,SW20=0,SW21=0;
	while(1){

		SW18 = GPIO_PTA_PADIN&0x0800;
		SW19 = GPIO_PTA_PADIN&0x0400;
		SW20 = GPIO_PTA_PADIN&0x0200;
		SW21 = GPIO_PTA_PADIN&0x0100;

		if(SW19 == 0){
			if(LED==0x8000){
				LED = 0x0001;
			}
			else{
				LED = LED<<1;
			}
			GPIO_PTB_GPIO = 0xffff-LED;
			GPIO_PTD_GPIO = segnum[11];
			delay1(200000);
		}

		else if(SW18 == 0){
			if(LED==0x0001){
				LED = 0x8000;
			}
			else{
				LED = LED>>1;
			}
			GPIO_PTB_GPIO = 0xffff-LED;
			GPIO_PTD_GPIO = segnum[11];
			delay1(200000);
		}

		if(SW20 == 0){
			while(SW18 != 0&&SW19 != 0&&SW21 != 0){
				SW18 = GPIO_PTA_PADIN&0x0800;
				SW19 = GPIO_PTA_PADIN&0x0400;
				SW20 = GPIO_PTA_PADIN&0x0200;
				SW21 = GPIO_PTA_PADIN&0x0100;
				int temp=0;
				for(temp=0;temp<8;temp = temp+1){
					GPIO_PTD_GPIO = date[temp];
					GPIO_PTA_GPIO = segindex[7-temp];
					delay1(1000);
				}

			}
		}
		if(SW21 == 0){
			while(SW18 != 0&&SW19 != 0&&SW20 != 0){
				SW18 = GPIO_PTA_PADIN&0x0800;
				SW19 = GPIO_PTA_PADIN&0x0400;
				SW20 = GPIO_PTA_PADIN&0x0200;
				SW21 = GPIO_PTA_PADIN&0x0100;
				int temp=0;
				int cnt = 0;
				while(cnt<10000){
					for(temp=0;temp<8;temp = temp+1){
						GPIO_PTD_GPIO = H_flash[temp];
						GPIO_PTA_GPIO = segindex[7-temp];

					}
					cnt++;
				}
				cnt = 0;
				while(cnt<10000){
					for(temp=0;temp<8;temp = temp+1){
						GPIO_PTD_GPIO = segnum[10];
						GPIO_PTA_GPIO = segindex[7-temp];

					}
					cnt++;
				}
			}
		}
		else{
		}




	}


	return 0;
}