#ifndef __NW12_H__
#define __NW12_H__


#ifdef __cplusplus
extern "C" {
#endif





//Public Defines



//AFN
#define NW12_AFN_CONFIRM			0x00
#define NW12_AFN_LINKCHECK			0x02

//FUN
#define NW12_FUN_RESET				1
#define NW12_FUN_DATA_USER			4
#define NW12_FUN_LINKCHECK			9
#define NW12_FUN_DATA_L1			10
#define NW12_FUN_DATA_L2			11

#define NW12_FUN_CONFIRM			0
#define NW12_FUN_RESPONSE			8
#define NW12_FUN_NODATA				9
#define NW12_FUN_LINKSTATE			11

//��Ч����ֵ
#define NW12_DATA_INVALID			0xFFFFFFFF

//�������
#define NW12_ERR_NO_ERROR			0x00	//��ȷ,�޴���
#define NW12_ERR_RELAY_FALE			0x01	//�м�����û�з���
#define NW12_ERR_INVALID			0x02	//�������ݷǷ�
#define NW12_ERR_NO_PERMIT			0x03	//����Ȩ�޲���
#define NW12_ERR_NO_DATA			0x04	//�޴�������
#define NW12_ERR_TIME_INVALID		0x05	//����ʱ��ʧЧ
#define NW12_ERR_NO_ADDR			0x06	//Ŀ���ַ������
#define NW12_ERR_CRC_FAIL			0x07	//У��ʧ��





//Public Typedefs
//֡������
struct nw12_c
{
	u8	fun : 4,
		fcv : 1,
		fcb_acd : 1,
		prm : 1,
		dir : 1;
} PACK_STRUCT_STRUCT;

//֡������
struct nw12_seq
{
	u8	seq : 4,
		con : 1,
		fin : 1,
		fir : 1,
		tpv : 1;
} PACK_STRUCT_STRUCT;

//ʱ���ǩ��
struct nw12_tp
{
	u8	tp[4];
	u8	dly;
} PACK_STRUCT_STRUCT;

//NW12��Լ�ṹ
struct nw12
{
	struct dlrcp	parent;
	u8	msa;
 	u8	adr[6];		//terid�ڸ�3�ֽ�
	u8	radr[6];
	struct nw12_c	c;
	u8	afn;
	struct nw12_seq	seq;
	struct nw12_tp	tp;
	u8	pw[16];	
	buf	data;
};
typedef struct nw12 nw12_t;





//External Functions
void nw12_Init(nw12_t *p);
sys_res nw12_Handler(nw12_t *p);
void nw12_Response(nw12_t *p);

sys_res nw12_TmsgSend(nw12_t *p, int nFun, int nAfn, buf b, int nType);

u16 nw12_ConvertDa2DA(int nDa);
int nw12_ConvertDA2Map(u16 nDA, void *pData);
sys_res nw12_Transmit(nw12_t *p, nw12_t *pD);
int nw12_RecvCheck(nw12_t *p);


#ifdef __cplusplus
}
#endif


#endif


