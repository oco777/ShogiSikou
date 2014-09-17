//
//  Kyokumen.cpp
//  MySampleProject
//
//  Created by 小森久 on 2014/06/11.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ShogiDefs.h"
#include "Kyokumen.h"
#include "TsumeHash.h"

UINT64 Kyokumen::m_HashSeed[ERY+1][SUJIDAN2KOMAPOS(9,9)+1];
UINT64 Kyokumen::m_HandHashSeed[EHI+1][MAX_FU+1];
UINT64 Kyokumen::m_HashHistory[MAX_TE_HISTORY+1];
Kiki Kyokumen::m_OuteHistory[MAX_TE_HISTORY+1];

Kyokumen::Kyokumen(int suji, int dan, int promote, int teNum, KomaInf ban[MAX_DAN][MAX_SUJI], int hand[EHI+1])
{
	Init(suji, dan, promote, teNum, ban, hand);
}

void Kyokumen::Init(int suji, int dan, int promote, int teNum, KomaInf ban[MAX_DAN][MAX_SUJI], int hand[EHI+1])
{
	HashInit();
	m_KyokumenHashVal = 0;
	m_HandHashVal = 0;
	m_HashVal = 0;
	
	memset(m_BanPadding1, WALL, sizeof(m_BanPadding1));
	memset(m_Ban, WALL, sizeof(m_Ban));
	memset(m_BanPadding2, WALL, sizeof(m_BanPadding2));
	m_TeNum = teNum;
	m_LastTe = Te(0, 0, EMPTY, EMPTY);
	m_KingS = 0;
	m_KingE = 0;
	m_Suji = suji;
	m_Dan = dan;
	m_Promote = promote;
	m_Value = 0;
	
	int idxD;
	int idxS;
	for (idxD=0; idxD < m_Dan; idxD++) {
		for (idxS=0; idxS < m_Suji; idxS++) {
			KomaPos pos = COLROW2KOMAPOS(idxS+1, m_Suji, idxD+1, m_Dan);
			m_Ban[pos] = ban[idxD][idxS];
			m_KyokumenHashVal ^= m_HashSeed[m_Ban[pos]][pos];
			if (m_Ban[pos] == SOU) {
				m_KingS = pos;
			}
			else if (m_Ban[pos] == EOU) {
				m_KingE = pos;
			}
			m_Value += KomaValue[m_Ban[pos]];
		}
	}
	
	KomaInf inf;
	int num;
	for (inf=EMPTY; inf < EHI+1; inf++) {
		m_Hand[inf] = hand[inf];
		m_Value += HandValue[inf] * m_Hand[inf];
		for (num=1; num <= m_Hand[inf]; num++) {
			m_HandHashVal ^= m_HandHashSeed[inf][num];
		}
	}
	
	InitControl();
	
	m_HashVal = m_KyokumenHashVal ^ m_HandHashVal;
	int i;
	for (i=0; i <= MAX_TE_HISTORY; i++) {
		m_HashHistory[i] = 0;
		m_OuteHistory[i] = 0;
	}
	m_HashHistory[m_TeNum] = m_HashVal;
	if (GetTeban() == SELF) {
		// 現在の手番が先手ということは、現在の局面に成った指し手は後手ということになる.
		// 後手が王手をしている状態かどうかを履歴に取っておく.
		m_OuteHistory[m_TeNum] = m_ControlE[m_KingS];
	}
	else {
		m_OuteHistory[m_TeNum] = m_ControlS[m_KingE];
	}
}

void Kyokumen::InitControl()
{
	memset(m_ControlS, 0, sizeof(m_ControlS));
	memset(m_ControlE, 0, sizeof(m_ControlE));
	
	int suji;
	int dan;
	int dir, move, jump;
	KomaPos pos2;
	for (suji=1; suji <= m_Suji; suji++) {
		for (dan=1; dan <= m_Dan; dan++) {
			KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
			if (m_Ban[pos] & ENEMY) {
				for (dir=DIRECT_START, move=KIKI_MOVE_START, jump=KIKI_JUMP_START; dir < DIRECT_NUM; dir++, move<<=1, jump<<=1) {
					if (CanJump[dir][m_Ban[pos]]) {
						pos2 = pos;
						do {
							pos2 += Direct[dir];
							m_ControlE[pos2] |= jump;
						} while (m_Ban[pos2] == EMPTY);
					} else if (CanMove[dir][m_Ban[pos]]) {
						m_ControlE[pos+Direct[dir]] |= move;
					}
				}
			}
			else if (m_Ban[pos] & SELF) {
				for (dir=DIRECT_START, move=KIKI_MOVE_START, jump=KIKI_JUMP_START; dir < DIRECT_NUM; dir++, move<<=1, jump<<=1) {
					if (CanJump[dir][m_Ban[pos]]) {
						pos2 = pos;
						do {
							pos2 += Direct[dir];
							m_ControlS[pos2] |= jump;
						} while (m_Ban[pos2] == EMPTY);
					} else if (CanMove[dir][m_Ban[pos]]) {
						m_ControlS[pos+Direct[dir]] |= move;
					}
				}
			}
		}
	}
}

