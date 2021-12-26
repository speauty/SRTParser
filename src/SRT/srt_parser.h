#pragma once
#include <iostream>
#include <fstream>
#include <array>
#include <tchar.h>

#define SRT_PARSER_NAME "SRT��Ļ������"
#define T_SRT_PARSER_NAME _T("SRT��Ļ������")
#define SRT_FILE_EXT "srt"
#define TMP_FILE_EXT "txt"
#define MENU_ITEMS_COUNT 8

/** ���в���
 * 
 * ��ע: Bilingual[˫��, ���дBI] monolingual[˫��, ��дMONO]
 * 
 * ˢ�µ�ǰ����[OP_REFRESH] �鿴��������[OP_VIEW_ALL_CONF] �˳���Ļ����[OP_EXIT]
 * ���ô洢Ŀ¼[OP_SET_STORAGE_DIR] ת���ļ�����[OP_SET_NEWER_NAME] ������ĻԴ��[OP_SET_SOURCE_PATH]
 * ����˫����Ļ[OP_BI_SWAP] ��ȡ��Ļ����[OP_EXTRACT_TEXT]
 * 
 **/
enum OP {
	OP_INIT = 0,
	OP_REFRESH = 1, OP_VIEW_ALL_CONF = 3, OP_EXIT = 5,
	OP_SET_STORAGE_DIR = 7, OP_SET_NEWER_NAME = 9, OP_SET_SOURCE_PATH = 11,
	OP_BI_SWAP = 13, OP_EXTRACT_TEXT = 15
};

/**
 * �˵���ṹ��
 **/
struct MenuItem
{
	unsigned short idx; /* ����(�����) */
	std::string name; /* ���� */
	OP opCode; /* ��Ӧ��OP���� */
	//OpCallFunc opCall; /* OP��Ӧ����������� */
};

/**
 * ��Ļ�ı���ṹ
 * serialNo ���
 * timeDuration ʱ������
 * textL1 ��һ����
 * textL2 �ڶ�����[���ܴ���]
 * �Կ�������
 * 
 **/
struct SrtTextBlock 
{
	std::string serialNo;
	std::string timeDuration;
	std::string textL1, textL2;

	void clear()
	{
		this->serialNo = "";
		this->timeDuration = "";
		this->textL1 = "";
		this->textL2 = "";
	}
};

struct Conf
{
	std::string storageDir; /* �洢Ŀ¼(δ���þͱ����ڵ�ǰ�ļ���) */
	std::string sourceFilePath; /* ��ĻԴ�ļ� */
	std::ifstream sourceFd; /* ��ĻԴ���� */
	std::string targetFileName; /* ��Ļ���ļ����� */
	std::array<MenuItem, MENU_ITEMS_COUNT> menuMaps; /* �˵� */
	unsigned short realOpCode = OP_INIT; // ��ǰ��������
};

struct State
{
	bool flagCurrentSrtIsOpened; // ��ǰ��ĻԴ�ļ��Ƿ��
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
	void __InitData();
	void __SetWindow();
	void __RenderTitleAndDesc();
	void __RenderMenu();
	void __OPParser();
	void __OPCall_OP_REFRESH();
	void __OPCall_OP_VIEW_ALL_CONF();
	void __OPCall_OP_SET_STORAGE_DIR();
	void __OPCall_OP_SET_NEWER_NAME();
	void __OPCall_OP_SET_SOURCE_PATH();
	void __OPCall_OP_BI_SWAP();
	void __OPCall_OP_EXTRACT_TEXT();
	void __OPCall_OP_EXIT();

public:
	SRTParser();
	~SRTParser();
	void Test() { std::cout << "SRTParser" << std::endl; }

	void RenderWindow();

};