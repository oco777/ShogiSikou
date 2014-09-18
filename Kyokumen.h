//
//  Kyokumen.h
//  MySampleProject
//
//  Created by 小森久 on 2014/06/11.
//
//

#ifndef __MySampleProject__Kyokumen__
#define __MySampleProject__Kyokumen__

#include "ShogiDefs.h"
#include "KomaInf.h"
#include "Te.h"

class Kyokumen {
private:
	KomaInf		m_BanPadding1[16];
	KomaInf		m_Ban[16*11];
	KomaInf		m_BanPadding2[16];
	Kiki		m_ControlS[16*11];
	Kiki		m_ControlE[16*11];
	UINT8		m_Hand[EHI+1];
	UINT16		m_TeNum;
	Te			m_LastTe;
	KomaPos		m_KingS;
	KomaPos		m_KingE;
	UINT8		m_Suji;
	UINT8		m_Dan;
	UINT8		m_Promote;
	INT32		m_Value;
	UINT64		m_KyokumenHashVal;
	UINT64		m_HandHashVal;
	UINT64		m_HashVal;
	
	static UINT64 m_HashSeed[ERY+1][SUJIDAN2KOMAPOS(9,9)+1];
	static UINT64 m_HandHashSeed[EHI+1][MAX_FU+1];
	static UINT64 m_HashHistory[MAX_TE_HISTORY+1];
	static Kiki m_OuteHistory[MAX_TE_HISTORY+1];
	
public:
	Kyokumen() {}
	Kyokumen(int suji, int dan, int promote, int teNum, KomaInf ban[MAX_DAN][MAX_SUJI], int hand[EHI+1]);
	int operator==(const Kyokumen &k) {
		int ret = true;
		for (int suji=1;suji <= m_Suji; suji++) {
			for (int dan=1; dan <= m_Dan; dan++) {
				KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
				if (m_Ban[pos] != k.GetBanInf(pos)) {
					ret = false;
					break;
				}
			}
		}
		return ret;
	}
	
	void Init(int suji, int dan, int promote, int teNum, KomaInf ban[MAX_DAN][MAX_SUJI], int hand[EHI+1]);
	
	KomaInf GetBanInf(KomaPos pos) const { return m_Ban[pos]; }
	void SetBanInf(KomaPos pos, KomaInf inf) { m_Ban[pos] = inf; }
	Kiki GetControlS(KomaPos pos) const { return m_ControlS[pos]; }
	const Kiki *GetControlSPtr() const { return m_ControlS; }
	void SetControlS(KomaPos pos, Kiki kiki) { m_ControlS[pos] = kiki; }
	Kiki GetControlE(KomaPos pos) const { return m_ControlE[pos]; }
	const Kiki *GetControlEPtr() const { return m_ControlE; }
	void SetControlE(KomaPos pos, Kiki kiki) { m_ControlE[pos] = kiki; }
	UINT8 GetHandNum(KomaInf inf) const { return m_Hand[inf]; }
	void SetHandNum(KomaInf inf, UINT8 num) { m_Hand[inf] = num; }
	UINT16 GetTeNum() const { return m_TeNum; }
	void SetTeNum(UINT16 num) { m_TeNum = num; }
	Te GetLastTe() const { return m_LastTe; }
	void SetLastTe(Te te) { m_LastTe = te; }
	KomaPos GetKingS() const { return m_KingS; }
	void SetKingS(KomaPos pos) { m_KingS = pos; }
	KomaPos GetKingE() const { return m_KingE; }
	void SetKingE(KomaPos pos) { m_KingE = pos; }
	UINT8 GetSuji() const { return m_Suji; }
	void SetSuji(UINT8 suji) { m_Suji = suji; }
	UINT8 GetDan() const { return m_Dan; }
	void SetDan(UINT8 dan) { m_Dan = dan; }
	UINT8 GetPromote() const { return m_Promote; }
	void SetPromote(UINT8 promote) { m_Promote = promote; }
	INT32 GetValue() const { return m_Value; }
	void SetValue(INT32 value) { m_Value = value; }
	UINT64 GetKyokumenHashVal() const { return m_KyokumenHashVal; }
	UINT64 GetHandHashVal() const { return m_HandHashVal; }
	UINT64 GetHashVal() const { return m_HashVal; }
	
	Teban GetTeban() const { if ((m_TeNum % 2) == 0) { return SELF; } return ENEMY; }
	
	void InitControl();
	int MakeLegalMoves(Teban teban, Te *teBuf, int *pin=nullptr);
	void MakePinInf(int *pin) const;
	int AntiCheck(Teban teban, Te *teBuf,int *pin, Kiki kiki);
	void AddMoves(Teban teban, int &teNum, Te *teTop, KomaPos from, int pin, int Rpin=0);
	int Utifudume(Teban teban, KomaPos to, int *pin);
	KomaPos search(KomaPos pos, int dir) const { do { pos += dir; } while (m_Ban[pos] == EMPTY); return pos; }
	void MoveKing(Teban teban, int &teNum, Te *teTop, Kiki kiki);
	void MoveTo(Teban teban, int &teNum, Te *teTop, KomaPos to, int* pin);
	void PutTo(Teban teban, int &teNum, Te *teTop, KomaPos to, int *pin);
	void AddMove(Teban teban, int &teNum, Te *teTop, KomaPos from, int diff, int pin, int Rpin=0);
	void AddStraight(Teban teban, int &teNum, Te *teTop, KomaPos from, int dir, int pin, int Rpin=0);
	Kiki CountMove(Teban teban, KomaPos pos, int *pin);
	Kiki CountControlS(KomaPos pos);
	Kiki CountControlE(KomaPos pos);
	void Move(Teban teban, const Te &te, bool bHashHistory);
	int BestEval(Teban teban);
	int Eval(KomaPos pos);
	int EvalMin(Te *AtackS, int NumAtackS, Te *AtackE, int NumAtackE);
	int EvalMax(Te *AtackS, int NumAtackS, Te *AtackE, int NumAtackE);
	bool IsCorrectMove(Te &te) const;
	bool IsLegalMove(Teban teban, Te &te) const;
	
	// 詰め将棋.
	int Mate(Teban teban, int maxDepth, Te &te);
	int CheckMate(Teban teban, int depth, int depthMax, Te *checks, Te &te);
	int MakeChecks(Teban teban, Te *teBuf, int *pin=nullptr);
	int AntiCheckMate(Teban teban, int depth, int depthMax, Te *antichecks);
	
	static void HashInit();
	static UINT64 GetHashSeed(KomaInf inf, KomaPos pos);
	static UINT64 GetHandHashSeed(KomaInf inf, int num);
	static UINT64 GetHashHistory(int teNum);
	static Kiki GetOuteHistory(int teNum);
};

template<class T> void swap(T &x, T &y)
{
	T temp = x;
	x = y;
	y = temp;
}

#if defined(WINDOWS)
#else // defined(WINDOWS).
template<class T> T min(const T &x, const T &y)
{
	return (x < y) ? x : y;
}

template<class T> T max(const T &x, const T &y)
{
	return (x > y) ? x : y;
}
#endif // defined(WINDOWS).

#endif /* defined(__MySampleProject__Kyokumen__) */