int Kyokumen::MakeLegalMoves(Teban teban, Te *teBuf, int *pin)
{
	int pbuf[16*11];
	int teNum = 0;
	if (pin == nullptr) {
		MakePinInf(pbuf);
		pin = pbuf;
	}
	if (teban == SELF && m_ControlE[m_KingS] != 0) {
		return AntiCheck(teban, teBuf, pin, m_ControlE[m_KingS]);
	}
	if (teban == ENEMY && m_ControlS[m_KingE] != 0) {
		return AntiCheck(teban, teBuf, pin, m_ControlS[m_KingE]);
	}
	
	int suji, dan;
	int sujiMax, danMax;
	int StartDan, EndDan;
	
	sujiMax = GetSuji();
	danMax = GetDan();
	
	// 盤上の駒を動かす.
	for (suji=1; suji <= sujiMax; suji++) {
		for (dan=1; dan <= danMax; dan++) {
			KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
			if ((m_Ban[pos] & teban) != 0) {
				AddMoves(teban, teNum, teBuf, pos, pin[pos]);
			}
		}
	}
	// 歩を打つ.
	if (m_Hand[teban|FU] > 0) {
		for (suji=1; suji <= sujiMax; suji++) {
			// 二歩チェック.
			int nifu = 0;
			for (dan=1; dan <= danMax; dan++) {
				KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
				if (m_Ban[pos]== (teban|FU)) {
					nifu = true;
					break;
				}
			}
			if (nifu) continue;
			// (先手なら２段目より下に、後手なら８段目より上に打つ）.
			if (teban == SELF) {
				StartDan = 2;
				EndDan = danMax;
			} else {
				StartDan = 1;
				EndDan = danMax-1;
			}
			for (dan=StartDan; dan<=EndDan; dan++) {
				// 打ち歩詰めもチェック.
				KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
				if (m_Ban[pos] == EMPTY && !Utifudume(teban, pos, pin)) {
					teBuf[teNum++] = Te(0, pos, teban|FU, EMPTY);
				}
			}
		}
	}
	// 香を打つ.
	if (m_Hand[teban|KY] > 0) {
		for (suji=1; suji <= sujiMax; suji++) {
			// (先手なら２段目より下に、後手なら８段目より上に打つ）.
			if (teban == SELF) {
				StartDan = 2;
				EndDan = danMax;
			} else {
				StartDan = 1;
				EndDan = danMax-1;
			}
			for (dan=StartDan; dan <= EndDan; dan++) {
				KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
				if (m_Ban[pos] == EMPTY) {
					teBuf[teNum++] = Te(0, pos, teban|KY, EMPTY);
				}
			}
		}
	}
	// 桂を打つ.
	if (m_Hand[teban|KE] > 0) {
		// (先手なら３段目より下に、後手なら７段目より上に打つ）.
		for(suji=1; suji <= sujiMax; suji++) {
			if (teban == SELF) {
				StartDan = 3;
				EndDan = danMax;
			} else {
				StartDan = 1;
				EndDan = danMax-2;
			}
			for (dan=StartDan; dan <= EndDan; dan++) {
				KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
				if (m_Ban[pos] == EMPTY) {
					teBuf[teNum++] = Te(0, pos, teban|KE, EMPTY);
				}
			}
		}
	}
	// 銀〜飛車は、どこにでも打てる.
	for (int koma=GI; koma <= HI; koma++) {
		if (m_Hand[teban|koma] > 0) {
			for (suji=1; suji <= sujiMax; suji++) {
				for (dan=1; dan <= danMax; dan++) {
					KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
					if (m_Ban[pos] == EMPTY) {
						teBuf[teNum++] = Te(0, pos, teban|koma, EMPTY);
					}
				}
			}
		}
	}
	
	return teNum;
}

void Kyokumen::MakePinInf(int *pin) const
{
	int i, dir;
	int sujiMax = GetSuji();
	int danMax = GetDan();
	
	// ピン情報を設定する.
	for (i=SUJIDAN2KOMAPOS(1, 1); i <= SUJIDAN2KOMAPOS(sujiMax, danMax); i++) {
		// 0はピンされていない、という意味.
		pin[i] = 0;
	}
	if (m_KingS != 0) {	// 自玉が盤面にある時のみ有効.
		for (dir=DIRECT_START; dir < DIRECT_NUM_FOR_PIN; dir++) {
			int p;
			p = search(m_KingS, -Direct[dir]);
			if ((m_Ban[p] != WALL) && ((m_Ban[p] & ENEMY) == 0)) { // 味方の駒が有る.
				if ((m_ControlE[p] & (1<<(KIKI_JUMP_OFFSET+dir))) != 0) {
					pin[p] = Direct[dir];
				}
			}
		}
	}
	if (m_KingE != 0) {	// 敵玉が盤面にある時のみ有効.
		for (dir=DIRECT_START; dir < DIRECT_NUM_FOR_PIN; dir++) {
			int p;
			p = search(m_KingE, -Direct[dir]);
			if ((m_Ban[p] != WALL) && ((m_Ban[p] & ENEMY) != 0)) { // 敵の駒が有る.
				if ((m_ControlS[p] & (1<<(KIKI_JUMP_OFFSET+dir))) != 0) {
					pin[p] = Direct[dir];
				}
			}
		}
	}
}

int Kyokumen::AntiCheck(Teban teban, Te *teBuf,int *pin, Kiki kiki)
{
	int king;
	int teNum = 0;
	if ((kiki & (kiki-1)) != 0) {
        // 両王手は玉を動かすしかない.
        MoveKing(teban, teNum, teBuf, kiki);
	} else {
		if (teban == SELF) {
			king = m_KingS;
		} else {
			king = m_KingE;
		}
		unsigned int id;
		int check;
        for (id = 0; id <= 31; id++) {
            if (kiki == (1u << id)) break;
        }
		if (id < KIKI_JUMP_OFFSET) {
            check = king - Direct[id];
		} else {
			check = search(king, -Direct[id-KIKI_JUMP_OFFSET]);
		}
        // 王手駒を取る.
        MoveTo(teban, teNum, teBuf, check, pin);
		
        // 玉を動かす.
        MoveKing(teban, teNum, teBuf, kiki);
		
		if (id >= KIKI_JUMP_OFFSET) {
            // 合駒をする手を生成する.
            int i;
            for (i=king-Direct[id-KIKI_JUMP_OFFSET]; m_Ban[i] == EMPTY; i-=Direct[id-KIKI_JUMP_OFFSET]) {
				MoveTo(teban, teNum, teBuf, i, pin); // 移動合.
            }
            for (i=king-Direct[id-KIKI_JUMP_OFFSET]; m_Ban[i] == EMPTY; i-=Direct[id-KIKI_JUMP_OFFSET]) {
				PutTo(teban, teNum, teBuf, i, pin);  // 駒を打つ合.
            }
        }
	}
	return teNum;
}

void Kyokumen::AddMoves(Teban teban, int &teNum, Te *teTop, KomaPos from, int pin, int Rpin)
{
	switch(m_Ban[from]) {
		case SFU:
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_UP], pin, Rpin);
			break;
		case EFU:
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_DOWN], pin, Rpin);
			break;
		case SKY:
			AddStraight(teban, teNum, teTop, from, Direct[DIRECT_UP], pin, Rpin);
			break;
		case EKY:
			AddStraight(teban, teNum, teTop, from, Direct[DIRECT_DOWN], pin, Rpin);
			break;
		case SKE:
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_SKE_1], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_SKE_2], pin, Rpin);
			break;
		case EKE:
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_EKE_1], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_EKE_2], pin, Rpin);
			break;
		case SGI:
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_UP], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT_UP], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT_UP], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT_DOWN], pin, Rpin);
			break;
		case EGI:
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT_UP], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT_UP], pin, Rpin);
			break;
		case SKI:case STO:case SNY:case SNK:case SNG:
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_UP], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT_UP], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT_UP], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT], pin, Rpin);
			break;
		case EKI:case ETO:case ENY:case ENK:case ENG:
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_UP], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT], pin, Rpin);
			break;
		case SRY:case ERY:
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT_UP], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT_UP], pin, Rpin);
		case SHI:case EHI:
			AddStraight(teban, teNum, teTop, from, Direct[DIRECT_DOWN], pin, Rpin);
			AddStraight(teban, teNum, teTop, from, Direct[DIRECT_UP], pin, Rpin);
			AddStraight(teban, teNum, teTop, from, Direct[DIRECT_RIGHT], pin, Rpin);
			AddStraight(teban, teNum, teTop, from, Direct[DIRECT_LEFT], pin, Rpin);
			break;
		case SUM:case EUM:
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_DOWN], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_LEFT], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_RIGHT], pin, Rpin);
			AddMove(teban, teNum, teTop, from, Direct[DIRECT_UP], pin, Rpin);
		case SKA:case EKA:
			AddStraight(teban, teNum, teTop, from, Direct[DIRECT_LEFT_DOWN], pin, Rpin);
			AddStraight(teban, teNum, teTop, from, Direct[DIRECT_RIGHT_UP], pin, Rpin);
			AddStraight(teban, teNum, teTop, from, Direct[DIRECT_LEFT_UP], pin, Rpin);
			AddStraight(teban, teNum, teTop, from, Direct[DIRECT_RIGHT_DOWN], pin, Rpin);
			break;
		case SOU:case EOU:
			MoveKing(teban, teNum, teTop, 0);	// 王手がかかっている時には、AntiCheckの方が呼ばれるから、Kikiは０です。
			break;
	}
}

