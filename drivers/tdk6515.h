#ifndef __TDK6515_H__
#define __TDK6515_H__


#ifdef __cplusplus
extern "C" {
#endif




//Public Typedefs
typedef struct {
	int		ste;
	uart_t *uart;
} t_tdk6515;




// =============================================================
// �Ӽ���IC�ж�ȡ��ԭʼֵ,���ջ�ת������Ӧ�Ľ���
// =============================================================
typedef struct {
    s32    temp;       // 1. �¶�                  S32  0.1��
    u32    freq;       // 2. Ƶ��                  U32  0.01HZ
    float  enp[4];     // 3. ����ABC�й�����       Float
    float  enq[4];     // 7. ����ABC�޹�����       Float
    float  ahour[4];   // 11.ABCN�ల��ƽ��Сʱ    Float
    float  vhour[3];   // 15.ABC�����ƽ��Сʱ     Float
    u32    caangle;    // 18.CA���ѹ������      U32     0.1DEC
    u32    cbangle;    // 19.CB���ѹ������      U32     0.1DEC
    u32    reserve;    // 20
    u32    vover0num;  // 21.��ѹ�������          U32
    float  p[4];       // 22.��ABC���й�����       Float
    float  q[4];       // 26.��ABC���޹�����       Float
    float  ui[4];      // 30.��ABC���ڹ���         Float
    float  viangle[4]; // 34.��ABC��ѹ�������     Float
    float  cos[4];     // 38.��ABC��������         Float
    float  i[4];       // 42.ABCN������Чֵ        Float
    float  u[3];       // 46.ABC���ѹ��Чֵ       Float
    float  ptneed;     // 49.�й���������          Float
    float  qtneed;     // 50.�޹���������          Float
    float  ppneed;     // 51.�й���������          Float
    float  qpneed;     // 52.�޹���������          Float
    u32    workr;      // 53.��������״̬�Ĵ���    SW
    u32    powerdir;   // 54.���ʷ���Ĵ���        SW
    u32    netstatus;  // 55.��������״̬��        SW
    u32    connstatus; // 56.��������״̬��        SW
    u32    adjustsum;  // 57.У������У��ͼĴ��� U32
    u32    lasttx;     // 58.��һ��TX����ֵ�Ĵ���     
    u32    uab;        // 59.AB�ߵ�ѹ��Чֵ
    u32    ubc;        // 60.BC�ߵ�ѹ��Чֵ
    u32    uac;        // 61.AC�ߵ�ѹ��Чֵ
    u32    ppulse;     // 62.�й����������
    u32    qpulse;     // 63.�޹����������
    u32    pulse1;     // 64.Զ�����������1
    u32    pulse2;     // 65.Զ�����������2
    u32    pulse3;     // 66.Զ�����������3
    u32    pulse4;     // 67.Զ�����������4
    u32    pulse5;     // 68.Զ�����������5
    u32    ua1min;     // 69.A���ѹһ����ƽ��ֵ
    u32    ub1min;     // 70.B���ѹһ����ƽ��ֵ
    u32    uc1min;     // 71.C���ѹһ����ƽ��ֵ
} t_tdk6515_rtdata;


// =============================================================
// �Ӽ���IC�ж�ȡ��ԭʼֵ,����ת�����˸�������������
// =============================================================
typedef struct {
    u32		curxbno;        // 72.��ǰг������ͨ����
    float   xbrate[21];     // 73.��ǰͨ����2-21��г��������
    float   xbbase;         // 94. ��ǰг��ͨ��������Чֵ
    u32		xbbasefreq;     // 95. ��ǰг������Ƶ��
} t_tdk6515_xbdata;




//External Functions
void tdk6515_Reset(t_tdk6515 *p);
int tdk6515_IsJLReady(void);
int tdk6515_IsXBReady(void);
void tdk6515_CmdSend(t_tdk6515 *p, int nWrite, u16 nAdr, size_t nLen);


#ifdef __cplusplus
}
#endif

#endif

