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
				//打开剪切板
				j = OpenClipboard(NULL);
				//判断是否打开成功，如果打开失败则重新尝试5次
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
			//分配字节数，HGLOBAL表示一个内存块
			size_t mem_size = (text.size() + 1) * sizeof(wchar_t);
			HGLOBAL hmem = GlobalAlloc(GHND, mem_size);
			//锁定内存中指定的内存块，并返回一个地址值令其指向内存块的起始处
			void* pmem = (void*)GlobalLock(hmem);
			//清空剪切板并释放剪切板内数据的句柄
			EmptyClipboard();
			memcpy(pmem, text.data(), mem_size);
			SetClipboardData(CF_TEXT, hmem);        //写入数据
			CloseClipboard();                       //关闭剪切板
			GlobalFree(hmem);                       //释放内存块
			return true;     //返回成功
		}

		std::wstring copyTextFromClipboard()
		{
			if (!OpenClipboard(NULL))
				return std::wstring();
			if (!IsClipboardFormatAvailable(CF_TEXT))
				return std::wstring();
			HGLOBAL hg = GetClipboardData(CF_TEXT);
			//锁定内存块
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

		//		// 构造代表子目录和文件夹路径的字符串，使用通配符"*"
		//		lstrcpyW(szFilePath, path.c_str());
		//		// 注释的代码可以用于查找所有以“.txt”结尾的文件
		//		// lstrcat(szFilePath, "\\*.txt");
		//		lstrcatW(szFilePath, L"\\*");

		//		// 查找第一个文件/目录，获得查找句柄
		//		hListFile = FindFirstFileW(szFilePath, &FindFileData);
		//		// 判断句柄
		//		if (hListFile == INVALID_HANDLE_VALUE)
		//		{
		//			printf("错误：%d\n", GetLastError());
		//		}
		//		else
		//		{
		//			do
		//			{
		//				// 如果不想显示代表本级目录和上级目录的“.”和“..”，
		//				// 可以使用注释部分的代码过滤
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
