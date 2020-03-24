
#if ATSVR_ENABLE
#include <drivers/atserver.h>



//Private Defines


//Private Macros
#if ATSVR_DEBUG_ENABLE
#define ATSVR_DBGOUT				dbg_trace
#else
#define ATSVR_DBGOUT(...)
#endif

//Private Variables
static atsvr_t xATServer = {0};

/* Macros. */
#define AT							(&xATServer)

//Const Varialbes
#if ATSVR_CMUX_ENABLE
static const u8 CMUX_HEADER = 0xF9;

static void atsvr_CmuxSend(uart_t *p, u8 adr, u8 c, const void *data, size_t len)
{
	u8 buf[4], fcs;
	u8 *ptmp = (u8 *)data;
	int send, once;

	buf[0] = adr;
	buf[1] = c;

	if (len == 0)
		once = 1;

	for (; once || len; len -= send, ptmp += send)
	{
		once = 0;
		
		send = MIN(0x1F, len);
		buf[2] = (send << 1) + 1;
		fcs = ~fcs8(buf, 3);

		uart_Send(p, &CMUX_HEADER, 1);
		uart_Send(p, buf, 3);
		uart_Send(p, ptmp, send);
		uart_Send(p, &fcs, 1);
		uart_Send(p, &CMUX_HEADER, 1);

#if 0 && ATSVR_DEBUG_ENABLE
		{
			int i;
			char str[8];

			ATSVR_DBGOUT("[CMUX>] ");
			for (i = 0; i < 3; i++)
			{
				sprintf(str, "%02X ", buf[i]);
				ATSVR_DBGOUT(str);
			}
			for (i = 0; i < send; i++)
			{
				sprintf(str, "%02X ", ptmp[i]);
				ATSVR_DBGOUT(str);
			}
			ATSVR_DBGOUT(STRING_0D0A);
		}
#endif
	}
}
#endif



#if ATSVR_PPP_ENABLE
#include <net/bdip/ppp.c>
#endif



static void atsvr_SendStr(atsvr_t *p, const char *str)
{

#if ATSVR_CMUX_ENABLE
	if (p->cmux)
	{
		buf b = {0};

		buf_Push(b, STRING_0D0A, 2);
		buf_Push(b, str, strlen(str));
		atsvr_CmuxSend(p->uart, ((p->dlci + 1) << 2) | 0x03, 0xEF, b->p, b->len);
		buf_Release(b);
	}
	else
#endif
	{
		uart_SendStr(p->uart, STRING_0D0A);
		uart_SendStr(p->uart, str);
	}

	ATSVR_DBGOUT(STRING_0D0A);
	ATSVR_DBGOUT("[AT>] ");
	ATSVR_DBGOUT(str);
}


