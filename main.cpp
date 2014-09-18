//
//  main.cpp
//  MySampleProject
//
//  Created by 小森久 on 2014/06/11.
//
//

#include "Kyokumen.h"
#include "Sikou.h"

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <fstream.h>

int volatile *ChudanFlag;

KomaInf HirateBan[9][9]={
	{EKY,EKE,EGI,EKI,EOU,EKI,EGI,EKE,EKY},
	{EMP,EHI,EMP,EMP,EMP,EMP,EMP,EKA,EMP},
	{EFU,EFU,EFU,EFU,EFU,EFU,EFU,EFU,EFU},
	{EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP},
	{EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP},
	{EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP,EMP},
	{SFU,SFU,SFU,SFU,SFU,SFU,SFU,SFU,SFU},
	{EMP,SKA,EMP,EMP,EMP,EMP,EMP,SHI,EMP},
	{SKY,SKE,SGI,SKI,SOU,SKI,SGI,SKE,SKY}
};
// こちらは面倒でもEHIまで0を並べないといけません。
int HirateMotigoma[EHI+1]={
// 空空空空空空空空空空空空空空空空空歩香桂銀金角飛王と杏圭全金馬龍空歩香桂銀金角飛
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

Kyokumen shoki(MAX_SUJI, MAX_DAN, MAX_PROMOTE, 0, HirateBan, HirateMotigoma);

extern "C" __declspec(dllexport) void sikou_ini(int* c)
{
	Kyokumen::HashInit();

	ChudanFlag=c;

	AllocConsole();
	SetConsoleTitle("自在将棋");

	FILE *err,*out,*in;
	int NewStdout,NewStdin,NewStderr;

	NewStdout=_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE),_O_TEXT);
	int dup2ret=dup2(1,NewStdout);
	out=freopen("CONOUT$","w",stdout);

	NewStdin=_open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE),_O_TEXT);
	dup2ret=dup2(0,NewStdin);
	in=freopen("CONIN$","r",stdin);

	NewStderr=_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE),_O_TEXT);
	dup2ret=dup2(2,NewStderr);
	err=freopen("CONOUT$","w",stderr);
}

extern "C" __declspec(dllexport) void sikou_end()
{
	FreeConsole();
}

Sikou think;

extern "C" __declspec(dllexport) int sikou(int tesu,unsigned char kifu[][2],int* timer_sec,int* i_moto,int* i_saki,int* i_naru)
{
	// ０手目で、平手の局面で、持ち駒なしから開始しましょう。
	KyokumenKomagumi Hirate(MAX_SUJI, MAX_DAN, MAX_PROMOTE, 0,HirateBan,HirateMotigoma);
	KyokumenKomagumi k(Hirate);
	KyokumenKomagumi::InitKanagomaValue();
	k.Initialize(true);

	Te t;
	int ret;

	int teban;
	for(int i=0;i<tesu;i++) {
		if (i%2==0) {
			teban=SELF;
		} else {
			teban=ENEMY;
		}
		Te te(teban,kifu[i][1],kifu[i][0],k);
		te.Print();
		k.Move(teban, te, true);
	}
	if (tesu%2==0) {
		teban=SELF;
	} else {
		teban=ENEMY;
	}
	k.SenkeiInit();
	t=think.Think(teban, k, shoki);

	if(t.IsNull()) {
		ret=0; //投了
	} else {
		*i_moto = t.CSAmoto();
		*i_saki = t.CSAsaki();
		*i_naru = t.CSApromote();
		printf("\n\n結論：");
		t.Print();
		printf("\n");
		ret=1;
	}
	return ret;
}
