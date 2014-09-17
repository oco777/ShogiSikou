//
//  Joseki.cpp
//  CustomizedShogi
//
//  Created by 小森久 on 2014/09/06.
//
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cocos2d.h"
#include "Joseki.h"

USING_NS_CC;

void Joseki::Init(char *filenames)
{
	if (m_JosekiSize > 0 && m_JosekiData != nullptr) {
		for (int i=0; i < m_JosekiSize; i++) {
			free(m_JosekiData[i]);
		}
		free(m_JosekiData);
		m_JosekiSize = 0;
		m_JosekiData = nullptr;
	}
	// filenamesは、,区切りとする。
	// JosekiDataは１エントリー５１２バイトとする。
	// JosekiSizeはファイルの長さ/512となる。
	char *filename = filenames;
	char *nextfile = strchr(filenames, ',');
	if (nextfile != NULL) {
		*nextfile = '\0';	// ,を¥0で置き換え.
		nextfile++;		// 次のファイル名の先頭へ.
		m_Child = new Joseki();
		m_Child->Init(nextfile);
	}
	else {
		m_Child = NULL;
	}
	
	auto sharedFileUtils = FileUtils::getInstance();
	std::string fullPath = sharedFileUtils->fullPathForFilename(filename);
	
	FILE *fp = fopen(fullPath.c_str(), "rb");
	m_JosekiSize = 0;
	if (fp != NULL) {
		for (;;) {
			char buf[512];
			if (fread(buf, 1, 512, fp) <= 0) {
				break;
			}
			//			JosekiData[JosekiSize]=(unsigned char*)malloc(512);
			//			memcpy(JosekiData[JosekiSize],buf,512);
			m_JosekiSize++;
		}
		if (m_JosekiSize > 0) {
			m_JosekiData = (unsigned char **)malloc(sizeof(unsigned char *) * m_JosekiSize);
			fseek(fp, 0, SEEK_SET);
			for (int i=0; i < m_JosekiSize; i++) {
				m_JosekiData[i] = (unsigned char*)malloc(512);
				if (fread(m_JosekiData[i], 1, 512, fp) <= 0) {
					break;
				}
			}
		}
		fclose(fp);
	}
}

void Joseki::fromJoseki(const Kyokumen &shoki, Teban shokiTeban, Kyokumen &k, int tesu, int &teNum, Te te[], int hindo[])
{
	teNum = 0;
	int i;
	for (i=0; i < m_JosekiSize; i++) {
		Kyokumen kk(shoki);
		Teban teban = shokiTeban;
		int j;
		for (j=0; j < tesu; j++) {
			if (m_JosekiData[i][j*2] == 0 || m_JosekiData[i][j*2] == 0xff) {
				break;
			}
			Te te = Te(teban, m_JosekiData[i][j*2+1], m_JosekiData[i][j*2], kk);
			kk.Move(teban, te, false);
			if (teban == ENEMY) {
				teban = SELF;
			}
			else {
				teban = ENEMY;
			}
		}
		if (j == tesu && k == kk) {
			Te ret = Te(teban, m_JosekiData[i][j*2+1], m_JosekiData[i][j*2], kk);
			if (m_JosekiData[i][j*2] == 0 || m_JosekiData[i][j*2] == 0xff) {
				continue;
			}
			int l;
			for (l=0; l < teNum; l++) {
				if (ret == te[l]) {
					hindo[l]++;
					break;
				}
			}
			if (l == teNum) {
				te[l] = ret;
				hindo[l] = 1;
				teNum++;
			}
		}
	}
	if (m_Child != NULL && teNum == 0) {
		m_Child->fromJoseki(shoki, shokiTeban, k, tesu, teNum, te, hindo);
	}
}