static int atsvr_Reply(atsvr_t *p, buf b)
{
	char str[64];
	char *cmd;
	int i, signal, sim;
#if ATSVR_ATRW_ENABLE
	char *tmp;
#endif

	cmd = (char *)&b->p[2];

	sim = rdss_GetSIM();
	signal = rdss_GetSignal();

	//��չATָ��,���ñ���ָ�ӿ���
	if (memscmp(cmd, "+GFBDC=") == 0)
		atsvr_BDId4ATCmd(cmd + 7);
	if (memscmp(cmd, "+GFBDC?") == 0)
	{
		sprintf(str, "+GFBDC: %X", rdss_GetDest());
		atsvr_SendStr(p, str);
	}

#if ATSVR_PPP_ENABLE
	//����,��������ģʽ ATD
	if (memscmp(cmd, "D") == 0)
	{
		atsvr_SendStr(p, "CONNECT");
		p->ppp = p->dlci + 1;
	}
#endif

#if 1
	if (memscmp(cmd, "+CGSN") == 0)
		atsvr_SendStr(p, "+CGSN:\"867703030536032\"");

	if (memscmp(cmd, "+CFUN?") == 0)
		atsvr_SendStr(p, "+CFUN: 1");

	if (memscmp(cmd, "+CGDCONT?") == 0)
		atsvr_SendStr(p, "+CGDCONT: 1,\"IP\",\"CMNET\"");

	if (memscmp(cmd, "+CMEE?") == 0)
		atsvr_SendStr(p, "+CMEE: 2");

	if (memscmp(cmd, "+CNUM?") == 0)
		return 0;
	
	if (memscmp(cmd, "+CCLK?") == 0)
		atsvr_SendStr(p, "+CCLK: \"04/03/19\"");

	if (memscmp(cmd, "$MYNETURC?") == 0)
		atsvr_SendStr(p, "$MYNETURC: 0");
	
	if (memscmp(cmd, "$MYIPFILTER=?") == 0)
		atsvr_SendStr(p, "$MYIPFILTER=0");

	if (memscmp(cmd, "$MYNETCON?") == 0)
		atsvr_SendStr(p, "$MYNETCON: 0");
	
	if (memscmp(cmd, "$MYURCSYSINFO?") == 0)
		atsvr_SendStr(p, "$MYURCSYSINFO: 0");
#endif	
	
#if ATSVR_CMUX_ENABLE
	//�����·����ģʽ
	if (memscmp(cmd, "+CMUX=0") == 0)
		return 2;
#endif

	//��ȡģ������汾 $MYGMR
	if (memscmp(cmd, "$MYGMR") == 0)
	{
		atsvr_SendStr(p, "$MYGMR");
		atsvr_SendStr(p, atsvr_GMR());
	}
	if (memscmp(cmd, "I") == 0)
	{
		atsvr_SendStr(p, "ATI");
		atsvr_SendStr(p, atsvr_GMR());
	}
	if (memscmp(cmd, "+GMR") == 0)
	{
		atsvr_SendStr(p, "+GMR");
		atsvr_SendStr(p, atsvr_GMR());
	}

	//��ȡSIM �����к� $MYCCID
	if (memscmp(cmd, "$MYCCID") == 0)
	{
		sprintf(str, "$MYCCID: \"898601%014X\"", sim);
		atsvr_SendStr(p, str);
	}

	//�鿴SIM ����״̬+CPIN?
	if (memscmp(cmd, "+CPIN?") == 0)
	{
		if (sim == 0)
			return 0;

		atsvr_SendStr(p, "+CPIN: READY");
	}

	//��ȡ��ǰ����ע��״̬+CREG
	if (memscmp(cmd, "+CREG?") == 0)
		atsvr_SendStr(p, "+CREG: 0,1");
	if (memscmp(cmd, "+CGREG?") == 0)
		atsvr_SendStr(p, "+CGREG: 0,5");

	//��ȡ�����ƶ��û�ʶ����+CIMI
	if (memscmp(cmd, "+CIMI") == 0)
		atsvr_SendStr(p, "+CIMI: 460029202075769");

	//����ѡ��+COPS
//	if (memscmp(cmd, "+COPS?") == 0)
//		atsvr_SendStr(p, "+COPS: 0,0,\"China Moblie\"");

	//��ȡ�ź�+CSQ
	if (memscmp(cmd, "+CSQ") == 0)
	{
		i = signal ? (7 * signal) : 99;
		sprintf(str, "+CSQ: %02d,1", i);
 		
		atsvr_SendStr(p, str);
	}

	//��ѯGPRS����
	if (memscmp(cmd, "+CGATT?") == 0)
		atsvr_SendStr(p, "+CGATT: 1");

	//��ѯͨ��ģ�鼰��Ϣ���� $MYMODEM
	if (memscmp(cmd, "$MYMODEM?") == 0)
		atsvr_SendStr(p, "$MYMODEM: 03,0001");
	
	//��ѯԶ��ͨѶģ������ $MYTYPE
	if (memscmp(cmd, "$MYTYPE?") == 0)
		atsvr_SendStr(p, "$MYTYPE: 03,01,00");

	//����/ȥ����PDP �������� $MYNETACT
	if (memscmp(cmd, "$MYNETACT?") == 0)
		atsvr_SendStr(p, "$MYNETACT: 0,1,\"127.0.0.1\"");

	//��������ѡ��ģʽ$MYNETINFO
	if (memscmp(cmd, "$MYNETINFO?") == 0)
		atsvr_SendStr(p, "$MYNETINFO: 1");

	//��ȡ��ǰ������ע����Ϣ$MYSYSINFO
	if (memscmp(cmd, "$MYSYSINFO") == 0)
		atsvr_SendStr(p, "$MYSYSINFO: 2,01");

#if ATSVR_ATRW_ENABLE
	//���÷������ $MYNETSRV
	if (memscmp(cmd, "$MYNETSRV=") == 0)
	{
		tmp = memfind(cmd, 21, "\"");
		if (tmp)
		{
			//��ȡĿ��IP
			sscanu8(tmp, ".", p->dip, 4);
#if ATSVR_BDID_ENABLE
			atsvr_BDId4IP(p->dip);
#endif
		}
	}

	//�������� $MYNETOPEN
	if (memscmp(cmd, "$MYNETOPEN=") == 0)
	{
		p->soc = atoi(cmd + 11);
		sprintf(str, "$MYNETOPEN: %d,1460", p->soc);
		atsvr_SendStr(p, str);
	}

	//��ѯ����״̬ $MYNETOPEN?
	if (memscmp(cmd, "$MYNETOPEN?") == 0)
	{
		sprintf(str, "$MYNETOPEN: %d,,,,,,0,\"%d.%d.%d.%d\",0", p->soc, p->dip[0], p->dip[1], p->dip[2], p->dip[3]);
		atsvr_SendStr(p, str);
	}

	//ACK��ѯ $MYNETACK
	if (memscmp(cmd, "$MYNETACK=") == 0)
	{
		sprintf(str, "$MYNETACK: %d,0,1460", p->soc);
		atsvr_SendStr(p, str);
	}

	//�������� $MYNETWRITE
	if (memscmp(cmd, "$MYNETWRITE=") == 0)
	{
		if (signal == 0)
			return 0;

		tmp = memfind(cmd, 16, ",");
		if (tmp)
		{
			uart_Recive(p->uart, b);
			buf_Release(b);
			
			i = atoi(tmp);
			sprintf(str, "$MYNETWRITE: %d,%d\r\n", p->soc, i);
			atsvr_SendStr(p, str);

			if (uart_RecLen(p->uart, b, i, 2000) >= i)
				atsvr_RxDo(b->p, b->len);

#if 1 && ATSVR_DEBUG_ENABLE
			{
				char dstr[8];
				
				sprintf(dstr, "%d\r\n", b->len);
				ATSVR_DBGOUT("[AT<] Rx len ");
				ATSVR_DBGOUT(dstr);
				for (i = 0; i < b->len; i++)
				{
					sprintf(dstr, "%02X ", b->p[i]);
					ATSVR_DBGOUT(dstr);
				}
				ATSVR_DBGOUT(STRING_0D0A);
			}
#endif
			
			buf_Release(b);
		}
	}

	//��ȡ���� $MYNETREAD
	if (memscmp(cmd, "$MYNETREAD=") == 0)
	{
		tmp = memfind(cmd, 15, ",");
		if (tmp)
		{
			i = MIN(p->tbuf->len, atoi(tmp));
			sprintf(str, "$MYNETREAD: %d,%d\r\n", p->soc, i);
			atsvr_SendStr(p, str);
			
			uart_Send(p->uart, p->tbuf->p, i);

#if 1 && ATSVR_DEBUG_ENABLE
			{
				int di;

				sprintf(str, "%d\r\n", i);
				ATSVR_DBGOUT("[AT>] Tx len ");
				ATSVR_DBGOUT(str);
				for (di = 0; di < i; di++)
				{
					sprintf(str, "%02X ", p->tbuf->p[di]);
					ATSVR_DBGOUT(str);
				}
				ATSVR_DBGOUT(STRING_0D0A);
			}
#endif

			buf_Remove(p->tbuf, i);
		}
	}
#endif

	return 1;
}

