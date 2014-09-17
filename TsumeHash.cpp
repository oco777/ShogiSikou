//
//  TsumeHash.cpp
//  CustomizedShogi
//
//  Created by 小森久 on 2014/09/09.
//
//

#include <string.h>
#include "TsumeHash.h"

TsumeVal TsumeHash::m_HashTbl[TSUME_HASH_SIZE];

UINT64 TsumeHash::FU_BIT_TBL[19]={
	0x0000000000000000,
	0x0000000000000001,
	0x0000000000000003,
	0x0000000000000007,
	0x000000000000000f,
	0x000000000000001f,
	0x000000000000003f,
	0x000000000000007f,
	0x00000000000001ff,
	0x00000000000003ff,
	0x00000000000007ff,
	0x0000000000000fff,
	0x0000000000001fff,
	0x0000000000003fff,
	0x0000000000007fff,
	0x000000000000ffff,
	0x000000000001ffff,
	0x000000000003ffff,
	0x000000000007ffff
};

UINT64 TsumeHash::KY_BIT_TBL[5]={
	0x0000000000000000,
	0x0000000000100000,
	0x0000000000300000,
	0x0000000000700000,
	0x0000000000f00000,
};

UINT64 TsumeHash::KE_BIT_TBL[5]={
	0x0000000000000000,
	0x0000000001000000,
	0x0000000003000000,
	0x0000000007000000,
	0x000000000f000000,
};

UINT64 TsumeHash::GI_BIT_TBL[5]={
	0x0000000000000000,
	0x0000000010000000,
	0x0000000030000000,
	0x0000000070000000,
	0x00000000f0000000,
};

UINT64 TsumeHash::KI_BIT_TBL[5]={
	0x0000000000000000,
	0x0000000100000000,
	0x0000000300000000,
	0x0000000700000000,
	0x0000000f00000000,
};

UINT64 TsumeHash::KA_BIT_TBL[3]={
	0x0000000000000000,
	0x0000000100000000,
	0x0000000300000000,
};

UINT64 TsumeHash::HI_BIT_TBL[3]={
	0x0000000000000000,
	0x0000000400000000,
	0x0000000c00000000,
};

UINT64 TsumeHash::CalcHand(UINT8 Motigoma[])
{
	return (
	FU_BIT_TBL[Motigoma[FU]] |
	KY_BIT_TBL[Motigoma[KY]] |
	KE_BIT_TBL[Motigoma[KE]] |
	GI_BIT_TBL[Motigoma[GI]] |
	KI_BIT_TBL[Motigoma[KI]] |
	KA_BIT_TBL[Motigoma[KA]] |
	HI_BIT_TBL[Motigoma[HI]]);
}

void TsumeHash::Add(UINT64 KyokumenHashVal, UINT64 HandHashVal, UINT8 Motigoma[], int mate, Te te)
{
	UINT64 NowHashVal = KyokumenHashVal;
	UINT64 Hand = CalcHand(Motigoma);
	int i;
	for (i=0; i < TSUME_HASH_RETRY_MAX; i++) {
		if (m_HashTbl[NowHashVal&TSUME_HASH_AND].HashVal == 0) {
			TsumeVal &t = m_HashTbl[NowHashVal&TSUME_HASH_AND];
			t.HashVal = KyokumenHashVal;
			t.Motigoma = Hand;
			t.mate = mate;
			t.NextEntry = 0;
			t.te = te;
			break;
		}
		else if (m_HashTbl[NowHashVal&TSUME_HASH_AND].HashVal == KyokumenHashVal) {
			if (m_HashTbl[NowHashVal&TSUME_HASH_AND].Motigoma == Hand) {
				// 持ち駒も含めて同一局面.
			}
			else {
				// 盤上が同一で、持ち駒が違うものが登録済み.
				while(m_HashTbl[NowHashVal&TSUME_HASH_AND].NextEntry != 0) {
					NowHashVal = m_HashTbl[NowHashVal&TSUME_HASH_AND].NextEntry;
					if (m_HashTbl[NowHashVal&TSUME_HASH_AND].Motigoma == Hand) {
						// 持ち駒も含めて同一局面.
						break;
					}
				}
			}
			if (m_HashTbl[NowHashVal&TSUME_HASH_AND].Motigoma == Hand) {
				TsumeVal &t = m_HashTbl[NowHashVal&TSUME_HASH_AND];
				t.HashVal = KyokumenHashVal;
				t.Motigoma = Hand;
				t.mate = mate;
				t.te = te;
				break;
			}
			TsumeVal &pre = m_HashTbl[NowHashVal&TSUME_HASH_AND];
			NowHashVal = KyokumenHashVal^HandHashVal;
			for (int j=0; j < TSUME_HASH_RETRY_MAX; j++) {
				if (m_HashTbl[NowHashVal&TSUME_HASH_AND].HashVal == 0) {
					TsumeVal &t = m_HashTbl[NowHashVal&TSUME_HASH_AND];
					t.HashVal = KyokumenHashVal;
					t.Motigoma = Hand;
					t.mate = mate;
					t.NextEntry = 0;
					t.te = te;
					pre.NextEntry = NowHashVal&TSUME_HASH_AND;
					break;
				}
				NowHashVal += TSUME_HASH_SIZE / 11;
			}
			break;
		}
		NowHashVal += TSUME_HASH_SIZE / 11;
	}
	if (i == TSUME_HASH_RETRY_MAX) {
//		printf("OVER MAX¥n");
	}
}

void TsumeHash::Clear()
{
	memset(m_HashTbl, 0, sizeof(m_HashTbl));
}

TsumeVal* TsumeHash::FindFirst(UINT64 KyokumenHashVal)
{
	UINT64 NowHashVal = KyokumenHashVal;
	for(int i=0; i < TSUME_HASH_RETRY_MAX; i++) {
		if (m_HashTbl[NowHashVal&TSUME_HASH_AND].HashVal == KyokumenHashVal) {
			return m_HashTbl+(NowHashVal&TSUME_HASH_AND);
		}
		NowHashVal += TSUME_HASH_SIZE / 11;
	}
	return nullptr;
}

TsumeVal* TsumeHash::FindNext(TsumeVal *Now)
{
	if (Now->NextEntry == 0) {
		return nullptr;
	}
	return m_HashTbl + Now->NextEntry;
}

TsumeVal *TsumeHash::Find(UINT64 KyokumenHashVal, UINT64 HandHashVal, UINT8 Motigoma[])
{
	UINT64 CalcMotigoma = CalcHand(Motigoma);
	TsumeVal *ret = FindFirst(KyokumenHashVal);
	while (ret != nullptr) {
		if (ret->Motigoma == CalcMotigoma) {
			return ret;
		}
		ret = FindNext(ret);
	}
	return nullptr;
}

// ハッシュに登録された局面で、持ち駒がより少ない局面で詰んでいる局面を探す.
TsumeVal *TsumeHash::DomSearchCheckMate(UINT64 KyokumenHashVal, UINT8 Motigoma[])
{
	UINT64 CalcMotigoma = CalcHand(Motigoma);
	TsumeVal *ret = FindFirst(KyokumenHashVal);
	while (ret != nullptr) {
		if (ret->Motigoma == CalcMotigoma) {
			if (ret->mate != 1) {
				return nullptr;
			}
		}
		// ハッシュに登録されているより持ち駒が多く、ハッシュで詰みなら詰み.
		if ((ret->Motigoma & CalcMotigoma) == ret->Motigoma && ret->mate == 1) {
			return ret;
		}
		ret = FindNext(ret);
	}
	return nullptr;
}