int Kyokumen::Utifudume(Teban teban, KomaPos to, int *pin)
{
	if (teban == SELF) {
		// まず、玉の頭に歩を打つ手じゃなければ打ち歩詰めの心配はない。
		if (m_KingE+Direct[DIRECT_DOWN] != to) {
			return 0;
		}
	} else {
		// まず、玉の頭に歩を打つ手じゃなければ打ち歩詰めの心配はない。
		if (m_KingS+Direct[DIRECT_UP] != to) {
			return 0;
		}
	}
	//実際に歩を打って確かめてみる。
	m_Ban[to] = FU|teban;
	if (teban == SELF) {
		// 自分の利きがあったら相手は玉で取れない　＆　取る動きを列挙してみたら玉で取る手しかない.
		if ((m_ControlS[to] != 0) && (CountMove(ENEMY, to, pin) == KIKI_MOVE_DOWN)) {
			// 玉に逃げ道があるかどうかをチェック.
			for (int i=0; i < DIRECT_NUM_FOR_PIN; i++) {
				KomaInf koma = m_Ban[m_KingE+Direct[i]];
				if (((koma & ENEMY) == 0) && !CountControlS(m_KingE+Direct[i])) {
					// 逃げ道があったので、盤面を元の状態に戻して、
					m_Ban[to] = EMPTY;
					// 打ち歩詰めではなかった。
					return 0;
				}
			}
			// 玉の逃げ道もないのなら、打ち歩詰め。盤面の状態は元に戻す。
			m_Ban[to] = EMPTY;
			return 1;
		}
		// 玉以外で取れる手があるか、玉で取れる。
		m_Ban[to] = EMPTY;
		return 0;
	} else {
		// 自分の利きがあったら相手は玉で取れない　＆　取る動きを列挙してみたら玉で取る手しかない.
		if ((m_ControlE[to] != 0) && (CountMove(SELF, to, pin) == KIKI_MOVE_UP)) {
			// 玉に逃げ道があるかどうかをチェック.
			for (int i=0; i < DIRECT_NUM_FOR_PIN; i++) {
				KomaInf koma = m_Ban[m_KingS+Direct[i]];
				if (((koma & SELF) == 0) && !CountControlE(m_KingS+Direct[i])) {
					// 逃げ道があったので、盤面を元の状態に戻して、
					m_Ban[to] = EMPTY;
					// 打ち歩詰めではなかった。
					return 0;
				}
			}
			// 玉の逃げ道もないのなら、打ち歩詰め。盤面の状態は元に戻す。
			m_Ban[to] = EMPTY;
			return 1;
		}
		// 玉以外で取れる手があるか、玉で取れる。
		m_Ban[to] = EMPTY;
		return 0;
	}
}

void Kyokumen::MoveKing(Teban teban, int &teNum, Te *teTop, Kiki kiki)
{
	int i;
	int id = -1;	//隣接王手駒の位置のid
	// 両王手でないなら王手駒の位置を探す.
	for (i=0; i < DIRECT_NUM_FOR_PIN; i++) {
		if (kiki & (1 << i)) {
			id = i;
			break;
		}
	}
	if (id >= 0) {
		// 隣接の王手 最初に取る手を生成するのだ.
		if (teban == SELF) {
			KomaInf koma = m_Ban[m_KingS-Direct[id]];
			if ((koma == EMPTY || (koma & ENEMY) != 0)
				&& !CountControlE(m_KingS - Direct[id]) // 敵の駒が効いていない.
				&& (kiki & (1 << (DIRECT_RIGHT_UP+KIKI_JUMP_OFFSET-id))) == 0) { // 敵の飛駒で貫かれていない.
				AddMove(teban, teNum, teTop, m_KingS, -Direct[id], 0);
			}
		} else {
			KomaInf koma = m_Ban[m_KingE-Direct[id]];
			if ((koma == EMPTY || (koma & SELF) != 0)
				&& !CountControlS(m_KingE - Direct[id]) // 敵の駒が効いていない.
				&& (kiki & (1 << (DIRECT_RIGHT_UP+KIKI_JUMP_OFFSET-id))) == 0) {  // 敵の飛駒で貫かれていない.
				AddMove(teban, teNum, teTop, m_KingE, -Direct[id], 0);
			}
		}
	}
	for (i=0; i < DIRECT_NUM_FOR_PIN; i++) {
		if (i == id) continue;
		if (teban == SELF) {
			KomaInf koma = m_Ban[m_KingS-Direct[i]];
			if ((koma == EMPTY || (koma & ENEMY) != 0)
				&& !CountControlE(m_KingS - Direct[i]) // 敵の駒が効いていない.
				&& (kiki & (1 << (DIRECT_RIGHT_UP+KIKI_JUMP_OFFSET-i))) == 0) { // 敵の飛駒で貫かれていない.
				AddMove(teban,teNum, teTop, m_KingS, -Direct[i], 0);
			}
		} else {
			KomaInf koma = m_Ban[m_KingE-Direct[i]];
			if ((koma==EMPTY || (koma & SELF) != 0)
				&& !CountControlS(m_KingE - Direct[i]) // 敵の駒が効いていない.
				&& (kiki & (1 << (DIRECT_RIGHT_UP+KIKI_JUMP_OFFSET-i))) == 0) { // 敵の飛駒で貫かれていない.
				AddMove(teban,teNum, teTop, m_KingE, -Direct[i], 0);
			}
		}
	}
}

void Kyokumen::MoveTo(Teban teban, int &teNum, Te *teTop, KomaPos to, int* pin)
{
	int p;
	KomaInf koma;
	
	for(int i=0; i < DIRECT_NUM; i++) {
		koma = m_Ban[to-Direct[i]];
		if (koma == EMPTY) {
			p = search(to, -Direct[i]);
			if ((m_Ban[p] & teban) != 0 && CanJump[i][m_Ban[p]]) {
				AddMove(teban, teNum, teTop, p, to-p, pin[p]);
			}
		} else {
			if ((koma&~teban) != OU && (koma&teban) != 0 && (CanMove[i][koma] || CanJump[i][koma])) {
				AddMove(teban, teNum, teTop, to-Direct[i], Direct[i], pin[to-Direct[i]]);
			}
		}
	}
}

