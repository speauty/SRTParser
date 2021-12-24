#pragma once
#include <iostream>
#include <tchar.h>


#define SRT_PARSER_NAME "SRT字幕解析器"
#define T_SRT_PARSER_NAME _T("SRT字幕解析器")
#define SRT_FILE_EXT "srt"

/**
 * 字幕[subtitle]规格
 * SLS_INIT 待设置
 * SLS_MONO 单语[monolingual]字幕 初始值
 * SLS_BI   双语[Bilingual]字幕
 **/
enum SubtitleSpec {
	SLS_INIT = 0, SLS_MONO = 1, SLS_BI = 2
};

/** 所有操作
 * 设置字幕规格[OP_SET_SS]  设置存储目录[OP_SET_STORAGE_DIR]  设置字幕文件[OP_SET_PATH]
 * 交换双语字幕[OP_S_SWAP]  提取字幕本文[OP_S_EXTRACT_TEXT]  分割字幕文件[OP_S_SEP]
 * 查看所有设置[OP_VIEW_CONF] 刷新当前窗口[OP_CLS] 退出字幕程序[OP_EXIT]
 **/
enum OP {
	OP_INIT = 0,
	OP_SET_SS = 1, OP_SET_STORAGE_DIR = 2, OP_SET_PATH = 3,
	OP_S_SWAP = 4, OP_S_EXTRACT_TEXT = 5, OP_S_SEP = 6,
	OP_VIEW_CONF = 7, OP_CLS = 8,  OP_EXIT = 9
};

struct SrtTextBlock
{
	std::string serialNo; // 序号
	std::string timeSlot; // 时间段
	std::string textL1; // 第一语言
	std::string textL2; // 第二语言

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
	SubtitleSpec subtitleSpec = SLS_MONO; // 字幕规格
	std::string storageDir; // 存储目录
	std::string sourceFilePath; // 字幕源文件
};

struct State
{
	unsigned short opCode = OP_SET_SS; // 当前操作代码
};

/**
 * srt字幕解析类(实际上是一个窗口应用) 
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