//File: fs/sfs/sfs.c 
//Simple File System
//
//
//History:
//V1.0	First Release
//		likazhou 2009-02-06
//V1.1	Length 8 supported
//		likazhou 2012-03-29

//Include Header Files



//Private Variables
#if SFS_LOCK_ENABLE
static os_sem_t sfs_sem;
#endif


//Private Macros
#if SFS_LOCK_ENABLE
#define sfs_Lock()				os_sem_wait(&sfs_sem)
#define sfs_Unlock()			os_sem_signal(&sfs_sem)
#else
#define sfs_Lock()
#define sfs_Unlock()
#endif


#define SFS_BLK_IDLE			0xFFFFFFFF
#define SFS_BLK_ACTIVE			0xFFFFFF00
#define SFS_BLK_FULL			0xFFFF0000


#define SFS_S_IDLE				0xFFFF
#define SFS_S_VALID				0xFF00
#define SFS_S_INVALID			0x0000


//Private Typedefs
typedef struct {
	u32 ste;
}sfs_ste_t;

typedef struct {
	u16			ste;
	u16			len;
	sfs_id_t	id;
}sfs_idx_t;










//Internal Functions
static adr_t _sfs_Find(sfs_t *p, sfs_id_t id, sfs_idx_t *pidx)
{
	adr_t nBlk, nBEnd, nIdx, nEnd, nAdr = 0;
	sfs_ste_t blk;
	size_t nSize = flash_BlkSize(p->dev);

	nBlk = p->start;
	nBEnd = nBlk + nSize * p->blk;
	for (; nBlk < nBEnd; nBlk += nSize)
	{
		memcpy(&blk, (const u8 *)nBlk, sizeof(blk));
		if (blk.ste == SFS_BLK_IDLE)
			continue;
		
		nIdx = nBlk + sizeof(blk);
		nEnd = nBlk + nSize;
		for (; nIdx < nEnd; nIdx = ALIGN4(nIdx + sizeof(sfs_idx_t) + pidx->len))
		{
			memcpy(pidx, (const u8 *)nIdx, sizeof(sfs_idx_t));
			if (pidx->ste != SFS_S_VALID)
				continue;
			
			if (pidx->id == id)
			{
				nAdr = nIdx;
				break;
			}
		}
		if (nAdr)
			break;
	}
	
	return nAdr;
}

static int _sfs_Free(const adr_t blk, size_t size, adr_t *padr)
{
	adr_t end;
	sfs_idx_t idx;

	*padr = blk + sizeof(sfs_ste_t);
	end = blk + size;
	
	for (; *padr < end; *padr = ALIGN4(*padr + sizeof(sfs_idx_t) + idx.len))
	{
		memcpy(&idx, (const u8 *)*padr, sizeof(sfs_idx_t));
		
		if (idx.ste == SFS_S_IDLE)
			return (end - (*padr + sizeof(sfs_idx_t)));
	}
	
	return 0;
}

