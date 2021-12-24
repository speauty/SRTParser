#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "srt_parser.h"
#include "windows.h"
#include "io.h"

#define SYS_CLS system("cls")
#define STD_PRINT_ENDL(msg) std::cout << msg << std::endl
#define STD_PRINT(msg) std::cout << msg

int hasChinese(const char* str)//返回0：无中文，返回1：有中文
{
	char c;
	while (1)
	{
		c = *str++;
		if (c == 0) break;  //如果到字符串尾则说明该字符串没有中文字符
		if (c & 0x80)        //如果字符高位为1且下一字符高位也是1则有中文字符
			if (*str & 0x80) return 1;
	}
	return 0;
}

void SRTParser::__SetWindow()
{
	SetConsoleTitle(T_SRT_PARSER_NAME);
	system("COLOR 3F");
	system("MODE CON COLS=60 LINES=30");
}

void SRTParser::__RenderTitleAndDesc()
{
	std::cout << std::endl 
		<< std::setw(38) << SRT_PARSER_NAME << std::endl << std::endl
		<< "针对SRT字幕文件的处理, 比如将文本转到指定文件中和交换双语, 以及分割字幕文件等." << std::endl;
}

void SRTParser::__RenderMenu()
{
	std::cout << "主要有以下操作: " << std::endl
		<< std::setw(18) << "1.设置字幕规格" << std::setw(18) << "2.设置存储目录" << std::setw(18) << "3.设置字幕文件" << std::endl
		<< std::setw(18) << "4.交换双语字幕" << std::setw(18) << "5.提取字幕本文" << std::setw(18) << "6.分割字幕文件" << std::endl
		<< std::setw(18) << "7.查看所有设置" << std::setw(18) << "8.刷新当前窗口" << std::setw(18) << "9.退出字幕程序" << std::endl;
}

void SRTParser::__OPCall_OP_SET_SS()
{
	std::string buff;
	unsigned short tmp = 0;
	buff.clear();
	STD_PRINT_ENDL("当前支持字幕规格: 1.单语版  2.双语版");
	STD_PRINT("请选择规格: ");
	std::cin >> buff;
	tmp = atoi(buff.c_str());
	if (tmp == 1) {
		this->conf.subtitleSpec = SLS_MONO;
	} else if (tmp == 2) {
		this->conf.subtitleSpec = SLS_BI;
	} else {
		this->conf.subtitleSpec = SLS_MONO;
		STD_PRINT_ENDL("暂不支持当前设置, 已重置为单语版");
	}
	buff.clear();
	STD_PRINT("当前字幕规格: ");
	buff = this->conf.subtitleSpec == SLS_MONO ? "单语版" : "双语版";
	STD_PRINT_ENDL(buff);
}

void SRTParser::__OPCall_OP_SET_STORAGE_DIR()
{
	std::string buff;
	STD_PRINT("请输入存储路径: ");
	std::cin >> buff;
	if (_access(buff.c_str(), 0) == -1) {
		STD_PRINT_ENDL("该目录不存在, 请自行创建");
	} else if (_access(buff.c_str(), 6) == -1) {
		STD_PRINT_ENDL("权限不足");
	} else {
		this->conf.storageDir = buff;
		STD_PRINT("当前存储路径: ");
		STD_PRINT_ENDL(this->conf.storageDir);
	}
}

void SRTParser::__OPCall_OP_SET_PATH()
{
	std::string buff;
	STD_PRINT("请输入字幕文件路径: ");
	std::cin >> buff;
	if (_access(buff.c_str(), 0) == -1) {
		STD_PRINT_ENDL("该文件不存在");
	} else if (_access(buff.c_str(), 6) == -1) {
		STD_PRINT_ENDL("权限不足");
	} else {
		if (buff.substr(buff.find_last_of(".") + 1, buff.length()) == SRT_FILE_EXT) {
			this->conf.sourceFilePath = buff;
			STD_PRINT("当前字幕文件路径: ");
			STD_PRINT_ENDL(this->conf.sourceFilePath);
		} else {
			STD_PRINT_ENDL("非法文件");
		}
		
	}
}

