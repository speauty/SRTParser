#pragma once
#include <iostream>
#include <fstream>
#include <array>
#include <tchar.h>

#define SRT_PARSER_NAME "SRT字幕解析器"
#define T_SRT_PARSER_NAME _T("SRT字幕解析器")
#define SRT_FILE_EXT "srt"
#define TMP_FILE_EXT "txt"
#define MENU_ITEMS_COUNT 8

/** 所有操作
 * 
 * 备注: Bilingual[双语, 后简写BI] monolingual[双语, 简写MONO]
 * 
 * 刷新当前窗口[OP_REFRESH] 查看所有配置[OP_VIEW_ALL_CONF] 退出字幕程序[OP_EXIT]
 * 设置存储目录[OP_SET_STORAGE_DIR] 转存文件名称[OP_SET_NEWER_NAME] 设置字幕源件[OP_SET_SOURCE_PATH]
 * 交换双语字幕[OP_BI_SWAP] 提取字幕本文[OP_EXTRACT_TEXT]
 * 
 **/
enum OP {
	OP_INIT = 0,
	OP_REFRESH = 1, OP_VIEW_ALL_CONF = 3, OP_EXIT = 5,
	OP_SET_STORAGE_DIR = 7, OP_SET_NEWER_NAME = 9, OP_SET_SOURCE_PATH = 11,
	OP_BI_SWAP = 13, OP_EXTRACT_TEXT = 15
};

/**
 * 菜单项结构体
 **/
struct MenuItem
{
	unsigned short idx; /* 索引(即序号) */
	std::string name; /* 名称 */
	OP opCode; /* 对应的OP代码 */
	//OpCallFunc opCall; /* OP对应操作函数句柄 */
};

/**
 * 字幕文本块结构
 * serialNo 序号
 * timeDuration 时间区间
 * textL1 第一语言
 * textL2 第二语言[可能存在]
 * 以空行区分
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
	std::string storageDir; /* 存储目录(未设置就保存在当前文件夹) */
	std::string sourceFilePath; /* 字幕源文件 */
	std::ifstream sourceFd; /* 字幕源件流 */
	std::string targetFileName; /* 字幕新文件名称 */
	std::array<MenuItem, MENU_ITEMS_COUNT> menuMaps; /* 菜单 */
	unsigned short realOpCode = OP_INIT; // 当前操作代码
};

struct State
{
	bool flagCurrentSrtIsOpened; // 当前字幕源文件是否打开
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