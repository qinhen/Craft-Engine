#pragma once
#ifndef CRAFT_ENGINE_WIN32_API_H_
#define CRAFT_ENGINE_WIN32_API_H_


#include "../../core/Core.h"

#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <winsock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <process.h>
#include <tchar.h>

#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "Ws2_32.lib")

#ifdef max
#undef max
#endif // max

#ifdef min
#undef min
#endif // min

namespace CraftEngine
{

	namespace win32
	{



		bool copyTextToClipboard(const std::wstring& text)
		{
			int i = 0, j = 0;
			for (; i < 6; i++)
			{
				//�򿪼��а�
				j = OpenClipboard(NULL);
				//�ж��Ƿ�򿪳ɹ��������ʧ�������³���5��
				if (j == 0 && i < 5)
					Sleep(60);
				if (j == 0 && i == 5)
					return false;
				if (j != 0)
				{
					i = 0;
					j = 0;
					break;
				}
			}
			//�����ֽ�����HGLOBAL��ʾһ���ڴ��
			size_t mem_size = (text.size() + 1) * sizeof(wchar_t);
			HGLOBAL hmem = GlobalAlloc(GHND, mem_size);
			//�����ڴ���ָ�����ڴ�飬������һ����ֵַ����ָ���ڴ�����ʼ��
			void* pmem = (void*)GlobalLock(hmem);
			//��ռ��а岢�ͷż��а������ݵľ��
			EmptyClipboard();
			memcpy(pmem, text.data(), mem_size);
			SetClipboardData(CF_TEXT, hmem);        //д������
			CloseClipboard();                       //�رռ��а�
			GlobalFree(hmem);                       //�ͷ��ڴ��
			return true;     //���سɹ�
		}

		std::wstring copyTextFromClipboard()
		{
			if (!OpenClipboard(NULL))
				return std::wstring();
			if (!IsClipboardFormatAvailable(CF_TEXT))
				return std::wstring();
			HGLOBAL hg = GetClipboardData(CF_TEXT);
			//�����ڴ��
			LPWSTR wstr = (LPWSTR)GlobalLock(hg);
			std::wstring text(wstr);
			GlobalUnlock(hg);
			CloseClipboard();
			return text;
		}


		struct Folder
		{
			std::wstring name;
			std::wstring full_path;
			uint64_t size;
		};



		//core::GeneralTree<Folder> getDirectory(std::wstring path)
		//{
		//	core::GeneralTree<Folder> tree;
		//	auto root = new Folder();
		//	tree.getRoot().setData(root);


		//	std::function<void(std::wstring, core::GeneralTree<Folder>::Iterator)> func;
		//	func = [&](std::wstring path, core::GeneralTree<Folder>::Iterator it)->void
		//	{
		//		WIN32_FIND_DATAW FindFileData;
		//		HANDLE hListFile;
		//		WCHAR szFilePath[MAX_PATH];

		//		// ���������Ŀ¼���ļ���·�����ַ�����ʹ��ͨ���"*"
		//		lstrcpyW(szFilePath, path.c_str());
		//		// ע�͵Ĵ���������ڲ��������ԡ�.txt����β���ļ�
		//		// lstrcat(szFilePath, "\\*.txt");
		//		lstrcatW(szFilePath, L"\\*");

		//		// ���ҵ�һ���ļ�/Ŀ¼����ò��Ҿ��
		//		hListFile = FindFirstFileW(szFilePath, &FindFileData);
		//		// �жϾ��
		//		if (hListFile == INVALID_HANDLE_VALUE)
		//		{
		//			printf("����%d\n", GetLastError());
		//		}
		//		else
		//		{
		//			do
		//			{
		//				// ���������ʾ������Ŀ¼���ϼ�Ŀ¼�ġ�.���͡�..����
		//				// ����ʹ��ע�Ͳ��ֵĴ������
		//				if (lstrcmpW(FindFileData.cFileName, (L".")) == 0 ||
		//					lstrcmpW(FindFileData.cFileName, (L"..")) == 0)
		//				{
		//					continue;
		//				}

		//				Folder* f = new Folder;
		//				f->name = FindFileData.cFileName;
		//				f->full_path = path + L"\\" + f->name;
		//				f->size = int64_t(int64_t(FindFileData.nFileSizeHigh) * (MAXDWORD + 1)) + FindFileData.nFileSizeLow;

		//				it.addChild(f);
		//				//it.getChild(it.getChildrenCount() - 1).setData(f);

		//				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		//				{
		//					int64_t total_size = 0;
		//					func(f->full_path, it.getChild(it.getChildrenCount() - 1));
		//					for (int i = 0; i < it.getChild(it.getChildrenCount() - 1).getChildrenCount(); i++)
		//						total_size += it.getChild(it.getChildrenCount() - 1).getChild(i).getData()->size;
		//					it.getChild(it.getChildrenCount() - 1).getData()->size = total_size;
		//				}
		//			} while (FindNextFileW(hListFile, &FindFileData));
		//		}
		//	};

		//	func(path, tree.getRoot());
		//	return tree;

		//}


	}

}

#endif
