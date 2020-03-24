


//Internal Functions
static void lpc176x_RccInit()
{

	SystemInit();

	/* Set the Vector Table base location */
	SCB->VTOR  = (BOOTLOADER_SIZE & 0x3FFFFF80);

	/* set pend exception priority */
	NVIC_SetPriority(PendSV_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

	/* init systick */
	SysTick_Config(__CORE_CLK / RT_TICK_PER_SECOND - 1);
}

static void lpc176x_IrqInit()
{

}

static void lpc176x_GpioIdleInit()
{

	//������û��ʹ�õ�IO��Ϊģ������
	//���Խ��͹��ĺ͸���EMC/EMI����
}



//External Functions
void arch_Init()
{

	//ʱ��ϵͳ��ʼ��
	lpc176x_RccInit();
	//�жϳ�ʼ��
	lpc176x_IrqInit();
	//GPIO��ʼ��
	lpc176x_GpioIdleInit();
}


void arch_IdleEntry()
{

	__WFI();
}


void arch_Reset()
{

	__set_FAULTMASK(1);
	NVIC_SystemReset();
}


