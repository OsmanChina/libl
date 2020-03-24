





//Internal Functions
static GPIOMode_TypeDef stm32_GpioMode(int nMode)
{

	switch (nMode) {
	case GPIO_M_IN_ANALOG:
		return GPIO_Mode_AIN;
	case GPIO_M_OUT_OD:
	case GPIO_M_OUT_PP:
		return GPIO_Mode_OUT;
	case GPIO_M_AF_OD:
	case GPIO_M_AF_PP:
		return GPIO_Mode_AF;
	default :
		return GPIO_Mode_IN;
	}
}

static GPIOOType_TypeDef stm32_GpioOType(int nMode)
{

	switch (nMode) {
	case GPIO_M_AF_PP:
	case GPIO_M_OUT_PP:
		return GPIO_OType_PP;
	default :
		return GPIO_OType_OD;
	}
}

static GPIOPuPd_TypeDef stm32_GpioPuPd(int nMode)
{
	switch (nMode) {
	case GPIO_M_IN_PD:
		return GPIO_PuPd_DOWN;
	case GPIO_M_IN_PU:
		return GPIO_PuPd_UP;
	default :
		return GPIO_PuPd_NOPULL;
	}
}

//External Functions
void stm32_GpioClockEnable(int nPort)
{

	switch (nPort) {
	case GPIO_P0:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		break;
	case GPIO_P1:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		break;
	case GPIO_P2:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		break;
	case GPIO_P3:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		break;
	case GPIO_P4:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		break;
	case GPIO_P5:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
		break;
	case GPIO_P6:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
		break;
	case GPIO_P7:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
		break;
	case GPIO_P8:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
		break;
	default:
		break;
	}
}

GPIO_TypeDef *arch_GpioPortBase(int nPort)
{

	switch (nPort) {
	case GPIO_P0:
		return GPIOA;
	case GPIO_P1:
		return GPIOB;
	case GPIO_P2:
		return GPIOC;
	case GPIO_P3:
		return GPIOD;
	case GPIO_P4:
		return GPIOE;
	case GPIO_P5:
		return GPIOF;
	case GPIO_P6:
		return GPIOG;
	case GPIO_P7:
		return GPIOH;
	case GPIO_P8:
		return GPIOI;
	default:
		return NULL;
	}
}

void arch_GpioSel(int nPort, int nPin, int GPIO_AF)
{
	
	GPIO_PinAFConfig(arch_GpioPortBase(nPort),  nPin, GPIO_AF);
}

void arch_GpioConf(int nPort, int nPin, int nMode, int nInit)
{
	GPIO_InitTypeDef xGpio;

	xGpio.GPIO_Pin = BITMASK(nPin);
	xGpio.GPIO_Mode = stm32_GpioMode(nMode);
	xGpio.GPIO_OType = stm32_GpioOType(nMode);
	xGpio.GPIO_PuPd = stm32_GpioPuPd(nMode);
	xGpio.GPIO_Speed = GPIO_Speed_50MHz;
	stm32_GpioClockEnable(nPort);
	
	if (nMode & GPIO_M_OUT_MASK)
	{
		switch (nInit)
		{
		case GPIO_INIT_HIGH:
			arch_GpioSet(nPort, nPin, 1);
			break;
		case GPIO_INIT_LOW:
			arch_GpioSet(nPort, nPin, 0);
			break;
		default:
			break;
		}
	}
	
	GPIO_Init(arch_GpioPortBase(nPort), &xGpio);
}

void arch_GpioSet(int nPort, int nPin, int nHL)
{

	if (nHL)
		GPIO_SetBits(arch_GpioPortBase(nPort), BITMASK(nPin));
	else
		GPIO_ResetBits(arch_GpioPortBase(nPort), BITMASK(nPin));
}

int arch_GpioRead(int nPort, int nPin)
{

	return GPIO_ReadInputDataBit(arch_GpioPortBase(nPort), BITMASK(nPin));
}