void SRTParser::__OPCall_OP_S_SWAP()
{
	if (this->conf.sourceFilePath.empty()) {
		STD_PRINT_ENDL("暂无字幕文件");
		return;
	}
	std::string currentStorageDir = this->conf.storageDir;
	if (currentStorageDir.empty()) currentStorageDir = "./";
	std::string newFilePath = currentStorageDir + "subtitle_swap." + SRT_FILE_EXT;
	std::string tmpBuffer; // 字符串缓存
	unsigned int lineNo = 1; // 行号, 可能涉及到重排序, 所以这里做下标记
	std::ifstream fs;
	std::ofstream newFS;
	newFS.open(newFilePath, std::ios::out|std::ios::trunc);
	if (!newFS.is_open()) {
		STD_PRINT_ENDL("输出文件创建异常");
		return;
	}

	SrtTextBlock currentBlock;


	fs.open(this->conf.sourceFilePath.data(), std::ios::in);
	if (!fs.is_open()) {
		STD_PRINT_ENDL("文件打开异常");
		return;
	}

	while (std::getline(fs, tmpBuffer) && !fs.eof()) {
		if (tmpBuffer.empty()) {
			newFS << currentBlock.serialNo << std::endl
				<< currentBlock.timeSlot << std::endl
				<< currentBlock.textL2 << std::endl
				<< currentBlock.textL1 << std::endl
				<< std::endl;
			currentBlock.clear();
		} else {
			if (currentBlock.serialNo.empty()) {
				currentBlock.serialNo = tmpBuffer;
			} else if (currentBlock.timeSlot.empty()) {
				currentBlock.timeSlot = tmpBuffer;
			} else if (currentBlock.textL1.empty()) {
				currentBlock.textL1 = tmpBuffer;
			} else if (currentBlock.textL2.empty()) {
				currentBlock.textL2 = tmpBuffer;
			}
		}
		lineNo++;
	}
	if (!currentBlock.serialNo.empty()) {
		newFS << currentBlock.serialNo << std::endl
			<< currentBlock.timeSlot << std::endl
			<< currentBlock.textL2 << std::endl
			<< currentBlock.textL1 << std::endl
			<< std::endl;
	}
	
	tmpBuffer.clear();
	fs.close();
	newFS.close();
	STD_PRINT("交换完成, ");
	std::cout << "共计: " << lineNo << "行, " << "新文件地址: " << newFilePath << std::endl;
}

void SRTParser::__OPCall_OP_VIEW_CONF()
{
	std::string tmpSpec = (this->conf.subtitleSpec == SLS_INIT) ? "未设置" : (this->conf.subtitleSpec==SLS_MONO?"单语版" : "双语版");
	std::cout << "字幕规格: " << tmpSpec << std::endl
		<< "存储目录: " << this->conf.storageDir << std::endl
		<< "字幕源件: " << this->conf.sourceFilePath << std::endl;
}

void SRTParser::__OPCall_OP_CLS()
{
	this->RenderWindow();
}

void SRTParser::__OPParser()
{
	std::string tmpBuffer;
	bool flagHasChinese = 0;
	do {
		this->state.opCode = 0;
		tmpBuffer.clear();
		STD_PRINT("请输入操作序号(回车键确认): ");
		std::cin >> tmpBuffer;
		flagHasChinese = hasChinese(tmpBuffer.c_str());
		this->state.opCode = atoi(tmpBuffer.c_str());
		std::cout << "解析输入: " << this->state.opCode 
			<< std::setw(22) << "原始输入(保留两位): "
			<< std::setw(2) << tmpBuffer.substr(0, 2)
			<< std::setw(10) << "  当前操作:";
		if ( this->state.opCode > OP_EXIT || flagHasChinese || isalpha(tmpBuffer[0]) ) {
			STD_PRINT_ENDL("不支持该代码");
			continue;
		} else {
			switch (this->state.opCode)
			{
			case OP_SET_SS:
				STD_PRINT_ENDL("设置字幕规格");
				this->__OPCall_OP_SET_SS();
				break;
			case OP_SET_STORAGE_DIR:
				STD_PRINT_ENDL("设置存储目录");
				this->__OPCall_OP_SET_STORAGE_DIR();
				break;
			case OP_SET_PATH:
				STD_PRINT_ENDL("设置字幕文件");
				this->__OPCall_OP_SET_PATH();
				break;
			case OP_S_SWAP:
				STD_PRINT_ENDL("交换双语字幕");
				this->__OPCall_OP_S_SWAP();
				break;
			case OP_S_EXTRACT_TEXT:
				STD_PRINT_ENDL("提取字幕本文");
				break;
			case OP_S_SEP:
				STD_PRINT_ENDL("分割字幕文件");
				break;
			case OP_VIEW_CONF:
				STD_PRINT_ENDL("查看所有设置");
				this->__OPCall_OP_VIEW_CONF();
				break;
			case OP_CLS:
				STD_PRINT_ENDL("刷新当前窗口");
				this->__OPCall_OP_CLS();
				break;
			case OP_EXIT:
				STD_PRINT_ENDL("退出当前程序");
				break;
			default:
				STD_PRINT_ENDL("未实现该操作");
				break;
			}
		}
	} while (this->state.opCode != 9);
	
}

void SRTParser::RenderWindow()
{
	this->__SetWindow();
	SYS_CLS;
	this->__RenderTitleAndDesc();
	this->__RenderMenu();
	this->__OPParser();
}