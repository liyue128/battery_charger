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

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

  //��ʱ��TIM2��ʼ��
  TIM_TimeBaseStructure.TIM_Period = THIRTY_SECOND; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ   
  TIM_TimeBaseStructure.TIM_Prescaler = PRESCALER_60000; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM2�ж�,��������ж�

  //�ж����ȼ�NVIC����
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM6�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	
  //TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx 
}

void TIM2_IRQHandler(void)   //TIM2�ж�
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
    {
			if(timer_count<=0) {
				TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־
				timer_count--;
			}
    }
}

//timer reset
void TimerReset(TIM_TypeDef* TIMx, uint16_t time)
{
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	
	//����һ�μ���ֵ����
	if(TIMx->CNT!=0) {
		TIMx->CNT=0;
	}
	//�����ʱ���жϱ�־
	if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIMx, TIM_IT_Update); 
	}
	timer_count=time;
}
