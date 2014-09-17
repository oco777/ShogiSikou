//
//  KyokumenKomagumi.cpp
//  CustomizedShogi
//
//  Created by 小森久 on 2014/09/06.
//
//

#include <stdlib.h>
#include "KyokumenKomagumi.h"
#include "Sikou.h"

#if defined(DEBUG_PRINT)
extern bool g_bDebugPrint;
#endif // defined(DEBUG_PRINT).

INT32 KyokumenKomagumi::m_KomagumiValue[ERY+1][16*11];
INT32 KyokumenKomagumi::m_JosekiKomagumi[HUMEI+1][ERY+1][16*11];
INT32 KyokumenKomagumi::m_SemegomaValueS[16*11][16*11];
INT32 KyokumenKomagumi::m_SemegomaValueE[16*11][16*11];
INT32 KyokumenKomagumi::m_MamorigomaValueS[16*11][16*11];
INT32 KyokumenKomagumi::m_MamorigomaValueE[16*11][16*11];

int DanValue[ERY+1][MAX_DAN+1]={
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//空
	{0,0,0,0,0,0,0,0,0,0},
	//歩
	{ 0,  0,15,15,15,3,1, 0, 0, 0},
	//香
	{ 0, 1,2,3,4,5,6,7,8,9},
	//桂
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//銀
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//金
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//角
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//飛
	{ 0,10,10,10, 0, 0, 0,  -5, 0, 0},
	//王
	{ 0,1200,1200,900,600,300,-10,0,0,0},
	//と
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//杏
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//圭
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//全
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//金
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//馬
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//龍
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//空
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//歩
	{ 0, 0, 0, 0, -1, -3,-15,-15,-15, 0},
	//香
	{ 0,-9,-8,-7, -6, -5, -4, -3, -2,-1},
	//桂
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//銀
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//金
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//角
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//飛
	{ 0, 0, 0, 5, 0, 0, 0,-10,-10,-10},
	//王
	{ 0, 0, 0, 0,10,-300,-600,-900,-1200,-1200},
	//と
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//杏
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//圭
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//全
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//金
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//馬
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//龍
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int JosekiKomagumiSGI[KyokumenKomagumi::HUMEI+1][9][9]=
{
	{	// IvsFURI 舟囲い、美濃、銀冠
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10, -7,-10,-10,-10,-10,-10,  7,-10},
		{-10,  7, -8, -7, 10,-10, 10,  6,-10},
		{-10, -2, -6, -5,-10,  6,-10,-10,-10},
		{-10, -7,  0,-10,-10,-10,-10,-10,-10}
	},{	// IvsNAKA　舟囲い
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10, -7,-10,-10, -7,-10,-10,  7,-10},
		{-10, -5, -8, -7, 10,-10, 10,  6,-10},
		{-10, -2, -3,  0,-10,  6,-10,-10,-10},
		{-10, -7, -5,-10,-10,-10,-10,-10,-10}
	},{ // FURIvsFURI　矢倉（逆）、美濃、銀冠
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10, -7, -7,-10},
		{-10,-10,-10,-10,-10,  5, 10, 10,-10},
		{-10,-10,-10,-10,-10,-10,  0,-10,-10},
		{-10,-10,-10,-10,-10,-10, -5,-10,-10}
	},{ // FURIvsI 美濃囲い、銀冠
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10, -3, -7,-10,-10,-10,-10,-10},
		{-10, -7,  4,  6,-10,-10,-10,  6,-10},
		{-10,  2,  3,  3,-10,-10,  4,-10,-10},
		{-10,-10,-10,  0,-10,-10,  0,-10,-10}
	},{ // NAKAvsI 中飛車
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,  8,  5,  8,-10,-10,-10},
		{-10,-10,  4,  4,  3,  4,  4,-10,-10},
		{-10,-10,  0,-10,-10,-10,  0,-10,-10}
	},{ // KAKUGAWARI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,  7,  5, -3,-10,-10},
		{-10,  8, 10,  7,  4,  0, -4,-10,-10},
		{-10,  0,-8,  -4,-10,-10, -5,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10}
	},{ // AIGAKARI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,  0,-10,-10,-10,-10,-10,-10},
		{-10, -5,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10}
	},{ // HUMEI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,  5,-10,-10},
		{-10,-10,-10,-10,-10,-10, -4,  0,-10},
		{-10,-10,  0,-10,-10,-10, -4, -3,-10},
		{-10, -5,-10, -5,-10,-10, -5,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10}
	}
};

