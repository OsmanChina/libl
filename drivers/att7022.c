#if ATT7022_ENABLE
#include <drivers/att7022.h>


//Private Defines
#define ATT7022_SAMPLEPOINT   		128
#define ATT7022_DATA_MASK			0x00FFFFFF
#define ATT7022_DATA_DEC			8192.0f

#define ATT7022_SPI_SPEED			200000



//Private Variables
static att7022_t att_x7022;


//Private Macros







//Internal Functions
#if ATT7022_DEBUG_ENABLE
#define ATT7022_DBGOUT				dbg_printf
#else
#define ATT7022_DBGOUT(...)
#endif



static void att7022_SpiGet()
{
	att7022_t *p = &att_x7022;
	
	p->spi = spi_Open(ATT7022_COMID, OS_TMO_FOREVER);
	spi_Config(p->spi, SPI_SCKIDLE_LOW, SPI_LATCH_2EDGE, ATT7022_SPI_SPEED);
#if SPI_SEL_ENABLE
	spi_CsSel(p->spi, ATT7022_CSID);
#endif
}


static sys_res att7022_Write(u32 nReg, u32 nData)
{
	u32 nCrc1, nCrc2, nTemp;
	att7022_t *p = &att_x7022;

	att7022_SpiGet();

	//д���ݼĴ���
	nData &= ATT7022_DATA_MASK;
	invert(&nData, 3);
	nTemp = nReg | 0x80 | (nData << 8);
	spi_Send(p->spi, &nTemp, 4);
	os_thd_slp1tick();
	//��У��1�Ĵ���
	nTemp = ATT7022_REG_WSPIData1;
	spi_TransThenRecv(p->spi, &nTemp, 1, &nCrc1, 3);
	nCrc1 &= ATT7022_DATA_MASK;
	//��У��2�Ĵ���
#if 0
	nTemp = ATT7022_REG_WSPIData2;
	spi_TransThenRecv(p->spi, &nTemp, 1, &nCrc2, 3);
	nCrc2 &= ATT7022_DATA_MASK;
#else
	nCrc2 = nCrc1;
#endif

	spi_Close(p->spi);

	if ((nData != nCrc1) || (nData != nCrc2))
	{
		ATT7022_DBGOUT("<ATT7022>WriteReg %02X Err %x %x %x", nReg, nData, nCrc1, nCrc2);
		return SYS_R_ERR;
	}
	return SYS_R_OK;
}

static u32 att7022_UgainCalibration(int nPhase)
{
	float urms, k, gain = 0;

	//��ȡ�����ѹֵ 
	urms = att7022_Read(ATT7022_REG_URmsA + nPhase);
	if (urms)
	{
		 //����ʵ�ʲ���Ĺ�����
		urms = urms / SYS_KVALUE;
		 
		//�˴���UCALI_CONSTΪ��׼����Ugain 
		k = UCALI_CONST / urms - 1;
		gain = k * POW2_23;
		if (k < 0)
			gain = POW2_24 + gain;
	}
	
	return (u32)(gain + 0.5f);
} 

static u32 att7022_IgainCalibration(int nPhase, float fIb)
{
	float irms, k, gain = 0;

	//��ȡ�����ѹֵ 
	irms = att7022_Read(ATT7022_REG_IRmsA + nPhase);
	if (irms)
	{
		irms = irms / SYS_KVALUE; 
		
		//�˴���ICALI_CONSTΪ��׼����Igain	
		k = (fIb * ICALI_MUL) / irms - 1;
		gain = k * POW2_23;
		if (k < 0)
			gain = POW2_24 + gain;
	}
	
	return (u32)(gain + 0.5f);
}

