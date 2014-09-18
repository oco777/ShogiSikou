//
//  Sikou.cpp
//  MySampleProject
//
//  Created by 小森久 on 2014/06/11.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Sikou.h"
#include "TsumeHash.h"

#if defined(WINDOWS)
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <fstream.h>
extern int volatile *ChudanFlag;
#endif // defined(WINDOWS).

HashEntry Sikou::m_HashTbl[HASHTBL_SIZE];
Joseki Sikou::m_Joseki;
Te Sikou::m_Stack[MAX_DEPTH];

#if defined(DEBUG_PRINT)
bool g_bDebugPrint = true;
#endif // defined(DEBUG_PRINT).

static int s_decideDepthMax(KyokumenKomagumi &k)
{
	int sujiMax = k.GetSuji();
	int danMax = k.GetDan();
	int komaNum = 0;
	int suji;
	int dan;
	for (suji=1; suji <= sujiMax; suji++) {
		for (dan=1; dan <= danMax; dan++) {
			if (k.GetBanInf(SUJIDAN2KOMAPOS(suji, dan)) != EMPTY) {
				komaNum++;
			}
		}
	}
	int bunbo = sujiMax * danMax * komaNum;
	if (bunbo == 0) {
		return MIN_SIKOU_DEPTH;
	}
	return min(MAX_SIKOU_DEPTH, max(MIN_SIKOU_DEPTH, 10000/bunbo));
}

Te Sikou::Think(Teban teban, KyokumenKomagumi k, const Kyokumen &kForJoseki)
{
#if defined(DEBUG_PRINT)
	int tesu = k.GetTeNum() + 1;
//	if (tesu == 39) {
//		g_bDebugPrint = true;
//	}
//	else {
		g_bDebugPrint = false;
//	}
//	printf("teNum[%d] teban[0x%02x]\n", k.GetTeNum(), teban);
//	ASSERT(k.GetTeban() == teban);
#endif // defined(DEBUG_PRINT).
	
//	TsumeHash::Clear();
#if defined(WINDOWS)
	m_ThinkStart = time(NULL);
#else // defined(WINDOWS).
	m_ThinkStart = std::chrono::system_clock::now();
#endif // defined(WINDOWS).
	
	int i, j;
	if (k.IsUseJoseki()) {
		int teNum;
		Te teBuf[MAX_TE_IN_KYOKUMEN];
		int hindo[MAX_TE_IN_KYOKUMEN];
		m_Joseki.fromJoseki(kForJoseki, SELF, k, k.GetTeNum(), teNum, teBuf, hindo);
		// 一番頻度の高い定跡を選ぶ。
		if (teNum > 0) {
			int max, maxhindo;
			max = 0;
			maxhindo = hindo[max];
			for (i=1; i < teNum; i++) {
				if (hindo[i] > maxhindo) {
					maxhindo = hindo[i];
					max = i;
				}
			}
			return teBuf[max];
		}
	}
	
//	int depthMax = 4;
	int depthMax = s_decideDepthMax(k);
	for (i=0; i < MAX_DEPTH; i++) {
		for (j=0; j < MAX_DEPTH; j++) {
			Best[i][j] = Te(0);
		}
	}
//	int bestVal = MinMax(teban, k, 0, depthMax);	// 三手読み+駒取り.
//	int bestVal = AlphaBeta(teban, k, -VAL_INFINITE, VAL_INFINITE, 0, depthMax);	// 三手読み+駒取り.
//	int bestVal = NegaAlphaBeta(teban, k, -VAL_INFINITE, VAL_INFINITE, 0, depthMax);	// 三手読み+駒取り.
	int bestVal = ITDeep(teban, k, -VAL_INFINITE+1, VAL_INFINITE-1, 0, depthMax);	// 三手読み+駒取り.
	
#if defined(DEBUG_PRINT)
#if defined(WINDOWS)
	time_t ms = (time(NULL) - m_ThinkStart) * 1000;
#else // defined(WINDOWS).
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_ThinkStart);
#endif // defined(WINDOWS).
	printf("[%d]手目 bestVal[%d] %lldms\n", tesu, bestVal, ms.count());
	for (i=0; i < depthMax; i++) {
		Best[0][i].Print();
//		printf("koma[0x%02x]\n", Best[0][i].GetKoma());
	}
	printf("\n");
#endif // defined(DEBUG_PRINT).
	
	return Best[0][0];
}

