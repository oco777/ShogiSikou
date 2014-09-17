//
//  Joseki.h
//  CustomizedShogi
//
//  Created by 小森久 on 2014/09/06.
//
//

#ifndef __CustomizedShogi__Joseki__
#define __CustomizedShogi__Joseki__

#include "Kyokumen.h"

class Joseki {
private:
	unsigned char **m_JosekiData;
	int m_JosekiSize;
	Joseki *m_Child;
	
public:
	Joseki() { m_JosekiData = nullptr; m_JosekiSize = 0; }
	void Init(char *filenames);
	void fromJoseki(const Kyokumen &shoki, Teban shokiTeban, Kyokumen &k, int tesu, int &teNum, Te te[], int hindo[]);
};

#endif /* defined(__CustomizedShogi__Joseki__) */