int JosekiKomagumiSKI[KyokumenKomagumi::HUMEI+1][9][9]=
{
	{	// IvsFURI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,  1,  2,-10,-10,-10,-10},
		{-10,-10,-10,  0,-10, -4,-10,-10,-10}
	},{	// IvsNAKA
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,  1,  2,-10,-10,-10,-10},
		{-10,-10,-10,  0,-10, -4,-10,-10,-10}
	},{ // FURIvsFURI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,  7, -3,-10,-10},
		{-10,-10,-10,-10,  5,  3,  6,-10,-10},
		{-10,-10,-10,-10,-10,  5,  4,-10,-10}
	},{ // FURIvsI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,  5,  1,-10,-10},
		{-10,-10,-10,-10,  4,  3,  7, -3,-10},
		{-10,-10,-10,  0,  1,  5,  2, -7,-10}
	},{ // NAKAvsI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10, -7, -4, -4,-10, -4, -4, -7,-10},
		{-10, -5, 10,  6,-10,  8, 10, -5,-10},
		{-10, -7, -6, -3, -6, -3, -6, -7,-10}
	},{ // KAKUGAWARI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,  6, -4, -4, -4, -8,-10},
		{-10,-10, 10,-10,  3,  0,  0, -7,-10},
		{-10,-10,-10,  0,-10,  0, -5, -7,-10}
	},{ // AIGAKARI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,  6,-10,-10,-10,-10,-10},
		{-10,-10, 10,-10,  3,-10,-10,-10,-10},
		{-10,-10,-10,  0,-10,  0,-10,-10,-10}
	},{ // HUMEI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,  3,-10,  5,-10,-10,-10,-10},
		{-10,-10,-10,  0,-10,  0,-10,-10,-10}
	}
};

int JosekiKomagumiSOU[KyokumenKomagumi::HUMEI+1][9][9]=
{
	{	// IvsFURI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{- 7,  9,-10,-10,-10,-10,-10,-10,-10},
		{  5,  7,  8,  4,-10,-10,-10,-10,-10},
		{ 10,  5,  3,-10,-10,-10,-10,-10,-10}
	},{	// IvsNAKA
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{- 7,  9,-10,-10,-10,-10,-10,-10,-10},
		{  5,  7,  8,  4,-10,-10,-10,-10,-10},
		{ 10,  5,  3,-10,-10,-10,-10,-10,-10}
	},{ // FURIvsFURI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,  4,  6, 10,  6},
		{-10,-10,-10,-10,-10,  4,  6,  5, 10}
	},{ // FURIvsI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,  4,  6, 10,  6},
		{-10,-10,-10,-10,-10,  4,  6,  5, 10}
	},{ // NAKAvsI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,  4,  6, 10,  6},
		{-10,-10,-10,-10,-10,  4,  6,  5, 10}
	},{ // KAKUGAWARI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{- 3, -4, -3,-10,-10,-10,-10,-10,-10},
		{  6,  8, -2,  0, -3,-10,-10,-10,-10},
		{ 10,  6, -4,- 6,- 7,-10,-10,-10,-10}
	},{ // AIGAKARI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{- 3, -4, -3,-10,-10,-10,-10,-10,-10},
		{  6,  8,  0,- 4,-10,-10,-10,-10,-10},
		{ 10,  6, -4,- 6,- 7,-10,-10,-10,-10}
	},{ // HUMEI
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{-10,-10,-10,-10,-10,-10,-10,-10,-10},
		{- 3, -4, -3,-10,-10,-10,-10,-10,-10},
		{  6,  8,  0,- 4,-10,-10,-10,-10,-10},
		{ 10,  6, -4,- 6,- 7,-10,-10,-10,-10}
	}
};

void KyokumenKomagumi::Initialize(bool bUseJoseki)
{
	m_bUseJoseki = bUseJoseki;
	if (m_bUseJoseki) {
		Sikou::InitJoseki();
	}
	// 本将棋のみ.
	int suji;
	int dan;
	KomaInf inf;
	for (suji=1; suji <= MAX_SUJI; suji++) {
		for (dan=1; dan <= MAX_DAN; dan++) {
			for (inf=SFU; inf <= ERY; inf++) {
				m_KomagumiValue[inf][SUJIDAN2KOMAPOS(suji, dan)] = 0;
				m_JosekiKomagumi[0][inf][SUJIDAN2KOMAPOS(suji, dan)] = DanValue[inf][dan];
			}
		}
	}
	InitKanagomaValue();
	InitShuubando();
	InitBonus();
}