void Kyokumen::PutTo(Teban teban, int &teNum, Te *teTop, KomaPos to, int *pin)
{
	int dan = KOMAPOS2DAN(to);
	int danMax = GetDan();
	if (teban == ENEMY) {
		dan = danMax - dan + 1;
	}
	if (m_Hand[teban|FU] > 0 && dan > 1) {
		// 歩を打つ手を生成.
		// 二歩チェック.
		int suji = KOMAPOS2SUJI(to);
		int nifu = 0;
		for(int d=1;d <= danMax; d++) {
			KomaPos pos = SUJIDAN2KOMAPOS(suji, d);
			if (m_Ban[pos] == (teban|FU)) {
				nifu = 1;
				break;
			}
		}
		// 打ち歩詰めもチェック.
		if (nifu == 0 && !Utifudume(teban, to, pin)) {
			teTop[teNum++] = Te(0, to, teban|FU, EMPTY);
		}
	}
	if (m_Hand[teban|KY] > 0 && dan > 1) {
		// 香を打つ手を生成.
		teTop[teNum++] = Te(0, to, teban|KY, EMPTY);
	}
	if (m_Hand[teban|KE] > 0 && dan > 2) {
		teTop[teNum++] = Te(0, to, teban|KE, EMPTY);
	}
	for (int koma=GI; koma <= HI; koma++) {
		if (m_Hand[teban|koma] > 0) {
			teTop[teNum++] = Te(0, to, teban|koma, EMPTY);
		}
	}
}

void Kyokumen::AddMove(Teban teban, int &teNum, Te *teTop, KomaPos from, int diff, int pin, int Rpin)
{
	if (Rpin == diff || Rpin == -diff) {
		return;
	}
	KomaPos to = from + diff;
	int dan = KOMAPOS2DAN(to);
	int fromDan = KOMAPOS2DAN(from);
	int danMax = GetDan();
	int promote = GetPromote();
	
	if ((pin == 0 || pin == diff || pin == -diff) && (m_Ban[to]&teban) == 0) {
		if (m_Ban[from] == SKE && dan <= 2) {
			// 必ず成る.
			teTop[teNum++] = Te(from, to, m_Ban[from], m_Ban[to], 1);
		} else if ((m_Ban[from] == SFU || m_Ban[from] == SKY) && dan <= 1) {
			// 必ず成る.
			teTop[teNum++] = Te(from, to, m_Ban[from], m_Ban[to], 1);
		} else if (m_Ban[from] == EKE && dan >= danMax-1) {
			// 必ず成る.
			teTop[teNum++] = Te(from, to, m_Ban[from], m_Ban[to], 1);
		} else if ((m_Ban[from] == EFU || m_Ban[from] == EKY) && dan >= danMax) {
			// 必ず成る.
			teTop[teNum++] = Te(from, to, m_Ban[from], m_Ban[to], 1);
		} else {
			if (teban == SELF && (fromDan <= promote || dan <= promote) && CanPromote[m_Ban[from]]) {
				teTop[teNum++] = Te(from, to, m_Ban[from], m_Ban[to], 1);
			} else if (teban == ENEMY && (fromDan >= danMax-promote+1 || dan >= danMax-promote+1) && CanPromote[m_Ban[from]]) {
				teTop[teNum++] = Te(from, to, m_Ban[from], m_Ban[to], 1);
			}
			// 成らない手も生成する。
			teTop[teNum++] = Te(from, to, m_Ban[from], m_Ban[to], 0);
		}
	}
}

void Kyokumen::AddStraight(Teban teban, int &teNum, Te *teTop, KomaPos from, int dir, int pin, int Rpin)
{
	if (dir == Rpin || dir == -Rpin) {
		return;
	}
	int i;
	if (pin == 0 || pin == dir || pin == -dir) {
		// 空白の間、動く手を生成する.
		for(i=dir; m_Ban[from+i] == EMPTY; i+=dir) {
			AddMove(teban, teNum, teTop, from, i, 0);
		}
		// 味方の駒でないなら、そこへ動く.
		if ((m_Ban[from+i] & teban) == 0) {
			AddMove(teban, teNum, teTop, from, i, 0);
		}
	}
}

Kiki Kyokumen::CountMove(Teban teban, KomaPos pos, int *pin)
{
	Kiki ret = 0;
	int i, move, jump;
	move = KIKI_MOVE_START;
	jump = KIKI_JUMP_START;
	for(i=0; i < DIRECT_NUM; i++, move<<=1, jump<<=1) {
		if (CanMove[i][m_Ban[pos-Direct[i]]] && (m_Ban[pos-Direct[i]]&teban) != 0 && (pin[pos-Direct[i]] == 0 || pin[pos-Direct[i]] == Direct[i] || pin[pos-Direct[i]] == -Direct[i])) {
			ret |= move;
		} else {
			KomaPos pos2 = search(pos, -Direct[i]);
			if (CanJump[i][m_Ban[pos2]] && (m_Ban[pos2]&teban) != 0 && (pin[pos2] == 0 || pin[pos2] == Direct[i] || pin[pos2] == -Direct[i])) {
				ret |= jump;
			}
		}
	}
	return ret;
}

Kiki Kyokumen::CountControlS(KomaPos pos)
{
	Kiki ret = 0;
	int i, move, jump;
	move = KIKI_MOVE_START;
	jump = KIKI_JUMP_START;
	for(i=0; i < DIRECT_NUM; i++, move<<=1, jump<<=1) {
		if (CanMove[i][m_Ban[pos-Direct[i]]] && (m_Ban[pos-Direct[i]]&SELF) != 0) {
			ret |= move;
		} else {
			KomaPos pos2 = search(pos, -Direct[i]);
			if (CanJump[i][m_Ban[pos2]] && (m_Ban[pos2]&SELF) != 0) {
				ret |= jump;
			}
		}
	}
	return ret;
}

Kiki Kyokumen::CountControlE(KomaPos pos)
{
	Kiki ret = 0;
	int i, move, jump;
	move = KIKI_MOVE_START;
	jump = KIKI_JUMP_START;
	for(i=0; i < DIRECT_NUM; i++, move<<=1, jump<<=1) {
		if (CanMove[i][m_Ban[pos-Direct[i]]] && (m_Ban[pos-Direct[i]]&ENEMY) != 0) {
			ret |= move;
		} else {
			KomaPos pos2 = search(pos, -Direct[i]);
			if (CanJump[i][m_Ban[pos2]] && (m_Ban[pos2]&ENEMY) != 0) {
				ret |= jump;
			}
		}
	}
	return ret;
}

