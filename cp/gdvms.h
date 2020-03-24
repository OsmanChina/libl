#ifndef __GDVMS_H__
#define __GDVMS_H__


//Public Defines
#define GDVMS_ADR_SIZE					6



//Public Typedefs
//��Լ�ṹ
struct gdvms
{
	struct dlrcp	parent;
	u8	ste;
	u8	adr[GDVMS_ADR_SIZE];
	u8	fseq;			//֡���
	u8	code : 6,		//������
		abn : 1,		//�쳣��־
		dir : 1;		//���ͷ���
	buf	data;			//��������
};
typedef struct gdvms gdvms_t;






//External Functions
void gdvms_Init(gdvms_t *p);
sys_res gdvms_TmsgSend(gdvms_t *p, int nCode, buf b, int nType);
sys_res gdvms_TmsgError(gdvms_t *p, int nCode, int nErr);
sys_res gdvms_Transmit(gdvms_t *p, gdvms_t *pD);
sys_res gdvms_Handler(gdvms_t *p);

void gdvms_Response(gdvms_t *p);



#endif

