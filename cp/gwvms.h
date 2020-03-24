#ifndef __GWVMS_H__
#define __GWVMS_H__


//Public Defines
#define GWVMS_ADR_SIZE					17

//���ͷ�����
#define GWVMS_CDIR_RECV					0x00	//��վ����
#define GWVMS_CDIR_SEND					0x01	//�ն˷���


//�������
#define GWVMS_ERR_NO_ERROR				0x00	//��ȷ,�޴���
#define GWVMS_ERR_RELAY_FALE			0x01	//�м�����û�з���
#define GWVMS_ERR_INVALID				0x02	//�������ݷǷ�
#define GWVMS_ERR_NO_PERMIT				0x03	//����Ȩ�޲���
#define GWVMS_ERR_NO_DATA				0x04	//�޴�������
#define GWVMS_ERR_TIME_INVALID			0x05	//����ʱ��ʧЧ
#define GWVMS_ERR_NO_ADDR				0x11	//Ŀ���ַ������
#define GWVMS_ERR_SEND_FAIL				0x12	//����ʧ��
#define GWVMS_ERR_SMS_LONG				0x13	//����Ϣ̫֡��

#define GWVMS_DATA_INVALID				0xFFFFFFFF



//Public Typedefs
//��Լ�ṹ
typedef struct
{
	struct dlrcp	parent;
	u8	ste;
	u8	adr[GWVMS_ADR_SIZE];
	u8	fseq;			//֡���
	u8	ftype;			//֡����
	u8	ptype;			//��������
	buf	data;			//��������
} t_gwvms, *p_gwvms;


//External Functions
void gwvms_Init(p_gwvms p);
sys_res gwvms_TmsgSend(p_gwvms p, int nFType, int nPType, buf b, int nType);
sys_res gwvms_Handler(p_gwvms p);

void gwvms_Response(p_gwvms p);



#endif

