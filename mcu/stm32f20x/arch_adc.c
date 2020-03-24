#if OS_TYPE

void arch_AdcInit()
{
	ADC_CommonInitTypeDef ADC_CommonInitStruct;

 	/* Enable ADC3 clock                                                        */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	ADC_CommonStructInit(&ADC_CommonInitStruct);
	ADC_CommonInit(&ADC_CommonInitStruct);
}


u32 arch_AdcData(int nPort, int nPin)
{
	u32 nVol;
	ADC_InitTypeDef ADC_InitStructure;

	ADC_StructInit(&ADC_InitStructure);

	ADC_Init(ADC3, &ADC_InitStructure);											// ����ADC1 ��ʼ��
	ADC_RegularChannelConfig(ADC3, ADC_Channel_4, 1, ADC_SampleTime_480Cycles);     //����ADC1 �Ĺ���ͨ����ת��˳��Ͳ���ʱ��

	/* Enable ADC1 ------------------------------------------------------*/
	ADC_Cmd(ADC3, ENABLE);
	
	nVol = ADC3->DR;
	
 	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConv(ADC3);
	
	return nVol;
}

#endif