void KyokumenKomagumi::SenkeiInit()
{
	m_KomagumiBonus[0] = 0;
	m_KomagumiBonus[1] = 0;
	
	if (! IsUseJoseki()) {
		return;
	}
	
	KomaPos SHI1, SHI2;
	KomaPos EHI1, EHI2;
	KomaPos SKA1, SKA2;
	KomaPos EKA1, EKA2;
	KomaPos suji, dan;
	KomaInf koma;
	SHI1 = SHI2 = EHI1 = EHI2 = SKA1 = SKA2 = EKA1 = EKA2 = 0;
	for (suji=1; suji <= MAX_SUJI; suji++) {
		for (dan=1; dan <= MAX_DAN; dan++) {
			KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
			KomaInf inf = GetBanInf(pos);
			if (inf == SHI) {
				if (SHI1 == 0) {
					SHI1 = pos;
				}
				else {
					SHI2= pos;
				}
			}
			if (inf == EHI) {
				if (EHI1 == 0) {
					EHI1 = pos;
				}
				else {
					EHI2 = pos;
				}
			}
			if (inf == SKA) {
				if (SKA1 == 0) {
					SKA1 = pos;
				}
				else {
					SKA2 = pos;
				}
			}
			if (inf == EKA) {
				if (EKA1==0) {
					EKA1 = pos;
				}
				else {
					EKA2 = pos;
				}
			}
		}
	}
	if (GetHandNum(SHI) == 1) {
		if (SHI1 == 0) {
			SHI1 = 1;
		}
		else {
			SHI2 = 1;
		}
	}
	if (GetHandNum(SHI) == 2) {
		SHI1 = SHI2 = 1;
	}
	if (GetHandNum(EHI) == 1) {
		if (EHI1 == 0) {
			EHI1 = 1;
		}
		else {
			EHI2 = 1;
		}
	}
	if (GetHandNum(EHI) == 2) {
		EHI1 = EHI2 = 1;
	}
	if (GetHandNum(SKA) == 1) {
		if (SKA1 == 0) {
			SKA1 = 1;
		}
		else {
			SKA2 = 1;
		}
	}
	if (GetHandNum(SKA) == 2) {
		SKA1 = SKA2 = 1;
	}
	if (GetHandNum(EKA) == 1) {
		if (EKA1 == 0) {
			EKA1 = 1;
		}
		else {
			EKA2 = 1;
		}
	}
	if (GetHandNum(EKA) == 2) {
		EKA1 = EKA2 = 1;
	}
	
	int Senkei = HUMEI;
	int GyakuSenkei = HUMEI;
	if (IS_BANPOS(SHI1) && IS_BANPOS(EHI1)) {
		if (KOMAPOS2SUJI(SHI1) < 5 && KOMAPOS2SUJI(EHI1) < 5) {
			Senkei = IvsFURI;
			GyakuSenkei = FURIvsI;
		}
		else if (KOMAPOS2SUJI(SHI1) < 5 && KOMAPOS2SUJI(EHI1) == 5) {
			Senkei = IvsNAKA;
			GyakuSenkei = NAKAvsI;
		}
		else if (KOMAPOS2SUJI(SHI1) >= 5 && KOMAPOS2SUJI(EHI1) <= 5) {
			Senkei = FURIvsFURI;
			GyakuSenkei = FURIvsFURI;
		}
		else if (KOMAPOS2SUJI(SHI1) > 5 && KOMAPOS2SUJI(EHI1) > 5) {
			Senkei = FURIvsI;
			GyakuSenkei = IvsFURI;
		}
		else if (KOMAPOS2SUJI(SHI1) == 5 && KOMAPOS2SUJI(EHI1) > 5) {
			Senkei = NAKAvsI;
			GyakuSenkei = IvsNAKA;
		}
		else if (KOMAPOS2SUJI(SHI1) == 2 && KOMAPOS2SUJI(EHI1) == 8) {
			Senkei = AIGAKARI;
			GyakuSenkei = AIGAKARI;
		}
		else if (SKA1 == 1 && EKA1 == 1) {
			Senkei = KAKUGAWARI;
			GyakuSenkei = KAKUGAWARI;
		}
	}
	
	for (suji=1; suji <= MAX_SUJI; suji++) {
		for (dan=1; dan <= MAX_DAN; dan++) {
			KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
			KomaInf inf = GetBanInf(pos);
//			SetValue(GetValue() - m_KomagumiValue[inf][pos]);
			for (koma=SFU; koma <= ERY; koma++) {
				if (koma == SGI) {
					m_JosekiKomagumi[Senkei][koma][pos] = JosekiKomagumiSGI[Senkei][dan-1][9-suji];
				}
				else if (koma == EGI) {
					m_JosekiKomagumi[Senkei][koma][pos] = -JosekiKomagumiSGI[GyakuSenkei][9-dan][suji-1];
				}
				else if (koma == SKI) {
					m_JosekiKomagumi[Senkei][koma][pos] = JosekiKomagumiSKI[Senkei][dan-1][9-suji];
				}
				else if (koma == EKI) {
					m_JosekiKomagumi[Senkei][koma][pos] = -JosekiKomagumiSKI[GyakuSenkei][9-dan][suji-1];
				}
				else if (koma == SOU) {
					m_JosekiKomagumi[Senkei][koma][pos] = JosekiKomagumiSOU[Senkei][dan-1][9-suji];
				}
				else if (koma == EOU) {
					m_JosekiKomagumi[Senkei][koma][pos] = -JosekiKomagumiSOU[GyakuSenkei][9-dan][suji-1];
				}
				else {
					m_JosekiKomagumi[Senkei][koma][pos] = DanValue[koma][dan];
				}
				m_KomagumiValue[koma][pos] = m_JosekiKomagumi[Senkei][koma][pos];
			}
//			SetValue(GetValue() + m_KomagumiValue[inf][pos]);
			if (IS_SELFKOMA(inf)) {
				m_KomagumiBonus[0] += m_KomagumiValue[inf][pos];
			}
			else if (IS_ENEMYKOMA(inf)) {
				m_KomagumiBonus[1] += m_KomagumiValue[inf][pos];
			}
		}
	}
}