static u32 att7022_PgainCalibration(int nPhase, float fIb)
{
	float pvalue, err, pcali, pgain = 0;
	att7022_t *p = &att_x7022;

	//�����������Ĺ���
	pvalue = att7022_Read(ATT7022_REG_PA + nPhase);	
	if (pvalue > POW2_23)
		pvalue -= POW2_24;
	
	//ת���ɹ�����
	pvalue = (pvalue / 256.0f) * (3200.0f / (float)p->ec);
	
	//������
	pcali = UCALI_CONST * fIb;
	err = (pvalue - pcali) / pcali;
	if (err)
	{
		err = -err / (1 + err);
		pgain = err * POW2_23;
		if (err < 0 )
			pgain = POW2_24 + pgain;			//����Pgain
	}
	
	return (u32)(pgain + 0.5f);
}

static u32 att7022_FPhaseCaliData(int nPhase, float fIb)
{
	float phase_v = 0, att7022_pvalue, seta, err, pcali_value = 0;
	u32 nTmp = 0;
	att7022_t *p = &att_x7022;

	pcali_value = UCALI_CONST * fIb * 0.5f;					//����У������й����ʳ���
	nTmp = att7022_Read(ATT7022_REG_PA + nPhase);	//��ȡ�й�����ֵ
 	if (nTmp > POW2_23)
 		nTmp -= POW2_24;
	
	att7022_pvalue = ((float)nTmp / 256.0f) * (3200.0f / (float)p->ec); //ת���ɹ�����
	err = (att7022_pvalue - pcali_value) / pcali_value; //������
	if (err)
	{
		seta = acosf((1 + err) * 0.5f);
		seta -= PI / 3.0f;
		phase_v = seta * POW2_23;
		if (seta < 0)
			phase_v = POW2_24 + phase_v;
	}
	
	return (u32)(phase_v + 0.5f);
}







//External Functions
void att7022_Init()
{
	t_gpio_def *pDef;

	memset(&att_x7022, 0, sizeof(att7022_t));
	for (pDef = tbl_bspAtt7022[0]; pDef < tbl_bspAtt7022[1]; pDef++)
	{
		sys_GpioConf(pDef);
	}
}


sys_res att7022_Reset(att7022_cali_t *pCali)
{
	u32 nTemp;
	int i;
	att7022_t *p = &att_x7022;

	//��λATT7022B
	sys_GpioSet(gpio_node(tbl_bspAtt7022, 0), 1);
	sys_Delay(3000);				//delay 30us
	sys_GpioSet(gpio_node(tbl_bspAtt7022, 0), 0);
	os_thd_slp1tick();

	switch (pCali->HFConst)
	{
	case 35:
		p->ec = 800;
		break;
	case 16:
		p->ec = 8000;
		break;
	default:
		p->ec = 3200;
		break;
	}

	for (i = 0; i < 3; i++)
	{
		if (att7022_Write(ATT7022_CALIDATA_WEN, 0) == SYS_R_OK)
			break;
	}
	if (i >= 3)
		return SYS_R_ERR;

	att7022_Write(ATT7022_CALIDATA_CLR, 0);

	att7022_Write(ATT7022_REG_UADCPga, 0);			//��ѹͨ��ADC��������Ϊ1
	att7022_Write(ATT7022_REG_HFConst, pCali->HFConst);	//����HFConst
	nTemp = IB_VO * ISTART_RATIO * CONST_G * POW2_23;
	att7022_Write(ATT7022_REG_Istartup, nTemp);		//������������
	att7022_Write(ATT7022_REG_EnUAngle, 0x003584);	//ʹ�ܵ�ѹ�нǲ���
	att7022_Write(ATT7022_REG_EnDtIorder, 0x005678);	//ʹ��������
	att7022_Write(ATT7022_REG_EAddMode, 0);			//���������ۼ�ģʽ	
	att7022_Write(ATT7022_REG_GCtrlT7Adc, 0); 		//�ر��¶Ⱥ�ADC7���� 	
	att7022_Write(ATT7022_REG_EnlineFreq, 0x007812);	//����/г����������
	att7022_Write(ATT7022_REG_EnHarmonic, 0x0055AA);	//����/г����������

	//д�빦������
	for (i = 0; i < 3; i++)
	{
		att7022_Write(ATT7022_REG_PgainA0 + i, pCali->Pgain0[i]);
		att7022_Write(ATT7022_REG_PgainA1 + i, pCali->Pgain1[i]);
	}

	//д���ѹ����
	for (i = 0; i < 3; i++)
	{
		att7022_Write(ATT7022_REG_UgainA + i, pCali->Ugain[i]);
	}

	//д���������
	for (i = 0; i < 3; i++)
	{
		att7022_Write(ATT7022_REG_IgainA + i, pCali->Igain[i]);
	}

	//д����λ������
	for (i = 0; i < 5; i++)
	{
		att7022_Write(ATT7022_REG_PhsregA0 + i, pCali->PhsregA[i]);
		att7022_Write(ATT7022_REG_PhsregB0 + i, pCali->PhsregB[i]);
		att7022_Write(ATT7022_REG_PhsregC0 + i, pCali->PhsregC[i]);
	}

	att7022_Write(ATT7022_CALIDATA_WEN, 1);

	os_thd_sleep(1600);
	
	return SYS_R_OK;
}