void Kyokumen::Move(Teban teban, const Te &te, bool bHashHistory)
{
	int i, j, move, jump;
	if (IS_BANPOS(te.GetFrom())) {
		// 元いた駒のコントロールを消す.
		int dir;
		for(dir=DIRECT_START, move=KIKI_MOVE_START, jump=KIKI_JUMP_START; dir < DIRECT_NUM; dir++, move<<=1, jump<<=1) {
			if (teban == SELF) {
				m_ControlS[te.GetFrom()+Direct[dir]] &= ~move;
			} else {
				m_ControlE[te.GetFrom()+Direct[dir]] &= ~move;
			}
			if (CanJump[dir][te.GetKoma()]) {
				KomaPos j = te.GetFrom();
				do {
					j += Direct[dir];
					if (teban == SELF) {
						m_ControlS[j] &= ~jump;
					} else {
						m_ControlE[j] &= ~jump;
					}
				} while (m_Ban[j] == EMPTY);
			}
		}
		// 元いた位置は空白になる.
		m_Ban[te.GetFrom()] = EMPTY;
		m_KyokumenHashVal ^= m_HashSeed[te.GetKoma()][te.GetFrom()];
		m_KyokumenHashVal ^= m_HashSeed[EMPTY][te.GetFrom()];
		// 飛び利きを伸ばす.
		for (i=0, jump=KIKI_JUMP_START; i < DIRECT_NUM_FOR_PIN; i++, jump<<=1) {
			int Dir = Direct[i];
			if ((m_ControlS[te.GetFrom()] & jump) != 0) {
				j = te.GetFrom();
				do {
					j += Dir;
					m_ControlS[j] |= jump;
				} while (m_Ban[j] == EMPTY);
			}
			if ((m_ControlE[te.GetFrom()] & jump) != 0) {
				j = te.GetFrom();
				do {
					j += Dir;
					m_ControlE[j] |= jump;
				} while (m_Ban[j] == EMPTY);
			}
		}
	} else {
		// 持ち駒から一枚減らす.
		m_HandHashVal ^= m_HandHashSeed[te.GetKoma()][m_Hand[te.GetKoma()]];
		m_Hand[te.GetKoma()]--;
		m_Value -= HandValue[te.GetKoma()];
		m_Value += KomaValue[te.GetKoma()];
	}
	if (m_Ban[te.GetTo()] != EMPTY) {
		// 相手の駒を持ち駒にする。
		// 持ち駒にする時は、成っている駒も不成りに戻す。（&‾PROMOTED）
		KomaInf inf = (teban | (m_Ban[te.GetTo()] & (~PROMOTED) & (~SELF) & (~ENEMY)));
		m_Value -= KomaValue[m_Ban[te.GetTo()]];
		m_Value += HandValue[inf];
		m_Hand[inf]++;
		m_HandHashVal ^= m_HandHashSeed[inf][m_Hand[inf]];
		// 取った駒の効きを消す.
		for (i=0, move=KIKI_MOVE_START, jump=KIKI_JUMP_START; i < DIRECT_NUM; i++, move<<=1, jump<<=1) {
			int Dir = Direct[i];
			if (CanJump[i][m_Ban[te.GetTo()]]) {
				j = te.GetTo();
				do {
					j += Dir;
					if (teban == SELF) {
						m_ControlE[j] &= ~jump;
					} else {
						m_ControlS[j] &= ~jump;
					}
				} while (m_Ban[j] == EMPTY);
			} else {
				j = te.GetTo() + Dir;
				if (teban == SELF) {
					m_ControlE[j] &= ~move;
				} else {
					m_ControlS[j] &= ~move;
				}
			}
		}
	} else {
		// 移動先で遮った飛び利きを消す.
		for (i=0, jump=KIKI_JUMP_START; i < DIRECT_NUM_FOR_PIN; i++, jump<<=1) {
			int Dir = Direct[i];
			if ((m_ControlS[te.GetTo()] & jump) != 0) {
				j = te.GetTo();
				do {
					j += Dir;
					m_ControlS[j] &= ~jump;
				} while (m_Ban[j] == EMPTY);
			}
			if ((m_ControlE[te.GetTo()] & jump) != 0) {
				j = te.GetTo();
				do {
					j += Dir;
					m_ControlE[j] &= ~jump;
				} while (m_Ban[j] == EMPTY);
			}
		}
	}
	m_KyokumenHashVal ^= m_HashSeed[m_Ban[te.GetTo()]][te.GetTo()];
	if (te.GetPromote()) {
		m_Value -= KomaValue[te.GetKoma()];
		m_Value += KomaValue[te.GetKoma()|PROMOTED];
		m_Ban[te.GetTo()] = te.GetKoma()|PROMOTED;
	} else {
		m_Ban[te.GetTo()] = te.GetKoma();
	}
	m_KyokumenHashVal ^= m_HashSeed[m_Ban[te.GetTo()]][te.GetTo()];
	// 移動先の利きをつける.
	for (i=0, move=KIKI_MOVE_START, jump=KIKI_JUMP_START; i < DIRECT_NUM; i++, move<<=1, jump<<=1) {
		if (CanJump[i][m_Ban[te.GetTo()]]) {
			j = te.GetTo();
			do {
				j += Direct[i];
				if (teban == SELF) {
					m_ControlS[j] |= jump;
				} else {
					m_ControlE[j] |= jump;
				}
			} while (m_Ban[j] == EMPTY);
		} else if (CanMove[i][m_Ban[te.GetTo()]]) {
			if (teban == SELF) {
				m_ControlS[te.GetTo()+Direct[i]] |= move;
			} else {
				m_ControlE[te.GetTo()+Direct[i]] |= move;
			}
		}
	}
	// 王様の位置は覚えておく。
	if (te.GetKoma() == SOU) {
		m_KingS = te.GetTo();
	}
	if (te.GetKoma() == EOU) {
		m_KingE = te.GetTo();
	}
	
	m_HashVal = m_KyokumenHashVal ^ m_HandHashVal;
	m_TeNum++;
	if (bHashHistory) {
		m_HashHistory[m_TeNum] = m_HashVal;
		m_OuteHistory[m_TeNum] = (teban == SELF) ? m_ControlS[m_KingE] : m_ControlE[m_KingS];
	}
	SetLastTe(te);
}

int Kyokumen::BestEval(Teban teban)
{
	// SorEの利きのある敵の駒（&SorE==0の駒）について、Evalを呼び出して、
	// 一番いい交換値を探す。
	int best = 0;
	int suji;
	int dan;
	int sujiMax = GetSuji();
	int danMax = GetDan();
	for (suji=1; suji <= sujiMax; suji++) {
		for (dan=1; dan <= danMax; dan++) {
			KomaPos pos = SUJIDAN2KOMAPOS(suji, dan);
			if ((m_Ban[pos] & teban) == 0) {
				int value = Eval(pos);
				if (value > best) {
					best = value;
				}
			}
		}
	}
	if (teban == ENEMY) {
		return -best;
	}
	return best;
}

