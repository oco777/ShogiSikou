//
//  Te.cpp
//  MySampleProject
//
//  Created by 小森久 on 2014/06/11.
//
//

#include <stdio.h>
#include "Te.h"
#include "Kyokumen.h"

Te::Te(KomaPos f, KomaPos t, KomaInf k, KomaInf c, int p, int K, int v)
{
	m_From = f;
	m_To = t;
	m_Koma = k;
	m_Capture = c;
	m_Promote = p;
	m_Kind = K;
	m_Value = v;
}

Te::Te(Teban teban, KomaPos f, KomaPos t, const Kyokumen &k)
{
	if (f > 100) {
		m_Koma = (f - 100) + teban;
		m_From = 0;
	}
	else {
        int fd = (f + 8) / 9;
        int fs = (f - 1) % 9 + 1;
		m_From = fs * 16 + fd;
		m_Koma = k.GetBanInf(m_From);
	}
	if (t > 100) {
		t = t - 100;
		m_Promote = 1;
	}
	else {
		m_Promote = 0;
	}
    int td = (t + 8) / 9;
    int ts = (t - 1) % 9 + 1;
	m_To = ts * 16 + td;
	m_Capture = k.GetBanInf(m_To);
	m_Kind = 0;
	m_Value = 0;
}

// 駒の種類を表示するための文字列.
const char *komaStr2[]={
	"  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ",
	"  ","歩","香","桂","銀","金","角","飛","王","と","杏","圭","全","金","馬","龍",
	"  ","歩","香","桂","銀","金","角","飛","王","と","杏","圭","全","金","馬","龍",
	" 壁","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ","  ",
};

void Te::FPrint(FILE *fp)
{
	fprintf(fp, "%02x", m_To);
	fprintf(fp, "%s", komaStr2[m_Koma]);
	if (m_Promote) {
		fprintf(fp, "成");
	}
	if (IS_HANDPOS(m_From)) {
		fprintf(fp, "打　");
	}
	else {
		fprintf(fp, "(%02x)", m_From);
	}
	if (m_Promote == 0) {
		fprintf(fp,"　");
	}
	fprintf(fp, "(");
	if (m_Value > 0) {
		fprintf(fp, "+");
	}
	fprintf(fp, "%d)", m_Value);
}


