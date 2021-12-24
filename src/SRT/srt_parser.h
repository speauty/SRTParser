#pragma once
#include <iostream>
#include <tchar.h>


#define SRT_PARSER_NAME "SRT��Ļ������"
#define T_SRT_PARSER_NAME _T("SRT��Ļ������")
#define SRT_FILE_EXT "srt"

/**
 * ��Ļ[subtitle]���
 * SLS_INIT ������
 * SLS_MONO ����[monolingual]��Ļ ��ʼֵ
 * SLS_BI   ˫��[Bilingual]��Ļ
 **/
enum SubtitleSpec {
	SLS_INIT = 0, SLS_MONO = 1, SLS_BI = 2
};

/** ���в���
 * ������Ļ���[OP_SET_SS]  ���ô洢Ŀ¼[OP_SET_STORAGE_DIR]  ������Ļ�ļ�[OP_SET_PATH]
 * ����˫����Ļ[OP_S_SWAP]  ��ȡ��Ļ����[OP_S_EXTRACT_TEXT]  �ָ���Ļ�ļ�[OP_S_SEP]
 * �鿴��������[OP_VIEW_CONF] ˢ�µ�ǰ����[OP_CLS] �˳���Ļ����[OP_EXIT]
 **/
enum OP {
	OP_INIT = 0,
	OP_SET_SS = 1, OP_SET_STORAGE_DIR = 2, OP_SET_PATH = 3,
	OP_S_SWAP = 4, OP_S_EXTRACT_TEXT = 5, OP_S_SEP = 6,
	OP_VIEW_CONF = 7, OP_CLS = 8,  OP_EXIT = 9
};

struct SrtTextBlock
{
	std::string serialNo; // ���
	std::string timeSlot; // ʱ���
	std::string textL1; // ��һ����
	std::string textL2; // �ڶ�����

	void clear()
	{
		this->serialNo = "";
		this->timeSlot = "";
		this->textL1 = "";
		this->textL2 = "";
	}
};

struct Conf
{
	SubtitleSpec subtitleSpec = SLS_MONO; // ��Ļ���
	std::string storageDir; // �洢Ŀ¼
	std::string sourceFilePath; // ��ĻԴ�ļ�
};

struct State
{
	unsigned short opCode = OP_SET_SS; // ��ǰ��������
};

/**
 * srt��Ļ������(ʵ������һ������Ӧ��) 
 **/
class SRTParser
{
private:
	Conf conf;
	State state;
private:
	void __SetWindow();
	void __RenderTitleAndDesc();
	void __RenderMenu();
	void __OPParser();
	void __OPCall_OP_SET_SS();
	void __OPCall_OP_SET_STORAGE_DIR();
	void __OPCall_OP_SET_PATH();
	void __OPCall_OP_S_SWAP();
	void __OPCall_OP_VIEW_CONF();
	void __OPCall_OP_CLS();

public:
	SRTParser() {}
	~SRTParser() {}
	void Test() { std::cout << "SRTParser" << std::endl; }

	void RenderWindow();

};