int Sikou::MinMax(Teban teban, KyokumenKomagumi k, int depth, int depthMax)
{
	Te teBuf[MAX_TE_IN_KYOKUMEN];
	if (depth >= depthMax) {
		// 現在の局面の評価値を返す。
		return k.GetValue() + k.BestEval(teban);
	}
	int teNum = k.MakeLegalMoves(teban, teBuf);
	int i;
	if (teNum == 0) {
		// 負け.
		if (teban == SELF) {
			return -VAL_INFINITE;
		}
		else {
			return VAL_INFINITE;
		}
	}
	// 乱数性を持たせるためにシャッフル
	//	for(i=0;i<min(10,teNum);i++) {
	//		swap(teBuf[i],teBuf[rand()%teNum]);
	//	}
	int retval;
	if (teban == SELF) {
		retval = -VAL_INFINITE-1;
	}
	else {
		retval = +VAL_INFINITE+1;
	}
	for (i=0; i < teNum; i++) {
		KyokumenKomagumi kk(k);
		kk.Move(teban, teBuf[i], true);
		int v = MinMax(teban==SELF?ENEMY:SELF, kk, depth+1, depthMax);
		if ((teban == SELF && v > retval) || (teban == ENEMY && v < retval)) {
			retval = v;
			Best[depth][depth] = teBuf[i];
			for (int j=depth+1; j < depthMax; j++) {
				Best[depth][j] = Best[depth+1][j];
			}
		}
	}
	return retval;
}

int Sikou::AlphaBeta(Teban teban, KyokumenKomagumi k, int alpha, int beta, int depth, int depthMax)
{
	Te teBuf[MAX_TE_IN_KYOKUMEN];
	if (depth >= depthMax) {
		// 現在の局面の評価値を返す。
		return k.GetValue() + k.BestEval(teban);
	}
	int teNum = k.MakeLegalMoves(teban, teBuf);
	int i;
	if (teNum == 0) {
		// 負け.
		if (teban == SELF) {
			return -VAL_INFINITE;
		}
		else {
			return VAL_INFINITE;
		}
	}
	//	// 乱数性を持たせるためにシャッフル.
	//	for(i=0;i<min(10,teNum);i++) {
	//		swap(teBuf[i],teBuf[rand()%teNum]);
	//	}
	int retval;
	if (teban == SELF) {
		retval = -VAL_INFINITE-1;
	}
	else {
		retval = +VAL_INFINITE+1;
	}
	for (i=0; i < teNum; i++) {
		KyokumenKomagumi kk(k);
		kk.Move(teban, teBuf[i], true);
		int v = AlphaBeta(teban==SELF?ENEMY:SELF, kk, alpha, beta, depth+1, depthMax);
		if ((teban == SELF && v > retval) || (teban == ENEMY && v < retval)) {
			retval = v;
			Best[depth][depth] = teBuf[i];
			for (int j=depth+1; j < depthMax; j++) {
				Best[depth][j] = Best[depth+1][j];
			}
			if (teban == SELF) {
				alpha = retval;
			}
			if (teban == ENEMY) {
				beta = retval;
			}
			if ((teban == SELF && retval >= beta) || (teban == ENEMY && retval <= alpha)) {
				return retval;
			}
		}
	}
	return retval;
}