u32 att7022_Read(u32 nReg)
{
	u32 nData, nCrc;
	att7022_t *p = &att_x7022;

	att7022_SpiGet();

	//�����ݼĴ���
	spi_TransThenRecv(p->spi, &nReg, 1, &nData, 3);
	os_thd_slp1tick();
	invert(&nData, 3);
	nData &= ATT7022_DATA_MASK;
	
	//��У��Ĵ���	
	nReg = ATT7022_REG_RSPIData;
	spi_TransThenRecv(p->spi, &nReg, 1, &nCrc, 3);
	invert(&nCrc, 3);
	nCrc &= ATT7022_DATA_MASK;

	spi_Close(p->spi);

	if (nData != nCrc)
	{
		ATT7022_DBGOUT("<ATT7022>ReadReg %02X Err %x %x", nReg, nData, nCrc);
		nData = 0;
	}
	
	return nData;
}


const u8 tbl_att7022RegEnergy[6] = {
	ATT7022_REG_PosEpA2,
	ATT7022_REG_NegEpA2,
	ATT7022_REG_PosEqA2,
	ATT7022_REG_NegEqA2,
	ATT7022_REG_NegEqA2,
	ATT7022_REG_PosEqA2,
};

u32 att7022_GetEnergy(int nPhase, int nType)
{

	return att7022_Read(tbl_att7022RegEnergy[nType] + nPhase);
}

//------------------------------------------------------------------------
//��	��:  att7022_GetFreq ()
//��	��: 
//��	��:  -
//��	��:  -
//��	��:  Ƶ��ֵ
//��	��:  ��ȡƵ��ֵ
//------------------------------------------------------------------------
float att7022_GetFreq()
{
	s32 nData;

	nData = att7022_Read(ATT7022_REG_Freq);
	if (nData != (-1))
		return (nData / ATT7022_DATA_DEC);
	
	return 0;
}


//------------------------------------------------------------------------
//��	��: att7022_GetVoltage ()
//��	��: 
//��	��: nPhase - ��λֵ
//��	��: -
//��	��: ��ѹ��Чֵ
//��	��: ��ȡ��ǰ��ѹ
//------------------------------------------------------------------------
float att7022_GetVoltage(int nPhase)
{
	u32 nVoltage;
	float fResult;

	nVoltage = att7022_Read(ATT7022_REG_URmsA + nPhase);
	fResult = (float)nVoltage / ATT7022_DATA_DEC;
	
	return fResult;
}



//------------------------------------------------------------------------
//��	��: att7022_GetCurrent ()
//��	��: 
//��	��: nPhase - ��λֵ
//��	��: -
//��	��: ������Чֵ
//��	��: ��ȡ��ǰ����
//------------------------------------------------------------------------
float att7022_GetCurrent(int nPhase)
{
	u32 nCurrent;
	float fResult;

	nCurrent = att7022_Read(ATT7022_REG_IRmsA + nPhase);
	fResult = (float)nCurrent / (ICALI_MUL * ATT7022_DATA_DEC);
	return fResult;
}