int Kyokumen::Eval(KomaPos pos)
{
	if (m_Ban[pos] == EMPTY) {
		return 0;
	}
	if ((m_Ban[pos] & SELF) != 0 && !m_ControlE[pos]) {
		// 取られる心配がない.
		return 0;
	}
	if ((m_Ban[pos] & ENEMY) != 0 && !m_ControlS[pos]) {
		// 取られる心配がない.
		return 0;
	}
	
	int ret;
	Te teTop[40];
	KomaPos ToPos = pos;
	
	// AtackCountを得るように、駒のリストを得る.
	Te *AtackS = teTop;
	// 一個所への利きは、最大隣接8+桂馬2+飛飛角角香香香香=18だから。
	Te *AtackE = teTop + 18;
	
	int AtackCountE = 0;
	int AtackCountS = 0;
	KomaPos pos1 = ToPos;
	KomaPos pos2;
	
	int i;
	int move;
	int jump;
	int PromoteS = 0;
	int PromoteE = 0;
	if (KOMAPOS2DAN(ToPos) <= GetPromote()) {
		PromoteS = 1;
	}
	if (KOMAPOS2DAN(ToPos) >= GetDan()-GetPromote()+1) {
		PromoteE = 1;
	}
	
	// 桂馬の利きは別に数える.
	for (i=DIRECT_START, move=KIKI_MOVE_START, jump=KIKI_JUMP_START; i < DIRECT_NUM_FOR_PIN; i++, move<<=1, jump<<=1) {
		pos2 = pos1;
		if ((m_ControlS[pos1] & move) != 0) {
			pos2 -= Direct[i];
			AtackS[AtackCountS].SetFrom(pos2);
			AtackS[AtackCountS].SetKoma(m_Ban[pos2]);
			AtackS[AtackCountS].SetTo(pos1);
			if ((PromoteS != 0 || KOMAPOS2DAN(pos2) <= GetPromote()) && CanPromote[AtackS[AtackCountS].GetKoma()]) {
				AtackS[AtackCountS].SetPromote(1);
			}
			else {
				AtackS[AtackCountS].SetPromote(0);
			}
			AtackCountS++;
		}
		else if ((m_ControlE[pos1] & move) != 0) {
			pos2 -= Direct[i];
			AtackE[AtackCountE].SetFrom(pos2);
			AtackE[AtackCountE].SetKoma(m_Ban[pos2]);
			AtackE[AtackCountE].SetTo(pos1);
			if ((PromoteE != 0 || KOMAPOS2DAN(pos2) >= GetDan()-GetPromote()+1) && CanPromote[AtackE[AtackCountE].GetKoma()]) {
				AtackE[AtackCountE].SetPromote(1);
			}
			else {
				AtackE[AtackCountE].SetPromote(0);
			}
			AtackCountE++;
		}
		if (m_Ban[pos1-Direct[i]] != SOU && m_Ban[pos1-Direct[i]] != EOU) {
			while ((m_ControlS[pos2] & jump) != 0 || (m_ControlE[pos2] & jump) != 0) {
				pos2 -= Direct[i];
				while (m_Ban[pos2] == EMPTY) {
					pos2 -= Direct[i];
				}
				if ((m_Ban[pos2] & ENEMY) != 0) {
					AtackE[AtackCountE].SetFrom(pos2);
					AtackE[AtackCountE].SetKoma(m_Ban[pos2]);
					AtackE[AtackCountE].SetTo(pos1);
					if ((PromoteE != 0 || KOMAPOS2DAN(pos2) >= GetDan()-GetPromote()+1) && CanPromote[AtackE[AtackCountE].GetKoma()]) {
						AtackE[AtackCountE].SetPromote(1);
					}
					else {
						AtackE[AtackCountE].SetPromote(0);
					}
					AtackCountE++;
				}
				else if ((m_Ban[pos2] & SELF) != 0) {
					AtackS[AtackCountS].SetFrom(pos2);
					AtackS[AtackCountS].SetKoma(m_Ban[pos2]);
					AtackS[AtackCountS].SetTo(pos1);
					if ((PromoteS != 0 || KOMAPOS2DAN(pos2) <= GetPromote()) && CanPromote[AtackS[AtackCountS].GetKoma()]) {
						AtackS[AtackCountS].SetPromote(1);
					}
					else {
						AtackS[AtackCountS].SetPromote(0);
					}
					AtackCountS++;
				}
			}
		}
	}
	// 桂馬の利き.
	for (i=DIRECT_SKE_1, move=KIKI_MOVE_SKE_1; i < DIRECT_NUM; i++, move<<=1) {
		if ((m_ControlS[pos1] & move) != 0) {
			pos2 = pos1 - Direct[i];
			AtackS[AtackCountS].SetFrom(pos2);
			AtackS[AtackCountS].SetKoma(m_Ban[pos2]);
			AtackS[AtackCountS].SetTo(pos1);
			if (PromoteS != 0 && CanPromote[AtackS[AtackCountS].GetKoma()]) {
				AtackS[AtackCountS].SetPromote(1);
			}
			else {
				AtackS[AtackCountS].SetPromote(0);
			}
			AtackCountS++;
		}
		if ((m_ControlE[pos1] & move) != 0) {
			pos2 = pos1 - Direct[i];
			AtackE[AtackCountE].SetFrom(pos2);
			AtackE[AtackCountE].SetKoma(m_Ban[pos2]);
			AtackE[AtackCountE].SetTo(pos1);
			if (PromoteE != 0 && CanPromote[AtackE[AtackCountE].GetKoma()]) {
				AtackE[AtackCountE].SetPromote(1);
			}
			else {
				AtackE[AtackCountE].SetPromote(0);
			}
			AtackCountE++;
		}
	}
	// 駒の価値でソート。
	for (i=0; i < AtackCountS-1; i++) {
		int max_id = i;
		int max_val = KomaValue[AtackS[i].GetKoma()];
		for (int j=i+1; j < AtackCountS; j++) {
			int v = KomaValue[AtackS[j].GetKoma()];
			if (v < max_val) {
				max_id = j;
				max_val = v;
			}
			else if (v == max_val) {
				if (KomaValue[AtackS[j].GetKoma()] < KomaValue[AtackS[max_id].GetKoma()]) {
					max_id = j;
				}
			}
		}
		//最大値との交換.
		if (i != max_id) {
			swap(AtackS[i], AtackS[max_id]);
		}
	}
	// 駒の価値でソート。
	for (i=0; i < AtackCountE-1; i++) {
		int max_id = i;
		int max_val = KomaValue[AtackE[i].GetKoma()];
		for (int j = i+1; j < AtackCountE; j++) {
			int v = KomaValue[AtackE[j].GetKoma()];
			if (v > max_val) {
				max_id = j;
				max_val = v;
			}
			else if (v == max_val) {
				if (KomaValue[AtackE[j].GetKoma()] > KomaValue[AtackE[max_id].GetKoma()]) {
					max_id = j;
				}
			}
		}
		//最大値との交換.
		if (i != max_id) {
			swap(AtackE[i], AtackE[max_id]);
		}
	}
	
	bool IsEnemy = ((m_Ban[pos] & ENEMY) != 0);
	bool IsSelf = (!IsEnemy && m_Ban[pos] != EMPTY);
	if (IsEnemy && AtackCountS > 0) {
		int Eval = GetValue();
		Kyokumen now(*this);
		ret = now.EvalMax(AtackS, AtackCountS, AtackE, AtackCountE) - Eval;
	}
	else if (IsSelf && AtackCountE > 0) {
		int Eval = GetValue();
		Kyokumen now(*this);
		ret = Eval - now.EvalMin(AtackS, AtackCountS, AtackE, AtackCountE);
	}
	else {
		ret = 0;
	}
	return ret;
}

