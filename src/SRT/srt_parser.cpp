#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <array>
#include <algorithm>
#include "srt_parser.h"
#include "windows.h"
#include "io.h"

#define SYS_CLS system("cls")
#define STD_PRINT_ENDL(msg) std::cout << msg << std::endl
#define STD_PRINT(msg) std::cout << msg
#define STD_PRINT_END() std::cout << std::endl

static int hasChinese(const char* str)
{
	char c;
	while (1) {
		c = *str++;
		if (c == 0) break;  //������ַ���β��˵�����ַ���û�������ַ�
		if (c & 0x80)        //����ַ���λΪ1����һ�ַ���λҲ��1���������ַ�
			if (*str & 0x80) return 1;
	}
	return 0;
}

static bool menuArrCmp(MenuItem a, MenuItem b)
{
	return a.idx < b.idx;
}

SRTParser::SRTParser()
{
	this->__InitData();
}

SRTParser::~SRTParser()
{
	if (this->state.flagCurrentSrtIsOpened || this->conf.sourceFd) {
		this->conf.sourceFd.close();
		this->state.flagCurrentSrtIsOpened = false;
	}
}

void SRTParser::__InitData()
{
	/* ��ʼ���洢Ŀ¼ */
	this->conf.storageDir = "./";
	/* ��ʼ���˵� */
	this->conf.menuMaps = { {
	{1, "ˢ�µ�ǰ����", OP_REFRESH}, {2, "�鿴��������", OP_VIEW_ALL_CONF}, {3, "�˳���Ļ����", OP_EXIT},
	{4, "���ô洢Ŀ¼", OP_SET_STORAGE_DIR}, {5, "ת���ļ�����", OP_SET_NEWER_NAME}, {6, "������ĻԴ��", OP_SET_SOURCE_PATH},
	{7, "����˫����Ļ", OP_BI_SWAP}, {8, "��ȡ��Ļ����", OP_EXTRACT_TEXT}
} };
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
	std::cout << "��Ҫ�����²���: " << std::endl;

	this->conf.menuMaps;

	std::sort(this->conf.menuMaps.begin(), this->conf.menuMaps.end(), menuArrCmp);

	for (unsigned short i = 0; i < MENU_ITEMS_COUNT; i++)
	{
		char tmpCurrentMenuName[20];
		sprintf_s(tmpCurrentMenuName, "%d.%s", this->conf.menuMaps[i].idx, this->conf.menuMaps[i].name.c_str());
		std::cout << std::setw(18) << tmpCurrentMenuName;
		if ((i % 3) == 2 || i == (MENU_ITEMS_COUNT - 1)) STD_PRINT_END();
	}
}

void SRTParser::__OPCall_OP_REFRESH()
{
	this->RenderWindow();
}

void SRTParser::__OPCall_OP_VIEW_ALL_CONF()
{
	std::cout << "�洢Ŀ¼: " << this->conf.storageDir << std::endl
		<< "��ĻԴ��: " << this->conf.sourceFilePath << std::endl
		<< "��Ļ�¼�: " << this->conf.targetFileName << std::endl;
}

void SRTParser::__OPCall_OP_SET_STORAGE_DIR()
{
	std::string buff;
	if (!this->conf.storageDir.empty()) {
		std::cout << "��ǰ�洢·��: " << this->conf.storageDir << std::endl;
	}
	STD_PRINT("������洢·��: ");
	std::cin >> buff;
	if (_access(buff.c_str(), 0) == -1) {
		STD_PRINT_ENDL("��Ŀ¼������, �����д���");
	} else if (_access(buff.c_str(), 6) == -1) {
		STD_PRINT_ENDL("Ȩ�޲���");
	} else {
		this->conf.storageDir = buff;
		if (!this->conf.targetFileName.empty()) {
			std::string path = this->conf.storageDir + this->conf.targetFileName;
			if (!_access(path.c_str(), 0)) {
				std::cout << "��Ӧת���ļ�: " << path << " �Ѵ���, �뼰ʱ���ݱ���, ���ⱻɾ���򸲸�" << std::endl;
			} else {
				std::ofstream newFS;
				newFS.open(path, std::ios::out);
				if (!newFS.is_open()) {
					std::cout << "��Ӧת���ļ��ϳ�ʧ��: " << path << std::endl;
					newFS.close();
					return;
				}
				newFS.close();
				remove(path.c_str());
			}
		}
		STD_PRINT("��ǰ�洢·��: ");
		STD_PRINT_ENDL(this->conf.storageDir);
	}
}