//空歩香桂銀金角飛王と杏圭全金馬龍
int ShuubandoByAtack[]={0,1,1,2,3,3,3,4,4,3,3,3,3,3,4,5};
//空歩香桂銀 金角飛王 と 杏 圭 全 金 馬 龍
int ShuubandoByDefence[]={0,0,0,0,-1,-1,0,-1,0,-1,-1,-1,-1,-1,-2,-1};
//空歩香桂銀金角飛王と杏圭全金馬龍
int ShuubandoByHand[]={0,0,1,1,2,2,2,3,0,0,0,0,0,0,0,0};

int IsKanagoma[]={
	//  空空空空空空空空空空空空空空空空空歩香桂銀金角飛王と杏圭全金馬龍
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,0,0,
	//	空歩香桂銀金角飛王と杏圭全金馬龍壁空空空空空空空空空空空空空空空
	0,0,0,0,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void KyokumenKomagumi::Move(Teban teban, const Te &te, bool bHashHistory)
{
	int self, enemy;
	if (teban == SELF) {
		self = 0;
		enemy = 1;
	}
	else {
		self = 1;
		enemy = 0;
	}
	if (te.GetKoma() == SOU || te.GetKoma() == EOU) {
		// 駒を動かした後で、全面的にBonusの計算しなおしが必要。
		// Semegoma,Mamorigomaなど全て変わるので。
	}
	else {
		if (IsKanagoma[te.GetKoma()] && te.GetFrom() > 0) {
			if (teban == SELF) {
				m_SemegomaBonus[0] -= m_SemegomaValueS[te.GetFrom()][GetKingE()];
				m_MamorigomaBonus[0] -= m_MamorigomaValueS[te.GetFrom()][GetKingS()];
			}
			else {
				m_SemegomaBonus[1] -= m_SemegomaValueE[te.GetFrom()][GetKingS()];
				m_MamorigomaBonus[1] -= m_MamorigomaValueE[te.GetFrom()][GetKingE()];
			}
		}
		if (te.GetCapture() != 0) {
			if (IsKanagoma[te.GetCapture()]) {
				if (teban == SELF) {
					m_SemegomaBonus[1] -= m_SemegomaValueE[te.GetTo()][GetKingS()];
					m_MamorigomaBonus[1] -= m_MamorigomaValueE[te.GetTo()][GetKingE()];
				}
				else {
					m_SemegomaBonus[0] -= m_SemegomaValueS[te.GetTo()][GetKingE()];
					m_MamorigomaBonus[0] -= m_MamorigomaValueS[te.GetTo()][GetKingS()];
				}
			}
		}
		if (te.GetPromote() == 0) {
			if (IsKanagoma[te.GetKoma()]) {
				if (teban == SELF) {
					m_SemegomaBonus[0] += m_SemegomaValueS[te.GetTo()][GetKingE()];
					m_MamorigomaBonus[0] += m_MamorigomaValueS[te.GetTo()][GetKingS()];
				}
				else {
					m_SemegomaBonus[1] += m_SemegomaValueE[te.GetTo()][GetKingS()];
					m_MamorigomaBonus[1] += m_MamorigomaValueE[te.GetTo()][GetKingE()];
				}
			}
		}
		else {
			if (IsKanagoma[te.GetKoma()|PROMOTED]) {
				if (teban == SELF) {
					m_SemegomaBonus[0] += m_SemegomaValueS[te.GetTo()][GetKingE()];
					m_MamorigomaBonus[0] += m_MamorigomaValueS[te.GetTo()][GetKingS()];
				}
				else {
					m_SemegomaBonus[1] += m_SemegomaValueE[te.GetTo()][GetKingS()];
					m_MamorigomaBonus[1] += m_MamorigomaValueE[te.GetTo()][GetKingE()];
				}
			}
		}
	}
	m_KomagumiBonus[self] -= m_KomagumiValue[te.GetKoma()][te.GetFrom()];
	int danMax = GetDan();
	int promote = GetPromote();
//	if (te.GetFrom() > 0 && KOMAPOS2DAN(te.GetFrom()) <= 4) {
	if (te.GetFrom() > 0 && KOMAPOS2DAN(te.GetFrom()) <= promote+1) {
		// ４段目以下・終盤度の計算.
		if (teban == SELF) {
			m_Shuubando[1] -= ShuubandoByAtack[te.GetKoma() & ~SELF];
		}
		else {
			m_Shuubando[1] -= ShuubandoByDefence[te.GetKoma() & ~ENEMY];
		}
	}
//	if (te.GetFrom() > 0 && KOMAPOS2DAN(te.GetFrom()) >= 6) {
	if (te.GetFrom() > 0 && KOMAPOS2DAN(te.GetFrom()) >= danMax-promote) {
		// ６段目以上・終盤度の計算.
		if (teban == SELF) {
			m_Shuubando[0] -= ShuubandoByDefence[te.GetKoma() & ~SELF];
		}
		else {
			m_Shuubando[0] -= ShuubandoByAtack[te.GetKoma() & ~ENEMY];
		}
	}
	if (te.GetFrom() == 0) {
		// 打つことによる終盤度の減少.
		if (teban == SELF) {
			m_Shuubando[1] -= ShuubandoByHand[te.GetKoma() & ~SELF];
		}
		else {
			m_Shuubando[0] -= ShuubandoByHand[te.GetKoma() & ~ENEMY];
		}
	}
	if (te.GetCapture() != 0) {
//		if (KOMAPOS2DAN(te.GetTo()) <= 4) {
		if (KOMAPOS2DAN(te.GetTo()) <= promote+1) {
			// ４段目以下・終盤度の計算.
			if (teban == SELF) {
				m_Shuubando[1] -= ShuubandoByDefence[te.GetCapture() & ~ENEMY];
			}
			else {
				m_Shuubando[1] -= ShuubandoByAtack[te.GetCapture() & ~SELF];
			}
		}
//		if (KOMAPOS2DAN(te.GetTo()) >= 6) {
		if (KOMAPOS2DAN(te.GetTo()) >= danMax-promote) {
			// ６段目以上・終盤度の計算.
			if (teban == SELF) {
				m_Shuubando[0] -= ShuubandoByAtack[te.GetCapture() & ~ENEMY];
			}
			else {
				m_Shuubando[0] -= ShuubandoByDefence[te.GetCapture() & ~SELF];
			}
		}
		// Handに入ったことによる終盤度の計算.
		if (teban == SELF) {
			m_Shuubando[1] += ShuubandoByHand[te.GetCapture() & ~ENEMY & ~PROMOTED];
		}
		else {
			m_Shuubando[0] += ShuubandoByHand[te.GetCapture() & ~SELF & ~PROMOTED];
		}
		m_KomagumiBonus[enemy] -= m_KomagumiValue[te.GetCapture()][te.GetTo()];
	}
	if (te.GetPromote() == 0) {
//		if (KOMAPOS2DAN(te.GetTo()) <= 4) {
		if (KOMAPOS2DAN(te.GetTo()) <= promote+1) {
			// ４段目以下・終盤度の計算.
			if (teban == SELF) {
				m_Shuubando[1] += ShuubandoByAtack[te.GetKoma() & ~SELF];
			}
			else {
				m_Shuubando[1] += ShuubandoByDefence[te.GetKoma() & ~ENEMY];
			}
		}
//		if (KOMAPOS2DAN(te.GetTo()) >= 6) {
		if (KOMAPOS2DAN(te.GetTo()) >= danMax-promote) {
			// ６段目以上・終盤度の計算.
			if (teban == SELF) {
				m_Shuubando[0] += ShuubandoByDefence[te.GetKoma() & ~SELF];
			}
			else {
				m_Shuubando[0] += ShuubandoByAtack[te.GetKoma() & ~ENEMY];
			}
		}
		m_KomagumiBonus[self] += m_KomagumiValue[te.GetKoma()][te.GetTo()];
	}
	else {
//		if (KOMAPOS2DAN(te.GetTo()) <= 4) {
		if (KOMAPOS2DAN(te.GetTo()) <= promote+1) {
			// ４段目以下・終盤度の計算.
			if (teban == SELF) {
				m_Shuubando[1] += ShuubandoByAtack[(te.GetKoma()|PROMOTED) & ~SELF];
			}
			else {
				m_Shuubando[1] += ShuubandoByDefence[(te.GetKoma()|PROMOTED) & ~ENEMY];
			}
		}
//		if (KOMAPOS2DAN(te.GetTo()) >= 6) {
		if (KOMAPOS2DAN(te.GetTo()) >= danMax-promote) {
			// ６段目以上・終盤度の計算.
			if (teban == SELF) {
				m_Shuubando[0] += ShuubandoByDefence[(te.GetKoma()|PROMOTED) & ~SELF];
			}
			else {
				m_Shuubando[0] += ShuubandoByAtack[(te.GetKoma()|PROMOTED) & ~ENEMY];
			}
		}
		m_KomagumiBonus[self] += m_KomagumiValue[te.GetKoma()|PROMOTED][te.GetTo()];
	}
	Kyokumen::Move(teban, te, bHashHistory);
	if (te.GetKoma() == SOU || te.GetKoma() == EOU) {
		// 全面的に金駒のBonusの計算しなおし。
		InitBonus();
	}
}

void KyokumenKomagumi::InitShuubando()
{
	// 終盤度を求めると同時に、終盤度によるボーナスの付加、駒の加点も行う。
	int sujiMax = GetSuji();
	int danMax = GetDan();
	int promote = GetPromote();
	int suji, dan;
	m_Shuubando[0] = 0;
	m_Shuubando[1] = 0;
	for (suji=1; suji <= sujiMax; suji++) {
//		for (dan=1; dan <= 4; dan++) {
		for (dan=1; dan <= promote+1; dan++) {
			KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
			KomaInf inf = GetBanInf(pos);
			if (IS_SELFKOMA(inf)) {
				m_Shuubando[1] += ShuubandoByAtack[inf & ~SELF];
			}
			if (IS_ENEMYKOMA(inf)) {
				m_Shuubando[1] += ShuubandoByDefence[inf & ~ENEMY];
			}
		}
//		for (dan=6; dan <= 9; dan++) {
		for (dan=danMax-promote; dan <= danMax; dan++) {
			KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
			KomaInf inf = GetBanInf(pos);
			if (IS_ENEMYKOMA(inf)) {
				m_Shuubando[0] += ShuubandoByAtack[inf & ~ENEMY];
			}
			if (IS_SELFKOMA(inf)) {
				m_Shuubando[0] += ShuubandoByDefence[inf & ~SELF];
			}
		}
	}
	KomaInf koma;
	for (koma=FU; koma <= HI; koma++) {
		m_Shuubando[0] += ShuubandoByHand[koma] * GetHandNum(koma | ENEMY);
		m_Shuubando[1] += ShuubandoByHand[koma] * GetHandNum(koma | SELF);
	}
}

void KyokumenKomagumi::InitBonus()
{
	int sujiMax = GetSuji();
	int danMax = GetDan();
	int suji, dan;
	m_SemegomaBonus[0] = 0;
	m_SemegomaBonus[1] = 0;
	m_MamorigomaBonus[0] = 0;
	m_MamorigomaBonus[1] = 0;
	
	for (suji=1; suji <= sujiMax; suji++) {
		for (dan=1; dan <= danMax; dan++) {
			KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
			KomaInf inf = GetBanInf(pos);
			if (IsKanagoma[inf]) {
				if (IS_SELFKOMA(inf)) {
					m_SemegomaBonus[0] += m_SemegomaValueS[pos][GetKingE()];
					m_MamorigomaBonus[0] += m_MamorigomaValueS[pos][GetKingS()];
				} else {
					m_SemegomaBonus[1] += m_SemegomaValueE[pos][GetKingS()];
					m_MamorigomaBonus[1] += m_MamorigomaValueE[pos][GetKingE()];
				}
			}
		}
	}
}

INT32 KyokumenKomagumi::Evaluate()
{
	// 終盤度を０〜１６の範囲に補正する。
	int Shuubando0, Shuubando1;
	
	if (m_Shuubando[0] < 0) {
		Shuubando0 = 0;
	}
	else if (m_Shuubando[0] > 16) {
		Shuubando0 = 16;
	}
	else {
		Shuubando0 = m_Shuubando[0];
	}
	if (m_Shuubando[1] < 0) {
		Shuubando1 = 0;
	}
	else if (m_Shuubando[1] > 16) {
		Shuubando1 = 16;
	}
	else {
		Shuubando1 = m_Shuubando[1];
	}
	
	// 終盤度の差を評価する。とりあえず、１違うと２００点違うことにする。
	int ret = (Shuubando1 - Shuubando0) * 200;
#if defined(DEBUG_PRINT)
if (g_bDebugPrint) {
	printf("Evaluate[%d] Shuubando[%d] [%d]\n", ret, Shuubando0, Shuubando1);
}
#endif // defined(DEBUG_PRINT).
	
	ret += m_SemegomaBonus[0] * Shuubando1 / 16;
	ret += m_MamorigomaBonus[0] * Shuubando0 /16;
#if defined(DEBUG_PRINT)
if (g_bDebugPrint) {
	printf("Evaluate[%d] 0 [%d] [%d]\n", ret, m_SemegomaBonus[0], m_MamorigomaBonus[0]);
}
#endif // defined(DEBUG_PRINT).
	ret += m_SemegomaBonus[1] * Shuubando0 /16;
	ret += m_MamorigomaBonus[1] * Shuubando1 /16;
#if defined(DEBUG_PRINT)
if (g_bDebugPrint) {
	printf("Evaluate[%d] 1 [%d] [%d]\n", ret, m_SemegomaBonus[1], m_MamorigomaBonus[1]);
}
#endif // defined(DEBUG_PRINT).
	
	ret += m_KomagumiBonus[0] + m_KomagumiBonus[1];
#if defined(DEBUG_PRINT)
if (g_bDebugPrint) {
	printf("Evaluate[%d] m_KomagumiBonus[%d] [%d]\n", ret, m_KomagumiBonus[0], m_KomagumiBonus[1]);
}
#endif // defined(DEBUG_PRINT).
	
	//	printf("ret=%d¥n",ret);
//	if (abs(ret)>10000) {
//		Print();
//		printf("ret=%d¥n",ret);
//	}
	
	// 最後に、駒得の点数と合わせて評価値とする。
//	return ret + GetValue();
	return (ret/10) + GetValue();
	//	return value;
}

// 手番から見て敵の駒.
static bool IsEnemy(Teban teban, KomaInf koma)
{
	return (koma != WALL && (koma & teban) == 0);
}

// 手番から見て味方の駒.
static bool IsSelf(Teban teban, KomaInf koma)
{
	return (koma != WALL && (koma & teban) != 0);
}

// 手の価値の比較（qsort用）大きい順に並ぶようにする。
static int teValueComp(const void *p1, const void *p2)
{
	Te *te1 = (Te *)p1;
	Te *te2 = (Te *)p2;
	return te2->GetValue() - te1->GetValue();
}

static const int Tbl[256]={
	0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

// xは、何ビット１が立っているか
static int bitnum(int x)
{
	return Tbl[((unsigned char *)&x)[0]] + Tbl[((unsigned char *)&x)[1]] + Tbl[((unsigned char *)&x)[2]] + Tbl[((unsigned char *)&x)[3]];
}

void KyokumenKomagumi::EvaluateTe(Teban teban, int teNum, Te *te)
{
	int i;
	int dir;
	int nowEval = Evaluate();
	//	Print();
	for (i=0; i < teNum; i++) {
		int LossS, LossE, GainS, GainE;
		LossS = LossE = GainS = GainE = 0;
		KyokumenKomagumi _new(*this);
		// 手番から見て敵の玉.
		KomaInf EnemyKing = (teban == SELF ? EOU : SOU);
		// 手番から見て敵の利き.
		const Kiki *_newControlE = (teban == SELF ? _new.GetControlEPtr() : _new.GetControlSPtr());
		// 手番から見て味方の利き.
		const Kiki *_newControlS = (teban == SELF ? _new.GetControlSPtr() : _new.GetControlEPtr());
		// 手番から見て味方の利き.
		const Kiki *_nowControlS = (teban == SELF ? GetControlSPtr() : GetControlEPtr());
		
		KomaInf NewKoma = (te[i].GetPromote() != 0 ? te[i].GetKoma()|PROMOTED : te[i].GetKoma());
		// 実際に一手動かして、評価値の変動をみる。
		_new.Move(teban, te[i], true);
		te[i].SetValue(_new.Evaluate() - nowEval);
		if (teban == ENEMY) {
			// 敵の番の時には手の価値をひっくり返して置く.
			te[i].SetValue(-te[i].GetValue());
		}
		if (te[i].GetFrom() != 0) {
			// 駒が居た場所の脅威がなくなる.
			LossS -= Eval(te[i].GetFrom());
		}
		// 新しく移動した先での脅威が加わる.
		LossS += _new.Eval(te[i].GetTo());
		// 相手に与える脅威と、新しく自分の駒にヒモをつけることで、減る脅威を計算.
		for (dir=DIRECT_START; dir < DIRECT_NUM; dir++) {
			if (CanMove[dir][NewKoma]) {
				int p = te[i].GetTo() + Direct[dir];
				if (_new.GetBanInf(p) != EnemyKing) {
					// 玉以外の駒に対する脅威.
					if (IsEnemy(teban, _new.GetBanInf(p))) {
						LossE += _new.Eval(p);
					}
					else if (IsSelf(teban, _new.GetBanInf(p))) {
						GainS += Eval(p) - _new.Eval(p);
					}
				}
				else {
					// 玉に与える脅威はそのまま計算すると大きすぎになるので調整する。
					if (_newControlE[te[i].GetTo()]) {
						if (_newControlS[te[i].GetTo()]) {
							LossE += 1000;
						}
						else {
							LossE += 500;
						}
					}
					else {
						LossE += 1500;
					}
				}
			}
		}
		// 移動したことで、別の駒の飛び利きを通す（かも知れない）.
		if (IS_BANPOS(te[i].GetFrom())) {
			for (dir=DIRECT_START; dir < DIRECT_NUM_FOR_PIN; dir++) {
				if ((_nowControlS[te[i].GetFrom()] & (1<<(dir+KIKI_JUMP_OFFSET))) !=0) {
					int p = _new.search(te[i].GetFrom(), Direct[dir]);
					if (_new.GetBanInf(p) != WALL) {
						// 飛び利きの通った先での交換値の再計算.
						if (IsEnemy(teban, _new.GetBanInf(p))) {
							LossE += _new.Eval(p);
						}
						else if (IsSelf(teban, _new.GetBanInf(p))) {
							GainS += Eval(p) - _new.Eval(p);
						}
					}
				}
			}
		}
		// 移動した駒自身の飛び利きによる脅威の増減を計算する.
		for (dir=DIRECT_START; dir < DIRECT_NUM_FOR_PIN; dir++) {
			if (CanJump[dir][NewKoma]) {
				int p = _new.search(te[i].GetTo(), Direct[dir]);
				// 例によって、玉に対する脅威は大きく評価されすぎるので調整する。
				if (_new.GetBanInf(p) != EnemyKing) {
					if (IsEnemy(teban, _new.GetBanInf(p))) {
						LossE += _new.Eval(p);
					}
					else if (IsSelf(teban, _new.GetBanInf(p))) {
						GainS += Eval(p) - _new.Eval(p);
					}
				}
				else {
					if (_newControlE[te[i].GetTo()]) {
						if (_newControlS[te[i].GetTo()]) {
							LossE += 1000;
						}
						else {
							LossE += 500;
						}
					}
					else {
						LossE += 1500;
					}
				}
			}
		}
		te[i].SetValue(te[i].GetValue() + GainS - LossS);
		if ((te[i].GetCapture() != EMPTY && te[i].GetCapture() != FU)) {
			// 歩以外の駒を取る手は無条件に1500点プラスして、読みに入れるようにする.
			te[i].SetValue(te[i].GetValue() + 1500);
		}
		if (te[i].GetFrom() == FU && bitnum(_newControlE[0xac-te[i].GetTo()]) > 1) {
			// 焦点の歩…かも。
			te[i].SetValue(te[i].GetValue() + 50);
		}
		// 攻撃の価値はこれくらいの値（１０分の１）を加えると、実験ではちょうど良い.
		te[i].SetValue(te[i].GetValue() + LossE / 10);
		
		//		te[i].Print();
		//		printf("val:%5d LossS:%5d LossE:%5d GainS:%5d GainE:%5d¥n",te[i].value,LossS,LossE,GainS,GainE);
	}
	//	char p[20];
	//	gets(p);
	// 並び替え。
	qsort(te, teNum, sizeof(te[0]), teValueComp);
}

static int Mamorigoma[17][9]={
	{ 50, 50, 50, 50, 50, 50, 50, 50, 50},
	{ 56, 52, 50, 50, 50, 50, 50, 50, 50},
	{ 64, 61, 55, 50, 50, 50, 50, 50, 50},
	{ 79, 77, 70, 65, 54, 51, 50, 50, 50},
	{100, 99, 95, 87, 74, 58, 50, 50, 50},
	{116,117,101, 95, 88, 67, 54, 50, 50},
	{131,129,124,114, 90, 71, 59, 51, 50},
	{137,138,132,116, 96, 76, 61, 53, 50},
	{142,142,136,118, 98, 79, 64, 52, 50},
	{132,132,129,109, 95, 75, 60, 51, 50},
	{121,120,105, 97, 84, 66, 54, 50, 50},
	{ 95, 93, 89, 75, 68, 58, 51, 50, 50},
	{ 79, 76, 69, 60, 53, 50, 50, 50, 50},
	{ 64, 61, 55, 51, 50, 50, 50, 50, 50},
	{ 56, 52, 50, 50, 50, 50, 50, 50, 50},
	{ 50, 50, 50, 50, 50, 50, 50, 50, 50},
	{ 50, 50, 50, 50, 50, 50, 50, 50, 50},
};

static int Semegoma[17][9]={
	{ 50, 50, 50, 50, 50, 50, 50, 50, 50},
	{ 50, 50, 50, 50, 50, 50, 50, 50, 50},
	{ 50, 50, 50, 50, 50, 50, 50, 50, 50},
	{ 54, 53, 51, 51, 50, 50, 50, 50, 50},
	{ 70, 66, 62, 55, 53, 50, 50, 50, 50},
	{ 90, 85, 80, 68, 68, 60, 53, 50, 50},
	{100, 97, 95, 85, 84, 71, 51, 50, 50},
	{132,132,129,102, 95, 71, 51, 50, 50},
	{180,145,137,115, 91, 75, 57, 50, 50},
	{170,165,150,121, 94, 78, 58, 52, 50},
	{170,160,142,114, 98, 80, 62, 55, 50},
	{140,130,110,100, 95, 75, 54, 50, 50},
	{100, 99, 95, 87, 78, 69, 50, 50, 50},
	{ 80, 78, 72, 67, 55, 51, 50, 50, 50},
	{ 62, 60, 58, 52, 50, 50, 50, 50, 50},
	{ 50, 50, 50, 50, 50, 50, 50, 50, 50},
	{ 50, 50, 50, 50, 50, 50, 50, 50, 50},
};

void KyokumenKomagumi::InitKanagomaValue()
{
	for (int kingSdan=1; kingSdan <= MAX_DAN; kingSdan++) {
		for (int kingSsuji=1; kingSsuji <= MAX_SUJI; kingSsuji++) {
			for (int kingEdan=1; kingEdan <= MAX_DAN; kingEdan++) {
				for (int kingEsuji=1; kingEsuji <= MAX_SUJI; kingEsuji++) {
					for (int suji=1; suji <= MAX_SUJI; suji++) {
						for (int dan=1; dan <= MAX_DAN; dan++) {
							int DiffSujiS = abs(kingSsuji - suji);
							int DiffSujiE = abs(kingEsuji - suji);
							int DiffDanSS = 8 + (dan - kingSdan);
							int DiffDanES = 8 + (dan - kingEdan);
							int DiffDanSE = 8 + (-(dan - kingSdan));
							int DiffDanEE = 8 + (-(dan - kingEdan));
							int kingS = kingSsuji + kingSdan;
							int kingE = kingEsuji + kingEdan;
							
							KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
							m_SemegomaValueS[pos][kingE] = Semegoma[DiffDanES][DiffSujiE] - 100;
							m_MamorigomaValueS[pos][kingS] = Mamorigoma[DiffDanSS][DiffSujiS] - 100;
							m_SemegomaValueE[pos][kingS] = -(Semegoma[DiffDanSE][DiffSujiS] - 100);
							m_MamorigomaValueE[pos][kingE] = -(Mamorigoma[DiffDanEE][DiffSujiE] - 100);
						}
					}
				}
			}
		}
	}
}



