#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

void GPIOinit(void){
	//GPIOB init
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);     //enable GPIO clock
	GPIO_InitTypeDef GPIO_InitStruct;                         //structure reference
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;     //Pins
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;                              //Mode
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;               //Switching speed
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                   //Push Pull type
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;                   //Pull Down
	GPIO_Init(GPIOC, &GPIO_InitStruct);                         //execute structure

	//GPIOD init
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_Write(GPIOD,0x0000);
}

void AD9850_reset(void){
	GPIO_WriteBit(GPIOC, GPIO_Pin_10, Bit_RESET);           //turn off all buses
	GPIO_WriteBit(GPIOC, GPIO_Pin_11, Bit_RESET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_12, Bit_RESET);
	for (int i=0; i<5000; i++);
	GPIO_WriteBit(GPIOC, GPIO_Pin_12, Bit_SET);       //Set FU_UD High(register load pin)
	for (int i=0; i<5000; i++);
	GPIO_WriteBit(GPIOC, GPIO_Pin_10, Bit_SET);       //Pulse the RESET pin
	for (int i=0; i<5000; i++);
	GPIO_WriteBit(GPIOC, GPIO_Pin_10, Bit_RESET);
	for (int i=0; i<5000; i++);                        // Chip is RESET now
}


uint8_t W[5] = {0, 0, 0, 0, 0};                 //keep this array global
uint32_t tword;

void AD9850_ParallelSend(uint32_t freq){
	tword = (freq*34.36);            //tuning word calculation

	W[4] = tword;                              //converting to 5words*8bits
	W[3] = tword >> 8;
	W[2] = tword >> 16;
	W[1] = tword >> 24;
	W[0] = 0x00;

	GPIO_WriteBit(GPIOC, GPIO_Pin_12,Bit_RESET);     // FU_UD low
	for (int k=0;k<5;k++){
		GPIO_Write(GPIOD, W[k]);                         //send the bits
		GPIO_WriteBit(GPIOC, GPIO_Pin_11,Bit_SET);       //Pulse W_CLK
		GPIO_WriteBit(GPIOC, GPIO_Pin_11,Bit_RESET);
	}
	GPIO_WriteBit(GPIOC, GPIO_Pin_12,Bit_SET);       // FU_UD high
}

int main(void)
{
	uint32_t frequency = (20);            //desired frequency input in Hz
	GPIOinit();                               //initialize the GPIO
	AD9850_reset();                           //Reset the DDS
	AD9850_ParallelSend(frequency);           //Use this to Program the AD9850

	for (uint32_t i = 2; i < 4000000; ++i)
	{
		frequency = (i*10);
		AD9850_ParallelSend(frequency);

		//for (int j = 0; j <5; ++j);
	}
	//while(1) {
	//for (int d = 0; d<50000;d++);          //Waste some time (optional)
	//}

}


/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}
