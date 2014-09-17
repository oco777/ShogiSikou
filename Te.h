//
//  Te.h
//  MySampleProject
//
//  Created by 小森久 on 2014/06/11.
//
//

#ifndef __MySampleProject__Te__
#define __MySampleProject__Te__

#include <stdio.h>
#include "TypeDefs.h"
#include "KomaInf.h"

class Kyokumen;

class Te {
private:
	KomaPos m_From;
	KomaPos m_To;
	KomaInf m_Koma;
	KomaInf m_Capture;
	UINT8 m_Promote;
	UINT8 m_Kind;
	INT16 m_Value;
	
public:
	Te() {}
	Te(int i) { m_From = 0; m_To = 0; m_Koma = 0; m_Capture = 0; m_Promote = 0; m_Kind = 0; m_Value = 0; }
	Te(KomaPos f, KomaPos t, KomaInf k, KomaInf c, int p=0, int K=0, int v=0);
	Te(Teban teban, KomaPos f, KomaPos t, const Kyokumen &k);
	int operator==(const Te &a) { return (a.GetFrom() == m_From && a.GetTo() == m_To && a.GetKoma() == m_Koma && a.GetPromote() == m_Promote); }
	
	inline bool IsNull() const { return (m_From == 0 && m_To == 0); }
	
	inline KomaPos GetFrom() const			{ return m_From; }
	void SetFrom(KomaPos pos)		{ m_From = pos; }
	inline KomaPos GetTo() const			{ return m_To; }
	void SetTo(KomaPos pos)			{ m_To = pos; }
	inline KomaInf GetKoma() const			{ return m_Koma; }
	void SetKoma(KomaInf inf)		{ m_Koma = inf; }
	inline KomaInf GetCapture() const		{ return m_Capture; }
	void SetCapture(KomaInf inf)	{ m_Capture = inf; }
	inline UINT8 GetPromote() const		{ return m_Promote; }
	void SetPromote(UINT8 promote)	{ m_Promote = promote; }
	inline UINT8 GetKind() const			{ return m_Kind; }
	void SetKind(UINT8 kind)		{ m_Kind = kind; }
	inline INT16 GetValue() const			{ return m_Value; }
	void SetValue(INT16 value)		{ m_Value = value; }
	
	void Print() { FPrint(stdout); }
	void FPrint(FILE *fp);
};

#endif /* defined(__MySampleProject__Te__) */