static sys_res _sfs_Write(sfs_t *p, sfs_id_t id, const void *data, size_t len)
{
	sys_res res;
	adr_t nAdrOld = 0, nIdx, nEnd, nIdxNext, nBlk, nBEnd, nAct = 0;
	sfs_ste_t xBlk;
	sfs_idx_t xIdx;
	int i, nQty, nIsFull = 1;
	size_t nSize;

	nSize = flash_BlkSize(p->dev);
	
	//�ָ�����ԭ����Ч����ʱ�����ж�
	nBlk = p->start;
	nBEnd = nBlk + nSize * (p->blk - 1);
	memcpy(&xBlk, (const u8 *)nBlk, sizeof(sfs_ste_t));
	if (xBlk.ste == SFS_BLK_ACTIVE)
	{
		memcpy(&xBlk, (const u8 *)nBEnd, sizeof(sfs_ste_t));
		if (xBlk.ste == SFS_BLK_ACTIVE)
			nAct = nBEnd;
	}
	
	//����ԭ��¼�ͼ���Ŀ�
	for (; nBlk <= nBEnd; nBlk += nSize)
	{
		memcpy(&xBlk, (const u8 *)nBlk, sizeof(sfs_ste_t));
		if ((nAct == 0) && (xBlk.ste == SFS_BLK_ACTIVE))
			nAct = nBlk;
		
		if ((nAdrOld == 0) && (xBlk.ste != SFS_BLK_IDLE))
		{
			nIdx = nBlk + sizeof(sfs_ste_t);
			nEnd = nBlk + nSize;
			for (; nIdx < nEnd; nIdx = ALIGN4(nIdx + sizeof(sfs_idx_t) + xIdx.len))
			{
				memcpy(&xIdx, (const u8 *)nIdx, sizeof(sfs_idx_t));
				if ((xIdx.id == id) && (xIdx.ste == SFS_S_VALID))
				{
					//�ҵ�ԭ��¼
					nAdrOld = nIdx;
					break;
				}
				//�ѵ�ĩβ
				if (xIdx.ste == SFS_S_IDLE)
					break;
			}
		}

		if (nAdrOld && nAct)
			break;
	}
	
	if (nAct == 0)
	{
		//δ�ҵ�����Ŀ�
		nAct = p->start;
 		if ((res = flash_NolockErase(p->dev, nAct))!= SYS_R_OK)
			return res;
		
		//�õ�һ��BlockΪ����״̬
		xBlk.ste = SFS_BLK_ACTIVE;
 		if ((res = flash_NolockProgram(p->dev, nAct, (u8 *)&xBlk, sizeof(sfs_ste_t))) != SYS_R_OK)
			return res;
	}
	
	//���ҿռ�,�ռ���,ֱ��д������
	if (_sfs_Free(nAct, nSize, &nIdx) >= (int)(len + sizeof(sfs_idx_t)))
		nIsFull = 0;
	
	//��ǰ��ID
	i = (nAct - p->start) / nSize;
	for (nQty = p->blk - 1; nIsFull && nQty; nQty--)
	{
		//�ÿ�ռ䲻��,��Ҫ�����¿�
		nAct = p->start + nSize * i;
		i = cycle(i, 0, p->blk - 1, 1);
		nBEnd = p->start + nSize * i;
		nBlk = p->start + nSize * cycle(i, 0, p->blk - 1, 1);
		memcpy(&xBlk, (const u8 *)nBEnd, sizeof(sfs_ste_t));
		
		if (xBlk.ste != SFS_BLK_IDLE)
		{
			//����NextBlk
 			if ((res = flash_NolockErase(p->dev, nBEnd)) != SYS_R_OK)
				return res;
		}
		
		//��NextBlkΪ��Act
		xBlk.ste = SFS_BLK_ACTIVE;
 		if ((res = flash_NolockProgram(p->dev, nBEnd, (const u8 *)&xBlk, sizeof(sfs_ste_t))) != SYS_R_OK)
			return res;
		
		//����ԭ�е���Ч��¼
		memcpy(&xBlk, (const u8 *)nBlk, sizeof(sfs_ste_t));
		if (xBlk.ste != SFS_BLK_IDLE)
		{
			nIdx = nBlk + sizeof(sfs_ste_t);
			nEnd = nBlk + nSize;
			nIdxNext = nBEnd + sizeof(sfs_ste_t);
			for (; nIdx < nEnd; nIdx = ALIGN4(nIdx + sizeof(sfs_idx_t) + xIdx.len))
			{
				memcpy(&xIdx, (const u8 *)nIdx, sizeof(sfs_idx_t));
				if (xIdx.ste == SFS_S_VALID)
				{
					//�ҵ���Ч��¼
					if (nAdrOld != nIdx)
					{
 						if ((res = flash_NolockProgram(p->dev, nIdxNext, (const u8 *)nIdx, sizeof(sfs_idx_t) + xIdx.len)) != SYS_R_OK)
							return res;
						
						nIdxNext += ALIGN4(sizeof(sfs_idx_t) + xIdx.len);
					}
					else
					{
						nAdrOld = 0;
					}
				}
			}
		}
		
		//��ԭActBlkΪFull
		xBlk.ste = SFS_BLK_FULL;
 		if ((res = flash_NolockProgram(p->dev, nAct, (const u8 *)&xBlk, sizeof(sfs_ste_t))) != SYS_R_OK)
			return res;
		
		//����OldBlk
 		if ((res = flash_NolockErase(p->dev, nBlk)) != SYS_R_OK)
			return res;
		
		//�ռ���,д������
		if (_sfs_Free(nBEnd, nSize, &nIdx) >= (int)(len + sizeof(sfs_idx_t)))
			nIsFull = 0;
	}
	
	if (nQty == 0)
		return SYS_R_FULL;
	
	//ɾ��ԭ��¼
	if (nAdrOld)
	{
		memcpy(&xIdx, (const u8 *)nAdrOld, sizeof(sfs_idx_t));
		xIdx.ste = SFS_S_INVALID;
		
 		if ((res = flash_NolockProgram(p->dev, nAdrOld, (const u8 *)&xIdx, sizeof(sfs_idx_t))) != SYS_R_OK)
			return res;
	}
	
	//д�¼�¼
	xIdx.ste = SFS_S_VALID;
	xIdx.id = id;
	xIdx.len = len;
	
	if ((res = flash_NolockProgram(p->dev, nIdx, (const u8 *)&xIdx, sizeof(sfs_idx_t))) != SYS_R_OK)
		return res;
	
	if ((res = flash_NolockProgram(p->dev, nIdx + sizeof(sfs_idx_t), (const u8 *)data, len)) != SYS_R_OK)
		return res;
	
	return SYS_R_OK;
}


void sfs_SystemInit()
{

#if SFS_LOCK_ENABLE
	os_sem_init(&sfs_sem, 1);
#endif
}

