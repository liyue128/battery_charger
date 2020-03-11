#include "main.h"
#include "timer.h"

#define PRESCALER_1 0
#define PRESCALER_60000 59999
#define THIRTY_SECOND 36000

extern u16 timer_count;
 
 void timer2_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

  //定时器TIM2初始化
  TIM_TimeBaseStructure.TIM_Period = THIRTY_SECOND; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值   
  TIM_TimeBaseStructure.TIM_Prescaler = PRESCALER_60000; //设置用来作为TIMx时钟频率除数的预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断

  //中断优先级NVIC设置
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM6中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
  NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	
  //TIM_Cmd(TIM2, ENABLE);  //使能TIMx 
}

void TIM2_IRQHandler(void)   //TIM2中断
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
    {
			if(timer_count<=0) {
				TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志
				timer_count--;
			}
    }
}

//timer reset
void TimerReset(TIM_TypeDef* TIMx, uint16_t time)
{
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	
	//将上一次计数值清零
	if(TIMx->CNT!=0) {
		TIMx->CNT=0;
	}
	//清除定时器中断标志
	if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIMx, TIM_IT_Update); 
	}
	timer_count=time;
}
