//
//  TsumeHash.h
//  CustomizedShogi
//
//  Created by 小森久 on 2014/09/09.
//
//

#ifndef __CustomizedShogi__TsumeHash__
#define __CustomizedShogi__TsumeHash__

#include "TypeDefs.h"
#include "ShogiDefs.h"
#include "Te.h"

struct TsumeVal {
	UINT64 HashVal;
	UINT64 Motigoma;
	int NextEntry;
	int mate;
	Te te;
};

class TsumeHash {
	static TsumeVal m_HashTbl[TSUME_HASH_SIZE];
	static UINT64 FU_BIT_TBL[19];
	static UINT64 KY_BIT_TBL[5];
	static UINT64 KE_BIT_TBL[5];
	static UINT64 GI_BIT_TBL[5];
	static UINT64 KI_BIT_TBL[5];
	static UINT64 KA_BIT_TBL[3];
	static UINT64 HI_BIT_TBL[3];
	
	static TsumeVal *FindFirst(UINT64 KyokumenHashVal);
	static TsumeVal *FindNext(TsumeVal *pNow);
	static UINT64 CalcHand(UINT8 Motigoma[]);
	
public:
	static void Clear();
	static void Add(UINT64 KyokumenHashVal, UINT64 HandHashVal, UINT8 Motigoma[], int mate, Te te);
	static TsumeVal *Find(UINT64 KyokumenHashVal, UINT64 HandHashVal, UINT8 Motigoma[]);
	static TsumeVal *DomSearchCheckMate(UINT64 KyokumenHashVal, UINT8 Motigoma[]);
	static TsumeVal *DomSearchAntiMate(UINT64 KyokumenHashVal, UINT8 Motigoma[]);
};


#endif /* defined(__CustomizedShogi__TsumeHash__) */
