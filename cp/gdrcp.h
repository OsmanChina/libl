#ifndef __GDRCP_H__
#define __GDRCP_H__


//���ͷ�����
#define GDRCP_CDIR_RECV					0x00	//��վ����
#define GDRCP_CDIR_SEND					0x01	//�ն˷���


//�������
#define GDRCP_ERR_NO_ERROR				0x00	//��ȷ,�޴���
#define GDRCP_ERR_RELAY_FALE			0x01	//�м�����û�з���
#define GDRCP_ERR_INVALID				0x02	//�������ݷǷ�
#define GDRCP_ERR_NO_PERMIT				0x03	//����Ȩ�޲���
#define GDRCP_ERR_NO_DATA				0x04	//�޴�������
#define GDRCP_ERR_TIME_INVALID			0x05	//����ʱ��ʧЧ
#define GDRCP_ERR_NO_ADDR				0x11	//Ŀ���ַ������
#define GDRCP_ERR_SEND_FAIL				0x12	//����ʧ��
#define GDRCP_ERR_SMS_LONG				0x13	//����Ϣ̫֡��

#define GDRCP_DATA_INVALID				0xFFFFFFFF

#endif

