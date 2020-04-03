#ifndef __RDRN_H__
#define __RDRN_H__

#ifdef __cplusplus
extern "C" {
#endif


/* Modem State */
enum
{
	RDRN_S_RESET = 0,
	RDRN_S_WAIT,
	RDRN_S_INIT,
	RDRN_S_ONLINE,
};

//Public Defines
#define RD_MSG_LEN				79	//628bits
#define RD_SIM_MAX				8


//Public Typedefs
struct rdrn
{
	u8			ste;
	u8			tmo;
	u8			signal; 			/* ��Ӧ������:��ǰѡ�õ��ź���ǿ�Ĳ���: ��Ӧ������Ϊ0�����ʾGPIO_LED_RD_NET���� */
	u8			tcnt;
	u8			tlen;
	u8			trep;
	u8			fixed;
	char		bsi[25];
	u8			tmsg[RD_MSG_LEN];
	u8			rmsg[RD_MSG_LEN];
	u32 		simid;			/* ������: �����Ų�Ϊ0�����ʾ��IC������ */
	u32 		destid; 			//ָ�ӻ�����
	int			latitude;
	int			longitud;
	time_t		time;
	uart_t *	uart;
	buf 		rbuf;
	os_sem_t	tsem;
};
typedef struct rdrn rdrn_t;











//External Functions
void rdss_PowerEn(void);
u32 rdss_GetSIM(void);
int rdss_GetSignal(void);
char *rdss_GetBSI(void);
u32 rdss_GetDest(void);
void rdss_SetDest(u32 id);
int rdss_TxNow(const void *data, size_t len, int rep);

void rnss_GetLocation(int *latitude, int *longitud);
time_t rnss_GetTimet(void);


//User Functions
int rdss_ReadMsg(void *p, u8 *rep);
int rdss_MsgQty(void);
void rdss_RxDo(const u8 *p, size_t len);
int rdss_PwrADC(void);

















#ifdef __cplusplus
}
#endif

#endif