int Kyokumen::EvalMin(Te *AtackS, int NumAtackS, Te *AtackE, int NumAtackE)
{
	int v = GetValue();
	if (NumAtackE > 0) {
		int k = 0;
		while (!IsCorrectMove(AtackE[k]) && k < NumAtackE) {
			k++;
		}
		if (k == 0) {
		}
		else if (k < NumAtackE) {
			Te t = AtackE[k];
			for (int i=k; i > 0; i--) {
				AtackE[i] = AtackE[i-1];
			}
			AtackE[0] = t;
		}
		else {
			// 他に手がない＝取れない。
			return v;
		}
		AtackE[0].SetCapture(m_Ban[AtackE[0].GetTo()]);
		Move(ENEMY, AtackE[0], true);
		return min(v, EvalMax(AtackS, NumAtackS, AtackE+1, NumAtackE-1));
	}
	else {
		return v;
	}
}

int Kyokumen::EvalMax(Te *AtackS, int NumAtackS, Te *AtackE, int NumAtackE)
{
	int v = GetValue();
	if (NumAtackS > 0) {
		// 邪魔駒の処理.
		int k = 0;
		while (!IsCorrectMove(AtackS[k]) && k < NumAtackS) {
			k++;
		}
		if (k == 0) {
		}
		else if (k < NumAtackS) {
			Te t = AtackS[k];
			for (int i=k; i > 0; i--) {
				AtackS[i] = AtackS[i-1];
			}
			AtackS[0] = t;
		}
		else {
			// 他に手がない＝取れない。
			return v;
		}
		AtackS[0].SetCapture(m_Ban[AtackS[0].GetTo()]);
		Move(SELF, AtackS[0], true);
		return max(v, EvalMin(AtackS+1, NumAtackS-1, AtackE, NumAtackE));
	}
	else {
		return v;
	}
}

static int kyori(KomaPos p1, KomaPos p2)
{
	return max(abs(KOMAPOS2SUJI(p1)-KOMAPOS2SUJI(p2)), abs(KOMAPOS2DAN(p1)-KOMAPOS2DAN(p2)));
}

bool Kyokumen::IsCorrectMove(Te &te) const
{
	if (m_Ban[te.GetFrom()] == SOU) {
		if (m_ControlE[te.GetTo()] != 0) {
			return false;
		}
		else {
			te.SetCapture(m_Ban[te.GetTo()]);
			return true;
		}
	}
	else if (m_Ban[te.GetFrom()] == EOU) {
		if (m_ControlS[te.GetTo()] != 0) {
			return false;
		}
		else {
			te.SetCapture(m_Ban[te.GetTo()]);
			return true;
		}
	}
	if (m_Ban[te.GetFrom()] == SKE || m_Ban[te.GetFrom()] == EKE) {
		te.SetCapture(m_Ban[te.GetTo()]);
		return true;
	}
	int d = kyori(te.GetFrom(), te.GetTo());
	if (d == 0) {
		return false;
	}
	int dir = (te.GetTo() - te.GetFrom()) / d;
	if (d == 1) {
		te.SetCapture(m_Ban[te.GetTo()]);
		return true;
	}
	// ジャンプなので、途中に邪魔な駒がいないかどうかチェックする.
	for (int i=1, pos=te.GetFrom()+dir; i < d; i++, pos=pos+dir) {
		if (m_Ban[pos] != EMPTY) {
			return false;
		}
	}
	te.SetCapture(m_Ban[te.GetTo()]);
	return true;
}

bool Kyokumen::IsLegalMove(Teban teban, Te &te) const
{
	if ((te.GetKoma() & teban) == 0) {
		return false;
	}
	if (IS_HANDPOS(te.GetFrom())) {
		if (GetHandNum(te.GetKoma()) == 0) {
			return false;
		}
	}
	else {
		if (GetBanInf(te.GetFrom()) != te.GetKoma()) {
			return false;
		}
	}
	if ((GetBanInf(te.GetTo()) & teban) != 0) {
		return false;
	}
	if (IsCorrectMove(te)) {
		Kyokumen kk(*this);
		kk.Move(teban, te, true);
		if (teban == SELF && kk.GetControlE(kk.GetKingS()) != 0) {
			return false;
		}
		if (teban == ENEMY && kk.GetControlS(kk.GetKingE()) != 0) {
			return false;
		}
		return true;
	}
	return false;
}

int Kyokumen::Mate(Teban teban, int maxDepth, Te &te)
{
//	Te teBuf[MAX_TE_IN_KYOKUMEN * MAX_MATE_DEPTH];	// 深さ30程度までなら十分すぎる大きさ.
	Te teBuf[MAX_TE_IN_KYOKUMEN * MAX_DEPTH];	// 深さ30程度までなら十分すぎる大きさ.
	
	TsumeHash::Clear();
	
	TsumeVal *p = TsumeHash::Find(m_KyokumenHashVal, m_HandHashVal, m_Hand+teban);
	if (p != nullptr) {
		if (p->mate == 1) {
			te = p->te;
		}
//#if defined(DEBUG_PRINT)
//	printf("Mate(0x%02x, %d, 0x%02x) return[1] [%d]\n", teban, maxDepth, te.GetKoma(), p->mate);
//#endif // defined(DEBUG_PRINT).
		return p->mate;
	}
	
	int ret = 0;
	for (int i=1; i <= maxDepth; i+=2) {
		ret = CheckMate(teban, 0, i, teBuf, te);
		if (ret != 0) {
			break;
		}
	}
	//  0:不明.
	//  1:詰んだ.
	// -1:不詰み.
//#if defined(DEBUG_PRINT)
//	printf("Mate(0x%02x, %d, 0x%02x) return[2] [%d]\n", teban, maxDepth, te.GetKoma(), ret);
//#endif // defined(DEBUG_PRINT).
	return ret;
}

