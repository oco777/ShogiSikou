//
//  Sikou.h
//  MySampleProject
//
//  Created by 小森久 on 2014/06/11.
//
//

#ifndef __MySampleProject__Sikou__
#define __MySampleProject__Sikou__

#include <chrono>
#include "ShogiDefs.h"
#include "Te.h"
#include "KyokumenKomagumi.h"
#include "Joseki.h"

class HashEntry {
private:
	UINT64 m_HashVal;		// ハッシュ値.
	Te m_Best;				// 前回の反復深化での最善手.
	Te m_Second;			// 前々回以前の反復深化での最善手.
	INT32 m_Value;			// αβ探索で得た局面の評価値.
	UINT8 m_Flag;			// αβ探索で得た値が、局面の評価値そのものか、上限値か下限値か.
	INT16 m_TeNum;			// αβ探索を行った際の手数.
	INT8 m_Depth;			// αβ探索を行った際の深さ.
	INT8 m_RemainDepth;		// αβ探索を行った際の残り深さ.
	
public:
	enum {
		EXACTLY_VALUE,		// 値は局面の評価値そのもの.
		LOWER_BOUND,		// 値は局面の評価値の下限値(val>=β).
		UPPER_BOUND			// 値は局面の評価値の上限値(val<=α).
	};

public:
	HashEntry() {}
	UINT64 GetHashVal() const { return m_HashVal; }
	void SetHashVal(UINT64 val) { m_HashVal = val; }
	Te GetBest() const { return m_Best; }
	void SetBest(Te te) { m_Best = te; }
	Te GetSecond() const { return m_Second; }
	void SetSecond(Te te) { m_Second = te; }
	INT32 GetValue() const { return m_Value; }
	void SetValue(INT32 val) { m_Value = val; }
	UINT8 GetFlag() const { return m_Flag; }
	void SetFlag(UINT8 flag) { m_Flag = flag; }
	INT16 GetTeNum() const { return m_TeNum; }
	void SetTeNum(INT16 num) { m_TeNum = num; }
	INT8 GetDepth() const { return m_Depth; }
	void SetDepth(INT8 depth) { m_Depth = depth; }
	INT8 GetRemainDepth() const { return m_RemainDepth; }
	void SetRemainDepth(INT8 depth) { m_RemainDepth = depth; }
	
	Teban GetTeban() const { return ((m_TeNum % 2) == 0 ? SELF : ENEMY); }
};

class Sikou {
private:
	std::chrono::system_clock::time_point m_ThinkStart;
	
	static HashEntry m_HashTbl[HASHTBL_SIZE];
	static Joseki m_Joseki;
	static Te m_Stack[MAX_DEPTH];
	
public:
	Te Think(Teban teban, KyokumenKomagumi k, const Kyokumen &kForJoseki);
	Te Best[MAX_DEPTH][MAX_DEPTH];
	int MinMax(Teban teban, KyokumenKomagumi k, int depth, int depthMax);
	int AlphaBeta(Teban teban, KyokumenKomagumi k, int alpha, int beta, int depth, int depthMax);
	int NegaAlphaBeta(Teban teban, KyokumenKomagumi &k, int alpha, int beta, int depth, int depthMax, bool bITDeep=true);
	int ITDeep(Teban teban, KyokumenKomagumi &k, int alpha, int beta, int depth, int depthMax);
	
	HashEntry *GetHashTbl(UINT64 hashVal) const { return &(m_HashTbl[hashVal & HASHTBL_MASK]); }
	int MakeMoveFirst(Teban teban, int depth, Te teBuf[], KyokumenKomagumi k);
	
	static void InitJoseki();
};


#endif /* defined(__MySampleProject__Sikou__) */