int Sikou::NegaAlphaBeta(Teban teban, KyokumenKomagumi &k, int alpha, int beta, int depth, int depthMax, bool bITDeep)
{
	if (*ChudanFlag != 0) {
		return -VAL_INFINITE;
	}
	if (depth == depthMax-1) {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	if (depth == 1) {
		int sennitite = 0;
		int teNum;
		for (teNum=k.GetTeNum(); teNum > 0; teNum-=2) {
			if (k.GetHashHistory(teNum) == k.GetHashVal()) {
				sennitite++;
			}
		}
		if (sennitite >= SENNITITE_NUM) {
			sennitite = 0;
			for (teNum=k.GetTeNum(); teNum > 0; teNum-=2) {
				if (k.GetOuteHistory(teNum) == 0) {
					break;
				}
				if (k.GetHashHistory(teNum) == k.GetHashVal()) {
					sennitite++;
					if (sennitite >= SENNITITE_NUM) {
						return VAL_INFINITE;
					}
				}
			}
			
			sennitite = 0;
			for (teNum=k.GetTeNum(); teNum > 1; teNum-=2) {
				if (k.GetOuteHistory(teNum-1) == 0) {
					break;
				}
				if (k.GetHashHistory(teNum) == k.GetHashVal()) {
					sennitite++;
					if (sennitite >= SENNITITE_NUM) {
						return -VAL_INFINITE;
					}
				}
			}
			return 0;
		}
	}
	
#if defined(WINDOWS)
	bool bTimeout = (time(NULL) > m_ThinkStart + SIKOU_TIMEOUT_SECOND);
#else // defined(WINDOWS).
	bool bTimeout = (std::chrono::system_clock::now() > m_ThinkStart + std::chrono::seconds(SIKOU_TIMEOUT_SECOND));
#endif // defined(WINDOWS).
	
	if (bTimeout || depth == depthMax) {
//		int value = k.GetValue() + k.BestEval(teban);
		int val = k.GetValue();
		int eval = k.Evaluate();
		int bval = k.BestEval(teban);
		int value = eval + bval;
//		int value = k.Evaluate() + k.BestEval(teban);
		// 自分の手番から見た得点を返す.
		if (teban == SELF) {
#if defined(DEBUG_PRINT)
if (g_bDebugPrint) {
	printf("value1[%d]=val[%d]+eval[%d]+bval[%d]\n", value, val, eval, bval);
}
#endif // defined(DEBUG_PRINT).
			return value;
		}
		else {
#if defined(DEBUG_PRINT)
if (g_bDebugPrint) {
	printf("value2[%d]=-(val[%d]+eval[%d]+bval[%d])\n", -value, val, eval, bval);
}
#endif // defined(DEBUG_PRINT).
			return -value;
		}
	}
	
	HashEntry *pHashEntry = GetHashTbl(k.GetHashVal());
	if (pHashEntry != nullptr && pHashEntry->GetHashVal() == k.GetHashVal()) {
		if (pHashEntry->GetValue() >= beta &&
			pHashEntry->GetTeNum() >= k.GetTeNum() &&
			pHashEntry->GetTeban() == k.GetTeban() &&
			pHashEntry->GetDepth() <= depth &&
			pHashEntry->GetRemainDepth() >= depthMax-depth &&
			pHashEntry->GetFlag() != HashEntry::UPPER_BOUND) {
			return pHashEntry->GetValue();
		}
		if (pHashEntry->GetValue() <= alpha &&
			pHashEntry->GetTeNum() >= k.GetTeNum() &&
			pHashEntry->GetTeban() == k.GetTeban() &&
			pHashEntry->GetDepth() <= depth &&
			pHashEntry->GetRemainDepth() >= depthMax-depth &&
			pHashEntry->GetFlag() != HashEntry::LOWER_BOUND) {
			return pHashEntry->GetValue();
		}
	}
	else if (depthMax - depth > 2 && bITDeep) {
		return ITDeep(teban, k, alpha, beta, depth, depthMax);
	}
	
	Te teBuf[MAX_TE_IN_KYOKUMEN];
	int retval = -VAL_INFINITE-1;
	int teNum = 0;
	
	if (depth < 2 && k.Mate(teban, MAX_MATE_DEPTH, teBuf[0]) == 1) {
		Best[depth][depth] = teBuf[0];
		Best[depth][depth+1] = Te(0);
		retval = VAL_INFINITE+1;
		goto HASH_ADD;
	}
	
	teNum = MakeMoveFirst(teban, depth, teBuf, k);
	
	int i;
	k.EvaluateTe(teban, teNum, teBuf);
	for (i=0; i < teNum; i++) {
		KyokumenKomagumi kk(k);
		if (teBuf[i].IsNull()) {
			continue;
		}
		m_Stack[depth] = teBuf[i];
		kk.Move(teban, teBuf[i], true);
//#if defined(DEBUG_PRINT)
//if (g_bDebugPrint) {
//	if (depth == 0) {
//		printf("# ");
//	}
//	printf("NegaAlphaBeta1 depth[%d]", depth);
//	teBuf[i].Print();
//	printf("\n");
//}
//#endif // defined(DEBUG_PRINT).
		int v = -NegaAlphaBeta(teban==SELF?ENEMY:SELF, kk, -beta, -max(alpha,retval), depth+1, depthMax);
		if (depth > 1 && m_Stack[depth-1].GetValue() < 0 && m_Stack[depth-1].GetTo() == m_Stack[depth].GetTo() && v <= retval) {
			v = -NegaAlphaBeta(teban==SELF?ENEMY:SELF, kk, -beta, -max(alpha,retval), depth+1, depthMax+2);
		}
		if (v > retval) {
			retval = v;
			Best[depth][depth] = teBuf[i];
			for (int j=depth+1; j < depthMax; j++) {
				Best[depth][j] = Best[depth+1][j];
			}
			if (retval >= beta) {
				goto HASH_ADD;
			}
		}
	}
	
	teNum = k.MakeLegalMoves(teban, teBuf);
	if (teNum == 0) {
		// 負け.
		return -VAL_INFINITE;
	}
	k.EvaluateTe(teban, teNum, teBuf);
	for (i=0; i < teNum; i++) {
		if (depth > 2 && (teBuf[i].GetValue() < -100 || i > 30) && i > 0 && retval > -VAL_INFINITE) {
//#if defined(DEBUG_PRINT)
//if (g_bDebugPrint) {
//	if (depth < 2) {
//		printf("depth[%d] i[%d]\n", depth, i);
//		int j;
//		for (j=0; j < teNum; j++) {
//			teBuf[j].Print();
//		}
//		printf("\n");
//	}
//}
//#endif // defined(DEBUG_PRINT).
			break;
		}
		KyokumenKomagumi kk(k);
		m_Stack[depth] = teBuf[i];
		kk.Move(teban, teBuf[i], true);
//#if defined(DEBUG_PRINT)
//if (g_bDebugPrint) {
//	if (depth == 0) {
//		printf("# ");
//	}
//	printf("NegaAlphaBeta2 depth[%d]", depth);
//	teBuf[i].Print();
//	printf("\n");
//}
//#endif // defined(DEBUG_PRINT).
		int v = -NegaAlphaBeta(teban==SELF?ENEMY:SELF, kk, -beta, -max(alpha,retval), depth+1, depthMax);
		if (depth > 1 && m_Stack[depth-1].GetValue() < 0 && m_Stack[depth-1].GetTo() == m_Stack[depth].GetTo() && v <= beta) {
			v = -NegaAlphaBeta(teban==SELF?ENEMY:SELF, kk, -beta, -max(alpha,retval), depth+1, depthMax+2);
		}
		if (v > retval) {
			retval = v;
			Best[depth][depth] = teBuf[i];
			for(int j=depth+1; j < depthMax; j++) {
				Best[depth][j] = Best[depth+1][j];
			}
//			if (retval > alpha) {
//				alpha = retval;
//			}
			if (retval >= beta) {
//				return retval;
				goto HASH_ADD;
			}
		}
	}
	
HASH_ADD:
	pHashEntry = GetHashTbl(k.GetHashVal());
	if (pHashEntry == nullptr) {
		goto NOT_ADD;
	}
	if (pHashEntry->GetHashVal() == k.GetHashVal()) {
		pHashEntry->SetSecond(pHashEntry->GetBest());
	}
	else {
		if (pHashEntry->GetTeNum() - pHashEntry->GetDepth() == k.GetTeNum() - depth && pHashEntry->GetRemainDepth() > depthMax-depth) {
			// ハッシュにあるデータの方が重要なので上書きしない.
			goto NOT_ADD;
		}
		pHashEntry->SetHashVal(k.GetHashVal());
		pHashEntry->SetSecond(Te(0));
	}
	if (retval > alpha) {
		pHashEntry->SetBest(Best[depth][depth]);
	}
	else {
		pHashEntry->SetBest(Te(0));
	}
	pHashEntry->SetValue(retval);
	if (retval <= alpha) {
		pHashEntry->SetFlag(HashEntry::UPPER_BOUND);
	}
	else if (retval >= beta) {
		pHashEntry->SetFlag(HashEntry::LOWER_BOUND);
	}
	else {
		pHashEntry->SetFlag(HashEntry::EXACTLY_VALUE);
	}
	pHashEntry->SetDepth(depth);
	pHashEntry->SetRemainDepth(depthMax-depth);
	pHashEntry->SetTeNum(k.GetTeNum());

NOT_ADD:
	return retval;
}

int Sikou::ITDeep(Teban teban, KyokumenKomagumi &k, int alpha, int beta, int depth, int depthMax)
{
	int retval = 0;
	int i;
	for (i=depth+1; i <= depthMax; i++) {
		retval = NegaAlphaBeta(teban, k, alpha, beta, depth, i, false);
	}
	return retval;
}

int Sikou::MakeMoveFirst(Teban teban, int depth, Te teBuf[], KyokumenKomagumi k)
{
	int teNum = 0;
	HashEntry *pHashEntry = GetHashTbl(k.GetHashVal());
	if (pHashEntry == nullptr) {
		return 0;
	}
	if (pHashEntry->GetHashVal() != k.GetHashVal()) {
		return 0;
	}
	if (pHashEntry->GetTeban() != k.GetTeban()) {
		return 0;
	}
	
	Te te = pHashEntry->GetBest();
	if (! te.IsNull()) {
		if (k.IsLegalMove(teban, te)) {
			teBuf[teNum++] = te;
		}
	}
	te = pHashEntry->GetSecond();
	if (! te.IsNull()) {
		if (k.IsLegalMove(teban, te)) {
			teBuf[teNum++] = te;
		}
	}
	if (depth > 1) {
		te = Best[depth-1][depth];
		if (! te.IsNull()) {
			if (k.IsLegalMove(teban, te)) {
				teBuf[teNum++] = te;
			}
		}
	}
	return teNum;
}

void Sikou::InitJoseki()
{
	char filenames[] = "public.bin";
	m_Joseki.Init(filenames);
}