void SRTParser::__OPCall_OP_SET_NEWER_NAME()
{
	std::string buff;
	std::string path;
	if (!this->conf.targetFileName.empty()) {
		std::cout << "��ǰת���ļ�: " << this->conf.targetFileName << std::endl;
	}
	STD_PRINT("������ת���ļ�����: ");
	std::cin >> buff;
	buff += ".";
	buff += SRT_FILE_EXT;
	if (!this->conf.storageDir.empty()) {
		path = this->conf.storageDir + buff;
		if (!_access(path.c_str(), 0)) {
			std::cout << "��ǰ�ļ�: " << path << " �Ѵ���, �뼰ʱ���ݱ���, ���ⱻɾ���򸲸�" << std::endl;
		} else {
			std::ofstream newFS;
			newFS.open(path, std::ios::out);
			if (!newFS.is_open()) {
				std::cout << "��ǰת���ļ��ϳ�ʧ��: " << path << std::endl;
				newFS.close();
				return;
			}
			newFS.close();
			remove(path.c_str());
		}
	}
	this->conf.targetFileName = buff;
	STD_PRINT("��ǰת���ļ�: ");
	STD_PRINT_ENDL(this->conf.targetFileName);
}

void SRTParser::__OPCall_OP_SET_SOURCE_PATH()
{
	std::string buff;
	if (!this->conf.sourceFilePath.empty()) {
		std::cout << "��ǰ��ĻԴ��: " << this->conf.sourceFilePath << std::endl;
	}
	STD_PRINT("��������ĻԴ��·��: ");
	std::cin >> buff;
	if (buff.substr(buff.find_last_of(".") + 1, buff.length()) != SRT_FILE_EXT) {
		STD_PRINT_ENDL("�Ǳ�׼SRT��Ļ�ļ�, �ܾ�����");
		return;
	}

	if (!this->conf.sourceFilePath.empty() && buff == this->conf.sourceFilePath) {
		STD_PRINT_ENDL("��ǰ��ĻԴ��������");
		return;
	}
	std::ifstream fs;
	fs.open(buff, std::ios::in);
	if (!fs.is_open()) {
		std::cout << "��ǰ��ĻԴ��: " << buff << " �޷�����, �����ļ��Ƿ���ڻ���Ч" << std::endl;
	} else {
		if (!this->conf.sourceFilePath.empty() && this->conf.sourceFd.is_open()) {
			this->conf.sourceFd.close();
		}
		if (this->state.flagCurrentSrtIsOpened || this->conf.sourceFd) {
			this->conf.sourceFd.close();
			this->state.flagCurrentSrtIsOpened = false;
		}
	}
	fs.close();
	this->conf.sourceFilePath = buff;
	this->conf.sourceFd.open(this->conf.sourceFilePath.data(), std::ios::in);
	this->state.flagCurrentSrtIsOpened = true;
	STD_PRINT("��ǰ��ĻԴ��: ");
	STD_PRINT_ENDL(this->conf.sourceFilePath);
}