//-------------------------------------------------------------------------
//sfs_Init - ��ʼ��һ��SFS�豸
//
//@sfsDev:�������豸���
//
//Note:
//
//Return: SYS_R_OK on success, errno otherwise
//-------------------------------------------------------------------------
sys_res sfs_Init(sfs_t *p)
{
	sys_res res;
	adr_t adr, end;
	sfs_ste_t blk;
	size_t size = flash_BlkSize(p->dev);

	sfs_Lock();
	
	//����Flash,���
	blk.ste = SFS_BLK_IDLE;
	adr = p->start;
	end = adr + size * p->blk;
	for (; adr < end; adr += size)
	{
		res = flash_NolockErase(p->dev, adr);
		if (res != SYS_R_OK)
		{
			sfs_Unlock();
			return res;
		}
	}
	
	//�õ�һ��BlockΪ����״̬
	blk.ste = SFS_BLK_ACTIVE;
	res = flash_NolockProgram(p->dev, p->start, (u8 *)&blk, sizeof(blk));
	
	sfs_Unlock();
	
	return res;
}

//-------------------------------------------------------------------------
//sfs_Write - д��һ����¼
//
//@sfsDev: �������豸���
//@nParam: ��¼��ʶ��
//@buf: ����ָ��
//@nDataLen: ���ݳ���
//
//Note: ���ݳ��ȷ�Χ0 ~ 655535
//
//Return: SYS_R_OK on success, errno otherwise
//-------------------------------------------------------------------------
sys_res sfs_Write(sfs_t *p, sfs_id_t id, const void *data, size_t len)
{
	sys_res res;

	sfs_Lock();
	res = _sfs_Write(p, id, data, len);
	sfs_Unlock();
	
	return res;
}

//-------------------------------------------------------------------------
//_sfs_Read - ����һ����¼
//
//@sfsDev: �������豸���
//@nParam: ��¼��ʶ��
//@pData: ����ָ��
//
//Note:
//
//Return: �ɹ���ȡ�����ݳ���, errno otherwise
//-------------------------------------------------------------------------
int sfs_Read(sfs_t *p, sfs_id_t id, void *data, size_t len)
{
	int read = -1;
	adr_t adr;
	sfs_idx_t idx;

	sfs_Lock();
	
	if ((adr = _sfs_Find(p, id, &idx)) != 0)
	{
		//�ҵ���¼,��ȡ
		read = MIN(len, idx.len);
		memcpy(data, (const u8 *)(adr + sizeof(sfs_idx_t)), read);
	}
	
	sfs_Unlock();
	
	return read;
}


int sfs_Find(sfs_t *p, sfs_id_t from, sfs_id_t to, void *data, size_t len)
{
	adr_t nIdx, nEnd, nBlk, nBEnd;
	sfs_ste_t blk;
	sfs_idx_t xIdx;
	int valid, qty;
	size_t size = flash_BlkSize(p->dev);
	u8 *ptmp = (u8 *)data;

	if (to < from)
		return 0;
	
	qty = to - from + 1;
	
	sfs_Lock();
	
	nBlk = p->start;
	nBEnd = nBlk + size * p->blk;
	for (valid = 0; (valid < qty) && (nBlk < nBEnd); nBlk += size)
	{
		memcpy(&blk, (const u8 *)nBlk, sizeof(blk));
		if (blk.ste == SFS_BLK_IDLE)
			continue;
		
		nIdx = nBlk + sizeof(blk);
		nEnd = nBlk + size;
		for (; (valid < qty) && (nIdx < nEnd); nIdx = ALIGN4(nIdx + sizeof(sfs_idx_t) + xIdx.len))
		{
			memcpy(&xIdx, (const u8 *)nIdx, sizeof(sfs_idx_t));
			if (xIdx.ste != SFS_S_VALID)
				continue;
			
			if ((xIdx.id >= from) && (xIdx.id <= to))
			{
				memcpy(ptmp + (xIdx.id - from) * len, (const u8 *)(nIdx + sizeof(sfs_idx_t)), MIN(len, xIdx.len));
				valid += 1;
			}
		}
	}
	
	sfs_Unlock();
	
	return valid;
}


//-------------------------------------------------------------------------
//sfs_Delete - ɾ��һ����¼
//
//@sfsDev:�������豸���
//@nParam: ��¼��ʶ��
//
//Note:
//
//Return: SYS_R_OK on success, errno otherwise
//-------------------------------------------------------------------------
sys_res sfs_Delete(sfs_t *p, sfs_id_t id)
{
	sys_res res = SYS_R_ERR;
	adr_t adr;
	sfs_idx_t idx;

	sfs_Lock();
	
	if ((adr = _sfs_Find(p, id, &idx)) != 0)
	{
		//�ҵ���¼,���Ϊ��Ч
		idx.ste = SFS_S_INVALID;
		res = flash_NolockProgram(p->dev, adr, (const u8 *)&idx, sizeof(sfs_idx_t));
	}
	
	sfs_Unlock();
	
	return res;
}

