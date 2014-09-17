//
//  ShogiDefs.h
//  MySampleProject
//
//  Created by 小森久 on 2014/06/21.
//
//

#ifndef MySampleProject_ShogiDefs_h
#define MySampleProject_ShogiDefs_h

#define MIN_SUJI	(3)
#define MAX_SUJI	(9)
#define MIN_DAN		(4)
#define MAX_DAN		(9)
#define MIN_PROMOTE	(1)
#define MAX_PROMOTE	(3)

#define MAX_KOMA	(40)
#define MAX_OU		(2)
#define MAX_HI		(2)
#define MAX_KA		(2)
#define MAX_KI		(4)
#define MAX_GI		(4)
#define MAX_KE		(4)
#define MAX_KY		(4)
#define MAX_FU		(18)

#define IS_SELFKOMA(inf)	(((inf)&SELF) != 0)
#define IS_ENEMYKOMA(inf)	(((inf)&ENEMY) != 0)
#define REVERSEOWNER(inf)	(IS_SELFKOMA(inf) ? (((inf)&(~SELF))|ENEMY) : (((inf)&(~ENEMY))|SELF))

#define COL2SUJI(col, max)	((max)-(col)+1)
#define ROW2DAN(row, max)	(row)
#define SUJI2COL(suji, max)	((max)-(suji)+1)
#define DAN2ROW(dan, max)	(dan)
#define REVERSECOL(col, max)	((max)-(col)+1)
#define REVERSEROW(row, max)	((max)-(row)+1)

#define SUJIDAN2KOMAPOS(suji, dan)	((suji)<<4|(dan))
#define COLROW2KOMAPOS(col, colMax, row, rowMax)	SUJIDAN2KOMAPOS(COL2SUJI((col), (colMax)), ROW2DAN((row), (rowMax)))
#define KOMAPOS2SUJI(pos)	(((pos)>>4)&0x0f)
#define KOMAPOS2DAN(pos)	((pos)&0x0f)
#define KOMAPOS2COL(pos, max)	(SUJI2COL(KOMAPOS2SUJI(pos), (max)))
#define KOMAPOS2ROW(pos, max)	(DAN2ROW(KOMAPOS2DAN(pos), (max)))
#define HANDPOS2INF(owner, pos)	((owner)+((pos)&0x07))

#define IS_BANPOS(pos)	((pos) >= 0x11)
#define IS_HANDPOS(pos)	(((pos) > 0) && ((pos) < 0x11))

#define MAX_TE_IN_KYOKUMEN	(600)
#define MAX_TE_HISTORY		(1000)
#define SENNITITE_NUM		(4)

enum {
	REASON_UNKNOWN,
	REASON_TSUMI,
	REASON_SENNITITE,
};

#define HASHTBL_SIZE	(1024*1024)
#define HASHTBL_MASK	(0x0fffff)

#define TSUME_HASH_RETRY_MAX	(9)
#define TSUME_HASH_SIZE			(1024*1024)
#define TSUME_HASH_AND			(TSUME_HASH_SIZE-1)


////////////////////////////
// 思考で使用する.
#define MAX_DEPTH	(32)
#define MIN_SIKOU_DEPTH		(4)
#define MAX_SIKOU_DEPTH		(8)
#define MAX_MATE_DEPTH		(7)
#define INFINITE	(999999)
#define SIKOU_TIMEOUT_SECOND	(30)

////////////////////////////
// デバッグ用.
//#define DEBUG_PRINT
//#define DEBUG_PRINT_TSUMI

#endif