static void atsvr_RxAT(atsvr_t *p, buf b)
{
	int res;

	for (; b->len > 2; buf_Remove(b, 1))
	{
		if (memcmp(b->p, "AT", 2))
			continue;

		if (buffstr(b, "\r") == NULL)
			break;

		AT->ste = 1;

		ATSVR_DBGOUT("[AT<] ");
		ATSVR_DBGOUT((char *)b->p);

		res = atsvr_Reply(p, b);

		switch (res)
		{
		case 0:
			buf_PushData(b, 0, 1);
			atsvr_SendStr(p, (const char *)b->p);
			atsvr_SendStr(p, "ERROR\r\n");
			break;
		
#if ATSVR_CMUX_ENABLE
		case 2:
			p->cmux = 1;
#endif
		default:
			//Ĭ�ϻظ�OK
			atsvr_SendStr(p, "OK\r\n");
			break;
		}

		buf_Release(b);
	}
}

#if ATSVR_CMUX_ENABLE
static void atsvr_CmuxRun(atsvr_t *p, buf b)
{
	u8 *ph, *pu, pf, fcs;
	int len, hlen, ilen, dlci;
	
	//����
	if (uart_Recive(p->uart, b) == 0)
		return;

	//����CMUX��Լ
	for (; b->len > 5; buf_Remove(b, 1))
	{
		if (b->p[0] != CMUX_HEADER)
			continue;

		ph = &b->p[1];
		if (ph[0] == CMUX_HEADER)
			continue;

		ilen = (ph[2] >> 1);
		if (ph[2] & 1)
		{
			//���ֽڳ���
			hlen = 3;
		}
		else
		{
			//˫�ֽڳ���
			ilen += (ph[3] << 7);
			hlen = 4;
		}
		len = hlen + ilen;
		if (len > PPP_MRU)
			continue;

		//���Ȳ���
		if (b->len < (len + 2))
			break;

		//CSУ��
		fcs = ~fcs8(ph, hlen);
		if (ph[len] != fcs)
			continue;

		dlci = ph[0] >> 2;
		//��֧��4����ͨ����
		if (dlci > 3)
			continue;

		//������ȷ
#if 0 && ATSVR_DEBUG_ENABLE
		{
			int i;
			char str[8];
	
			ATSVR_DBGOUT("[CMUX<] ");
			for (i = 0; i < len; i++)
			{
				sprintf(str, "%02X ", ph[i]);
				ATSVR_DBGOUT(str);
			}
			ATSVR_DBGOUT(STRING_0D0A);
		}
#endif
		
		pf = ph[1] & 0x10;
		switch (ph[1] & 0xEF)
		{
		case 0x43:		//DISC
			//ͨ��0�յ�DISC�˳�CMUX״̬
			if (dlci == 0)
				p->cmux = 0;
			
		case 0x2F:		//SABM
			atsvr_CmuxSend(p->uart, ph[0], 0x63 | pf, NULL, 0);		//UA

			break;

		case 0xEF:		//UIH
			pu = &ph[hlen];
			switch (dlci)
			{
			case 1:
			case 2:
				p->dlci = dlci - 1;
				buf_Push(p->rbuf[p->dlci], pu, ilen);
#if ATSVR_PPP_ENABLE
				if (p->ppp == dlci)
				{
					ppp_process(p->rbuf[p->dlci]);
				}
				else
#endif
				{
					atsvr_RxAT(p, p->rbuf[p->dlci]);
				}
				break;

			default:
				switch (pu[0])
				{
				case 0x23:
					atsvr_CmuxSend(p->uart, ph[0], ph[1], pu, ilen);

					break;
				}

				break;
			}

			break;

		default:
			break;
		}
	}
}
#endif




