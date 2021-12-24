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

int hasChinese(const char* str)//����0�������ģ�����1��������
{
	char c;
	while (1)
	{
		c = *str++;
		if (c == 0) break;  //������ַ���β��˵�����ַ���û�������ַ�
		if (c & 0x80)        //����ַ���λΪ1����һ�ַ���λҲ��1���������ַ�
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
		<< "���SRT��Ļ�ļ��Ĵ���, ���罫�ı�ת��ָ���ļ��кͽ���˫��, �Լ��ָ���Ļ�ļ���." << std::endl;
}

void SRTParser::__RenderMenu()
{
	std::cout << "��Ҫ�����²���: " << std::endl
		<< std::setw(18) << "1.������Ļ���" << std::setw(18) << "2.���ô洢Ŀ¼" << std::setw(18) << "3.������Ļ�ļ�" << std::endl
		<< std::setw(18) << "4.����˫����Ļ" << std::setw(18) << "5.��ȡ��Ļ����" << std::setw(18) << "6.�ָ���Ļ�ļ�" << std::endl
		<< std::setw(18) << "7.�鿴��������" << std::setw(18) << "8.ˢ�µ�ǰ����" << std::setw(18) << "9.�˳���Ļ����" << std::endl;
}

void SRTParser::__OPCall_OP_SET_SS()
{
	std::string buff;
	unsigned short tmp = 0;
	buff.clear();
	STD_PRINT_ENDL("��ǰ֧����Ļ���: 1.�����  2.˫���");
	STD_PRINT("��ѡ����: ");
	std::cin >> buff;
	tmp = atoi(buff.c_str());
	if (tmp == 1) {
		this->conf.subtitleSpec = SLS_MONO;
	} else if (tmp == 2) {
		this->conf.subtitleSpec = SLS_BI;
	} else {
		this->conf.subtitleSpec = SLS_MONO;
		STD_PRINT_ENDL("�ݲ�֧�ֵ�ǰ����, ������Ϊ�����");
	}
	buff.clear();
	STD_PRINT("��ǰ��Ļ���: ");
	buff = this->conf.subtitleSpec == SLS_MONO ? "�����" : "˫���";
	STD_PRINT_ENDL(buff);
}

void SRTParser::__OPCall_OP_SET_STORAGE_DIR()
{
	std::string buff;
	STD_PRINT("������洢·��: ");
	std::cin >> buff;
	if (_access(buff.c_str(), 0) == -1) {
		STD_PRINT_ENDL("��Ŀ¼������, �����д���");
	} else if (_access(buff.c_str(), 6) == -1) {
		STD_PRINT_ENDL("Ȩ�޲���");
	} else {
		this->conf.storageDir = buff;
		STD_PRINT("��ǰ�洢·��: ");
		STD_PRINT_ENDL(this->conf.storageDir);
	}
}

void SRTParser::__OPCall_OP_SET_PATH()
{
	std::string buff;
	STD_PRINT("��������Ļ�ļ�·��: ");
	std::cin >> buff;
	if (_access(buff.c_str(), 0) == -1) {
		STD_PRINT_ENDL("���ļ�������");
	} else if (_access(buff.c_str(), 6) == -1) {
		STD_PRINT_ENDL("Ȩ�޲���");
	} else {
		if (buff.substr(buff.find_last_of(".") + 1, buff.length()) == SRT_FILE_EXT) {
			this->conf.sourceFilePath = buff;
			STD_PRINT("��ǰ��Ļ�ļ�·��: ");
			STD_PRINT_ENDL(this->conf.sourceFilePath);
		} else {
			STD_PRINT_ENDL("�Ƿ��ļ�");
		}
		
	}
}

void SRTParser::__OPCall_OP_S_SWAP()
{
	if (this->conf.sourceFilePath.empty()) {
		STD_PRINT_ENDL("������Ļ�ļ�");
		return;
	}
	std::string currentStorageDir = this->conf.storageDir;
	if (currentStorageDir.empty()) currentStorageDir = "./";
	std::string newFilePath = currentStorageDir + "subtitle_swap." + SRT_FILE_EXT;
	std::string tmpBuffer; // �ַ�������
	unsigned int lineNo = 1; // �к�, �����漰��������, �����������±��
	std::ifstream fs;
	std::ofstream newFS;
	newFS.open(newFilePath, std::ios::out|std::ios::trunc);
	if (!newFS.is_open()) {
		STD_PRINT_ENDL("����ļ������쳣");
		return;
	}

	SrtTextBlock currentBlock;


	fs.open(this->conf.sourceFilePath.data(), std::ios::in);
	if (!fs.is_open()) {
		STD_PRINT_ENDL("�ļ����쳣");
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
	STD_PRINT("�������, ");
	std::cout << "����: " << lineNo << "��, " << "���ļ���ַ: " << newFilePath << std::endl;
}

void SRTParser::__OPCall_OP_VIEW_CONF()
{
	std::string tmpSpec = (this->conf.subtitleSpec == SLS_INIT) ? "δ����" : (this->conf.subtitleSpec==SLS_MONO?"�����" : "˫���");
	std::cout << "��Ļ���: " << tmpSpec << std::endl
		<< "�洢Ŀ¼: " << this->conf.storageDir << std::endl
		<< "��ĻԴ��: " << this->conf.sourceFilePath << std::endl;
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
		STD_PRINT("������������(�س���ȷ��): ");
		std::cin >> tmpBuffer;
		flagHasChinese = hasChinese(tmpBuffer.c_str());
		this->state.opCode = atoi(tmpBuffer.c_str());
		std::cout << "��������: " << this->state.opCode 
			<< std::setw(22) << "ԭʼ����(������λ): "
			<< std::setw(2) << tmpBuffer.substr(0, 2)
			<< std::setw(10) << "  ��ǰ����:";
		if ( this->state.opCode > OP_EXIT || flagHasChinese || isalpha(tmpBuffer[0]) ) {
			STD_PRINT_ENDL("��֧�ָô���");
			continue;
		} else {
			switch (this->state.opCode)
			{
			case OP_SET_SS:
				STD_PRINT_ENDL("������Ļ���");
				this->__OPCall_OP_SET_SS();
				break;
			case OP_SET_STORAGE_DIR:
				STD_PRINT_ENDL("���ô洢Ŀ¼");
				this->__OPCall_OP_SET_STORAGE_DIR();
				break;
			case OP_SET_PATH:
				STD_PRINT_ENDL("������Ļ�ļ�");
				this->__OPCall_OP_SET_PATH();
				break;
			case OP_S_SWAP:
				STD_PRINT_ENDL("����˫����Ļ");
				this->__OPCall_OP_S_SWAP();
				break;
			case OP_S_EXTRACT_TEXT:
				STD_PRINT_ENDL("��ȡ��Ļ����");
				break;
			case OP_S_SEP:
				STD_PRINT_ENDL("�ָ���Ļ�ļ�");
				break;
			case OP_VIEW_CONF:
				STD_PRINT_ENDL("�鿴��������");
				this->__OPCall_OP_VIEW_CONF();
				break;
			case OP_CLS:
				STD_PRINT_ENDL("ˢ�µ�ǰ����");
				this->__OPCall_OP_CLS();
				break;
			case OP_EXIT:
				STD_PRINT_ENDL("�˳���ǰ����");
				break;
			default:
				STD_PRINT_ENDL("δʵ�ָò���");
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