float att7022_GetPower(u32 nReg, int nPhase)
{
	float fResult, eck;
	
	eck = 3200.0f / (float)att_x7022.ec; //�������ϵ��
	fResult = att7022_Read(nReg + nPhase);
	
	if (fResult > POW2_23)
		fResult -= POW2_24;
	
	if (nPhase < 3)
	{
		//��ȡ���๦��
		fResult = fResult * eck / 256000.0f; //ת���ɹ�����
	}
	else
	{
		//��ȡ���๦��
		fResult = fResult * eck / 64000.0f; //ת���ɹ�����
	}
	
	return fResult;
}

u32 att7022_GetFlag() 
{
	
	return att7022_Read(ATT7022_REG_SFlag); //��ȡ״̬�Ĵ��� 
}
//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
u32 att7022_GetPowerDir()
{

	//Bit0-3 �ֱ��ʾA B  C  ������й����ʵķ���	0 ��ʾΪ��	1 ��ʾΪ�� 
	//Bit4-7 �ֱ��ʾA B  C  ������޹����ʵķ��� 	0 ��ʾΪ��	1 ��ʾΪ�� 
	return att7022_Read(ATT7022_REG_PFlag); //��ȡ���ʷ���Ĵ���
}


//------------------------------------------------------------------------
//��	��:  att7022_GetPFV ()
//��	��: 
//��	��:  nPhase - ��λֵ
//��	��:  -
//��	��:  ��������
//��	��:  ��ȡ��������
//------------------------------------------------------------------------
float att7022_GetPFV(int nPhase)
{
	int nData = 0;
	float f_data = 0.0f;

	nData = att7022_Read(ATT7022_REG_PfA + nPhase);
	if (nData > POW2_23)
		nData -= POW2_24;
	
	f_data = (float)nData / POW2_23;
	
	return f_data;
}
//------------------------------------------------------------------------
//��	��:  att7022_GetPAG ()
//��	��: 
//��	��: nPhase - ��λֵ
//��	��: -
//��	��: �����ڵ�ѹ�ļн�
//��	��: ��ȡ�Ƕ�
//------------------------------------------------------------------------
float att7022_GetPAG(int nPhase) 
{
	float sita;
	
	sita = att7022_Read(ATT7022_REG_PgA + nPhase);
	if (sita > POW2_23)
		sita -= POW2_24;
	
	return (sita * 360 / POW2_23 / PI);
}

//-------------------------------------------------------------------------
//��ѹ���
//-------------------------------------------------------------------------
float att7022_GetPVAG(int nPhase) 
{

	return ((float)att7022_Read(ATT7022_REG_YUaUb + nPhase) / 8192.0f);
}

u32 att7022_GetQuanrant(int Phase) 
{
	u32 pflag;
	u32 p_direction, q_direction;

	pflag = att7022_Read(ATT7022_REG_PFlag); //�ȶ�ȡ���ʷ���Ĵ���(����Ϊ0,����Ϊ1)
	
	p_direction = ((pflag >> Phase) & 0x00000001);
	q_direction = ((pflag >> (Phase + 4)) & 0x00000001);
	if (p_direction)
	{
		if (q_direction)
		{
			//P- Q-
			return 3; //ATT7022B  3
		}
		else
		{
			//P- Q+
			return 2; //ATT7022B  2
		}
	}
	else
	{
		if (q_direction)
		{
			//P+ Q-
			return 4; //ATT7022B  4
		}
		else
		{
			//P+ Q+
			return 1; //ATT7022B  1
		}
	}
}