void atsvr_Init()
{

	AT->uart = uart_Open(ATSVR_UART_ID, OS_TMO_FOREVER);
}

void atsvr_Config(int baud)
{

	uart_Config(AT->uart, baud, UART_PARI_NO, UART_DATA_8D, UART_STOP_1D);
}

int atsvr_GetState()
{

	return AT->ste;
}

void atsvr_Run()
{
	atsvr_t *p = AT;

	if (p->cnt++ >= (1000 / OS_TICK_MS))
	{
		p->cnt = 0;

#if ATSVR_PPP_ENABLE
		if (p->ppp)
		{
			//����256���ղ����κ�ppp֡�˳���ATģʽ
			if (p->tmo++ == 0xFF)
				p->ppp = 0;
		}
		else
#endif
		{
#if ATSVR_ATRW_ENABLE
			//���ݵ��������ϱ� $MYURCREAD
			if (p->tbuf->len)
				atsvr_SendStr(AT, "$MYURCREAD: 0\r\n");
#endif
		}
	}

	//����
#if ATSVR_CMUX_ENABLE
	if (p->cmux)
	{
		atsvr_CmuxRun(p, p->cbuf);
#if ATSVR_PPP_ENABLE
		if (p->ppp)
			ppp_send();
#endif
	}
	else
#endif
	{
		uart_Recive(p->uart, p->rbuf[0]);
#if ATSVR_PPP_ENABLE
		if (p->ppp)
		{
			ppp_process(p->rbuf[0]);
			ppp_send();
		}
		else
#endif
		{
			atsvr_RxAT(p, p->rbuf[0]);
		}
	}
}


int atsvr_TxPush(const void *data, size_t len)
{

	return buf_Push(AT->tbuf, data, len);
}






#endif