void SRTParser::__OPCall_OP_BI_SWAP()
{
	unsigned long long begin = GetTickCount64();
	if (this->conf.sourceFilePath.empty()) {
		STD_PRINT_ENDL("������Ļ�ļ�");
		return;
	}
	std::string newerTmpPath, buffer;
	if (!this->conf.storageDir.empty()) newerTmpPath = this->conf.storageDir;
	if (!this->conf.targetFileName.empty()) {
		newerTmpPath += this->conf.targetFileName;
	}
	else {
		buffer = this->conf.sourceFilePath;
		if (buffer.find("/") != buffer.npos) {
			newerTmpPath += buffer.substr(buffer.find_last_of("/") + 1, buffer.length());
		}
		else {
			newerTmpPath += buffer.substr(buffer.find_last_of("\\") + 1, buffer.length());
		}

		buffer.clear();
	}
	newerTmpPath += ".";
	newerTmpPath += SRT_FILE_EXT;
	std::string tmpBuffer; // �ַ�������
	unsigned int lineNo = 1; // �к�, �����漰��������, �����������±��
	std::ofstream newFS;
	newFS.open(newerTmpPath, std::ios::out|std::ios::trunc);
	if (!newFS.is_open()) {
		std::cout << "��ʱ�ļ�:" << newerTmpPath << " �ϳ�ʧ��" << std::endl;
		return;
	}

	SrtTextBlock currentBlock;
	unsigned int blockCount = 0;

	if (!this->state.flagCurrentSrtIsOpened) {
		this->conf.sourceFd.open(this->conf.sourceFilePath);
		if (!this->conf.sourceFd.is_open()) {
			std::cout << "��ǰ��ĻԴ��: " << this->conf.sourceFilePath << " �޷�����, �����ļ��Ƿ���ڻ���Ч" << std::endl;
			this->conf.sourceFd.close();
			remove(newerTmpPath.c_str());
			newFS.close();
			return;
		}
		this->state.flagCurrentSrtIsOpened = true;
	}
	this->conf.sourceFd.clear();
	this->conf.sourceFd.seekg(0, std::ios::beg);
	std::string errMsg = "";
	while (std::getline(this->conf.sourceFd, tmpBuffer) && !this->conf.sourceFd.eof()) {
		if (tmpBuffer.empty()) {
			if (currentBlock.textL2.empty()) {
				errMsg = "�ڶ����Բ�����";
				break;
			}
			newFS << currentBlock.serialNo << std::endl
				<< currentBlock.timeDuration << std::endl
				<< currentBlock.textL2 << std::endl
				<< currentBlock.textL1 << std::endl
				<< std::endl;
			blockCount++;
			currentBlock.clear();
		} else {
			if (currentBlock.serialNo.empty()) {
				currentBlock.serialNo = tmpBuffer;
			} else if (currentBlock.timeDuration.empty()) {
				currentBlock.timeDuration = tmpBuffer;
			} else if (currentBlock.textL1.empty()) {
				currentBlock.textL1 = tmpBuffer;
			} else if (currentBlock.textL2.empty()) {
				currentBlock.textL2 = tmpBuffer;
			}
		}
		lineNo++;
	}
	if (errMsg.empty() && !currentBlock.serialNo.empty()) {
		newFS << currentBlock.serialNo << std::endl
			<< currentBlock.timeDuration << std::endl
			<< currentBlock.textL2 << std::endl
			<< currentBlock.textL1 << std::endl
			<< std::endl;
		blockCount++;
	}
	tmpBuffer.clear();
	newFS.close();
	if (errMsg.empty()) {
		std::cout << "˫����Ļ�����ɹ�" << std::endl
			<< "��ʱ�ļ�: " << newerTmpPath << std::endl
			<< "д����ĸ��: " << blockCount << ", д���ı���: " << lineNo << std::endl;
	} else {
		std::cout << "˫����Ļ����ʧ��" << std::endl
			<< "����ԭ��: " << errMsg << std::endl
			<< "��ʱ�ļ�: " << newerTmpPath << std::endl
			<< "д����ĸ��: " << blockCount << ", д���ı���: " << lineNo << std::endl;
	}
	std::wcout << "��ʱ(ms): " << (GetTickCount64() - begin) << std::endl;
}

void SRTParser::__OPCall_OP_EXIT()
{
	this->conf.realOpCode = OP_EXIT;
}

