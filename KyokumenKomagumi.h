//
//  KyokumenKomagumi.h
//  CustomizedShogi
//
//  Created by 小森久 on 2014/09/06.
//
//

#ifndef __CustomizedShogi__KyokumenKomagumi__
#define __CustomizedShogi__KyokumenKomagumi__

#include "Kyokumen.h"

class KyokumenKomagumi : public Kyokumen {
public:
	enum {
		IvsFURI,		// 居飛車対振り飛車.
		IvsNAKA,		// 居飛車対中飛車.
		FURIvsFURI,		// 相振り飛車.
		FURIvsI,		// 振り飛車対居飛車.
		NAKAvsI,		// 中飛車対居飛車.
		KAKUGAWARI,		// 角換り.
		AIGAKARI,		// 相掛かり（または居飛車の対抗系）.
		HUMEI			// 戦形不明.
	};
	
private:
	bool m_bUseJoseki;
	UINT8 m_Shuubando[2];
	INT32 m_KomagumiBonus[2];
	INT32 m_SemegomaBonus[2];
	INT32 m_MamorigomaBonus[2];
	
	static INT32 m_KomagumiValue[ERY+1][16*11];
	static INT32 m_JosekiKomagumi[HUMEI+1][ERY+1][16*11];
	static INT32 m_SemegomaValueS[16*11][16*11];
	static INT32 m_SemegomaValueE[16*11][16*11];
	static INT32 m_MamorigomaValueS[16*11][16*11];
	static INT32 m_MamorigomaValueE[16*11][16*11];
	
public:
	KyokumenKomagumi() { }
	KyokumenKomagumi(int suji, int dan, int promote, int teNum, KomaInf ban[MAX_DAN][MAX_SUJI], int hand[EHI+1]) : Kyokumen(suji, dan, promote, teNum, ban, hand) {
		m_bUseJoseki = false;
		InitShuubando();
		InitBonus();
	}
	void Initialize(bool bUseJoseki);
	void SenkeiInit();
	void Move(Teban teban, const Te &te, bool bHashHistory);
	void InitShuubando();
	void InitBonus();
	INT32 Evaluate();
	void EvaluateTe(Teban teban, int teNum, Te *te);
//	void narabe(int teNum, const char *t[]);
	
	bool IsUseJoseki() const { return m_bUseJoseki; }
	void SetUseJoseki(bool bSet) { if (bSet) { m_bUseJoseki = true; } else { m_bUseJoseki = false; } }

	static void InitKanagomaValue();
};

#endif /* defined(__CustomizedShogi__KyokumenKomagumi__) */