/**************************
 * ����:			att7022_GetHarmonic
 * ����:			��ȡ��ͨ��г��
 * �������:		Ch(0--B)  �ֱ��Ӧ�����ͨ����������3.2k
 Ua\Ia\Ub\Ib\Uc\Ic\In\Ua+Ia\Ub+Ib\Uc+Ic\Ua+Ub+Uc\Ia+Ib+Ic
 * ����ֵ:			
 * ����˵��:		RVMDK 3.01a
***************************/
sys_res att7022_GetHarmonic(int Ch, s16 *pbuf)
{
	int i;
	u32 nData, nReg = 0x7F;
	att7022_t *p = &att_x7022;

	//������������
	nData=0xCCCCC0|Ch;
	att7022_Write(0xC0, nData);//�����������ݻ���
	//�ȴ������������
#if 0
	os_thd_sleep(200);
#else
	for (i = 0; i < 20; i++)
	{   //һ���ظ�3�ξ�����?
		if(att7022_Read(0x7E) >= 240)//��һ��д���ݵ�λ��
			break;
		
		os_thd_sleep(20);
	}
#endif
	if (i < 20)
	{
		//�����û���ȡָ�����ʼλ��
		att7022_Write(0xC1, 0);
		
		att7022_SpiGet();
		for (i = 0; i < 240; i++)
		{
			spi_TransThenRecv(p->spi, &nReg, 1, &nData, 3);//��ȡ����
			invert(&nData, 3);
			nData &= ATT7022_DATA_MASK;
			if (i < ATT7022_SAMPLEPOINT)
				pbuf[i] = (s16)(nData >> 8);
		}
		spi_Close(p->spi);

		return SYS_R_OK;
	}
	
	return SYS_R_ERR;
}

int att7022_GetSig()
{

	return sys_GpioGet(gpio_node(tbl_bspAtt7022, 1));
}


u16 att7022_GetEConst()
{
	
	return att_x7022.ec;
}


//------------------------------------------------------------------------
//��	��: BSP_ATT7022B_UIP_gainCalibration () 
//��	��: 
//��	��: -
//��	��: -
//��	��: -
//��	��: У��U��I��P����
//------------------------------------------------------------------------
void att7022_CaliUIP(att7022_cali_t *pCali)
{
	int i, j;
	u32 phv;

	for (i = 0; i < 3; i++)
	{
		//��ѹͨ��У��
		phv = 0;
		
		for (j = 0; j < 8; j++)
		{
			phv += att7022_UgainCalibration(PHASE_A + i);
			os_thd_sleep(250);
		}
		phv /= 8;
		pCali->Ugain[i] = phv;
		
		//����ͨ��У��
		phv = 0;
		for (j = 0; j < 8; j++)
		{
			phv += att7022_IgainCalibration(PHASE_A + i, 1.5f);
			os_thd_sleep(250);
		}
		phv /= 8;
		pCali->Igain[i] = phv;   
		
		//����У��
		phv = 0;
		for (j = 0; j < 8; j++)
		{
			phv += att7022_PgainCalibration(PHASE_A + i, 1.5f);
			os_thd_sleep(250);
		}
		phv /= 8;
		pCali->Pgain0[i] = phv;
		pCali->Pgain1[i] = phv;
	}
}
//------------------------------------------------------------------------
//��	��: BSP_ATT7022B_Phase_gainCalibration () 
//��	��: 
//��	��: -
//��	��: -
//��	��: -
//��	��: ��λ��У��,��������Ϊ  0.5L  ��������
//------------------------------------------------------------------------
void att7022_CaliPhase(att7022_cali_t *pCali)
{
	int i;
	u32 phv;

	phv = 0;
	for (i = 0; i < 8; i++)
	{
		phv += att7022_FPhaseCaliData(PHASE_A, 1.5f); //1.5A��У��
		os_thd_sleep(250);
	}
	
	phv /= 8;
	for (i = 0; i < 5; i++)
	{
		pCali->PhsregA[i] = phv;   
	}

	phv = 0;
	for (i = 0; i < 8; i++)
	{
		phv += att7022_FPhaseCaliData(PHASE_B, 1.5f); //1.5A��У��
		os_thd_sleep(250);
	}
	
	phv /= 8;
	for (i = 0; i < 5; i++)
	{
		pCali->PhsregB[i] = phv;  
	}

	phv = 0;
	for (i = 0; i < 8; i++)
	{
		phv += att7022_FPhaseCaliData(PHASE_C, 1.5f); //1.5A��У��
		os_thd_sleep(250);
	}
	
	phv /= 8;
	for (i = 0; i < 5; i++)
	{
		pCali->PhsregC[i] = phv;  
	}
}



#endif