void SRTParser::__OPCall_OP_EXTRACT_TEXT()
{
	unsigned long long begin = GetTickCount64();
	if (this->conf.sourceFilePath.empty()) {
		STD_PRINT_ENDL("��δ������ĻԴ��, �ܾ�����");
	}
	std::string newerTmpPath, buffer;
	if (!this->conf.storageDir.empty()) newerTmpPath = this->conf.storageDir;
	if (!this->conf.targetFileName.empty()) {
		newerTmpPath += this->conf.targetFileName;
	} else {
		buffer = this->conf.sourceFilePath;
		if (buffer.find("/") != buffer.npos) {
			newerTmpPath += buffer.substr(buffer.find_last_of("/") + 1, buffer.length());
		} else {
			newerTmpPath += buffer.substr(buffer.find_last_of("\\") + 1, buffer.length());
		}
		
		buffer.clear();
	}
	newerTmpPath += ".";
	newerTmpPath += TMP_FILE_EXT;
	std::ofstream newerTmpFd;
	newerTmpFd.open(newerTmpPath, std::ios::out | std::ios::trunc);
	if (!newerTmpFd.is_open()) {
		std::cout << "��ʱ�ļ�:" << newerTmpPath  << " �ϳ�ʧ��" << std::endl;
		return;
	}


	if (!this->state.flagCurrentSrtIsOpened) {
		this->conf.sourceFd.open(this->conf.sourceFilePath);
		if (!this->conf.sourceFd.is_open()) {
			std::cout << "��ǰ��ĻԴ��: " << this->conf.sourceFilePath << " �޷�����, �����ļ��Ƿ���ڻ���Ч" << std::endl;
			this->conf.sourceFd.close();
			remove(newerTmpPath.c_str());
			newerTmpFd.close();
			return;
		}
		this->state.flagCurrentSrtIsOpened = true;
	}
	buffer.clear();
	SrtTextBlock currentBlock;
	unsigned int blockCount = 0;
	unsigned int insertLineCount = 0;
	this->conf.sourceFd.clear();
	this->conf.sourceFd.seekg(0, std::ios::beg); // �ص��ļ�ͷ
	while (std::getline(this->conf.sourceFd, buffer) && !this->conf.sourceFd.eof()) {
		if (buffer.empty()) {
			blockCount++;
			if (!currentBlock.textL1.empty()) {
				newerTmpFd << currentBlock.textL1 << std::endl;
				insertLineCount++;
			}
			if (!currentBlock.textL2.empty()) {
				newerTmpFd << currentBlock.textL2 << std::endl;
				insertLineCount++;
			}
			newerTmpFd << std::endl;
			currentBlock.clear();
		}
		else {
			if (currentBlock.serialNo.empty()) {
				currentBlock.serialNo = buffer;
			}
			else if (currentBlock.timeDuration.empty()) {
				currentBlock.timeDuration = buffer;
			}
			else if (currentBlock.textL1.empty()) {
				currentBlock.textL1 = buffer;
			}
			else if (currentBlock.textL2.empty()) {
				currentBlock.textL2 = buffer;
			}
		}
	}
	if (!currentBlock.textL1.empty()) {
		blockCount++;
		insertLineCount++;
		newerTmpFd << currentBlock.textL1 << std::endl;
	}
	if (!currentBlock.textL2.empty()) {
		insertLineCount++;
		newerTmpFd << currentBlock.textL2 << std::endl;
	}
	newerTmpFd << std::endl;
	newerTmpFd.close();

	std::cout << "��Ļ�ı���ȡ���" << std::endl
		<< "ת���ļ�: " << newerTmpPath << std::endl
		<<" ������Ļ��: " << blockCount  << ", д���ı���: " << insertLineCount << ", ��ʱ(ms): " << (GetTickCount64() - begin) << std::endl;
}

void SRTParser::__OPParser()
{
	std::string tmpBuffer;
	unsigned short tmpMenuNo;
	bool flagHasChinese = 0;
	do {
		this->conf.realOpCode = OP_INIT;
		tmpBuffer.clear();
		STD_PRINT("������������(�س���ȷ��): ");
		std::cin >> tmpBuffer;
		flagHasChinese = hasChinese(tmpBuffer.c_str());
		tmpMenuNo = atoi(tmpBuffer.c_str());
		std::cout << "��������: " << tmpMenuNo
			<< std::setw(22) << "ԭʼ����(������λ): "
			<< std::setw(2) << tmpBuffer.substr(0, 2)
			<< std::setw(10) << "  ��ǰ����:";
		if (tmpMenuNo > this->conf.menuMaps.size() || flagHasChinese || isalpha(tmpBuffer[0])) {
			STD_PRINT_ENDL("��֧�ָô���");
			continue;
		} else {
			MenuItem currentMenu = this->conf.menuMaps.at(tmpMenuNo - 1);
			this->conf.realOpCode = currentMenu.opCode;
			STD_PRINT_ENDL(currentMenu.name);
			switch (this->conf.realOpCode)
			{
			case OP_REFRESH:
				this->__OPCall_OP_REFRESH(); break;
			case OP_VIEW_ALL_CONF:
				this->__OPCall_OP_VIEW_ALL_CONF(); break;
			case OP_EXIT:
				this->__OPCall_OP_EXIT(); break;
			case OP_SET_STORAGE_DIR:
				this->__OPCall_OP_SET_STORAGE_DIR(); break;
			case OP_SET_NEWER_NAME:
				this->__OPCall_OP_SET_NEWER_NAME(); break;
			case OP_SET_SOURCE_PATH:
				this->__OPCall_OP_SET_SOURCE_PATH(); break;
			case OP_BI_SWAP:
				this->__OPCall_OP_BI_SWAP(); break;
			case OP_EXTRACT_TEXT:
				this->__OPCall_OP_EXTRACT_TEXT(); break;
			default:
				STD_PRINT_ENDL("δʵ�ָò���"); break;
			}
		}
	} while (this->conf.realOpCode != OP_EXIT);
	
}

void SRTParser::RenderWindow()
{
	this->__SetWindow();
	SYS_CLS;
	this->__RenderTitleAndDesc();
	this->__RenderMenu();
	this->__OPParser();
}