int Kyokumen::CheckMate(Teban teban, int depth, int depthMax, Te *checks, Te &te)
{
	int teNum = MakeChecks(teban, checks);
	if (teNum == 0) {
		TsumeHash::Add(m_KyokumenHashVal, m_HandHashVal, m_Hand+teban, -1, 0);
//#if defined(DEBUG_PRINT)
//		printf("CheckMate(0x%02x, %d, %d, 0x%02x) return[1] [%d]\n", teban, depth, depthMax, te.GetKoma(), -1);
//#endif // defined(DEBUG_PRINT).
		return -1;	//詰まない.
	}
	
	TsumeVal *p = TsumeHash::DomSearchCheckMate(m_KyokumenHashVal, m_Hand+teban);
	if (p != nullptr) {
		te = p->te;
//#if defined(DEBUG_PRINT)
//		printf("CheckMate(0x%02x, %d, %d, 0x%02x) return[2] [%d]\n", teban, depth, depthMax, te.GetKoma(), 1);
//#endif // defined(DEBUG_PRINT).
		return 1;	// 詰んだ.
	}
	int valmax = -1;
	for (int i=0; i < teNum; i++) {
		Kyokumen kk(*this);
		kk.Move(teban, checks[i], true);
#if defined(DEBUG_PRINT_TSUMI)
if (g_bDebugPrint) {
	printf("CheckMate depth[%d]", depth);
	checks[i].Print();
	printf("\n");
}
#endif // defined(DEBUG_PRINT_TSUMI).
		int val = kk.AntiCheckMate(teban^(SELF|ENEMY), depth+1, depthMax, checks+teNum);
		if (val > valmax) {
			valmax = val;
		}
		if (valmax == 1) {
			te = checks[i];
			break; //詰んだ.
		}
	}
	if (valmax == 1) { //詰んだ.
//		ASSERT(IS_SELFKOMA(te.GetKoma()));
		TsumeHash::Add(m_KyokumenHashVal, m_HandHashVal, m_Hand+teban, 1, te);
	}
	else if (valmax == -1) { //本当に詰まなかった.
		TsumeHash::Add(m_KyokumenHashVal, m_HandHashVal, m_Hand+teban, -1, 0);
	}
//#if defined(DEBUG_PRINT)
//	printf("CheckMate(0x%02x, %d, %d, 0x%02x) return[3] [%d]\n", teban, depth, depthMax, te.GetKoma(), valmax);
//#endif // defined(DEBUG_PRINT).
	return valmax;
}

int Kyokumen::MakeChecks(Teban teban, Te *teBuf, int *pin)
{
	Kyokumen kk(*this);			// 実際に動かしてみる局面.
	const Kiki *selfControl;	// 手番側から見て、自分の利き.
	KomaPos enemyKing;				// 手番側から見て、相手の玉の位置.
	
	if (teban == SELF) {
		selfControl = kk.GetControlSPtr();
		enemyKing = GetKingE();
	}
	else {
		selfControl = kk.GetControlEPtr();
		enemyKing = GetKingS();
	}
	int teNum = MakeLegalMoves(teban, teBuf, pin);
	// 実際に動かしてみて、王手だけを残す。
	int outeNum = 0;
	for (int i=0; i < teNum; i++) {
		kk = *this;
		kk.Move(teban, teBuf[i], true);
		// 相手の玉に利きが付いていれば王手。
		if (selfControl[enemyKing]) {
			teBuf[outeNum++] = teBuf[i];
//		printf("MakeChecks ");
//		teBuf[i].Print();
//		printf("\n");
		}
	}
//#if defined(DEBUG_PRINT)
//	printf("MakeChecks(0x%02x) return [%d]\n", teban, outeNum);
//#endif // defined(DEBUG_PRINT).
	return outeNum;
}

int Kyokumen::AntiCheckMate(Teban teban, int depth, int depthMax, Te *antichecks)
{
	Te te;
	int teNum = MakeLegalMoves(teban, antichecks);
	
	int i = 0;
	int valmin = 1;
	if (teNum == 0) {
//#if defined(DEBUG_PRINT)
//		printf("AntiCheckMate(0x%02x, %d, %d) return[1] [%d]\n", teban, depth, depthMax, 1);
//#endif // defined(DEBUG_PRINT).
		return 1;
	}
	if (depth >= depthMax+1) {
//#if defined(DEBUG_PRINT)
//		printf("AntiCheckMate(0x%02x, %d, %d) return[2] [%d]\n", teban, depth, depthMax, 0);
//#endif // defined(DEBUG_PRINT).
		return 0; //長さの限界詰みは不明.
	}
	
	for (i=0; i < teNum; i++) {
		Kyokumen k(*this);
		k.Move(teban, antichecks[i], true);
#if defined(DEBUG_PRINT_TSUMI)
if (g_bDebugPrint) {
	printf("AntiCheckMate depth[%d]", depth);
	antichecks[i].Print();
	printf("\n");
}
#endif // defined(DEBUG_PRINT_TSUMI).
		int val = k.CheckMate(teban^(SELF|ENEMY), depth+1, depthMax, antichecks+teNum, te);
		if (val < valmin) {
			valmin = val;
		}
		if (valmin == -1) {
			break; // 詰まなかった.
		}
		if (valmin == 0) {
			break; // 詰まなかった.
		}
	}
//#if defined(DEBUG_PRINT)
//	printf("AntiCheckMate(0x%02x, %d, %d) return[3] [%d]\n", teban, depth, depthMax, valmin);
//#endif // defined(DEBUG_PRINT).
	return valmin;
}

void Kyokumen::HashInit()
{
	KomaInf inf;
	for (inf=EMPTY; inf <= ERY; inf++) {
		for (KomaPos pos=SUJIDAN2KOMAPOS(1,1); pos <= SUJIDAN2KOMAPOS(9,9); pos++) {
			m_HashSeed[inf][pos] = (((UINT64)rand())<<49) |
				(((UINT64)rand())<<34) |
				(((UINT64)rand())<<19) |
				(((UINT64)rand())<<4) |
				(((UINT64)rand())&0x07);
		}
	}
	
	for (inf=SFU; inf <= EHI; inf++) {
		for (int maisuu=0; maisuu <= MAX_FU; maisuu++) {
			m_HandHashSeed[inf][maisuu] = (((UINT64)rand())<<49) |
				(((UINT64)rand())<<34) |
				(((UINT64)rand())<<19) |
				(((UINT64)rand())<<4) |
				(((UINT64)rand())&0x07);
		}
	}
}

UINT64 Kyokumen::GetHashSeed(KomaInf inf, KomaPos pos)
{
	if (inf > ERY) {
		return 0;
	}
	if (pos > SUJIDAN2KOMAPOS(9,9)) {
		return 0;
	}
	return m_HashSeed[inf][pos];
}

UINT64 Kyokumen::GetHandHashSeed(KomaInf inf, int num)
{
	if (inf > EHI) {
		return 0;
	}
	if (num > MAX_FU) {
		return 0;
	}
	return m_HandHashSeed[inf][num];
}

UINT64 Kyokumen::GetHashHistory(int teNum)
{
	if (teNum > MAX_TE_HISTORY) {
		return 0;
	}
	return m_HashHistory[teNum];
}

Kiki Kyokumen::GetOuteHistory(int teNum)
{
	if (teNum > MAX_TE_HISTORY) {
		return 0;
	}
	return m_OuteHistory[teNum];
}



