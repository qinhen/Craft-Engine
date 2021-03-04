#pragma once
#ifndef CRAFT_ENGINE_UTIL_PACKAGE_H_
#define CRAFT_ENGINE_UTIL_PACKAGE_H_

#include "./Core.h"
#include "../Win32/Win32Api.h"


namespace CraftEngine
{
	namespace Util
	{




		class PackageLoader
		{
		private:

			std::ifstream m_file;

			struct Folder
			{
				std::string name;
				size_t begin;
				size_t size;
			};

			GeneralTree<Folder> m_package;
		public:


			~PackageLoader()
			{
				clear();
			}

			void clear()
			{
				m_file.close();
				m_package.getRoot().clear();
			}

			bool isOpen()const
			{
				return 	m_file.is_open();
			}

			bool link(const std::wstring& pke)
			{
				clear();
				m_file.open(pke, std::ios::binary);
				if (!m_file.is_open())
					return false;

				//
				union FileHeader
				{
					struct
					{
						char id[16];
						uint8_t hash[16];
						int32_t block_header_begin;
						int32_t block_header_size;
						int32_t block_header_count;
					};
					uint8_t unused[64];
				};

				struct BlockHeader
				{
					char name[108];
					uint32_t children_count;
					uint64_t begin;
					uint64_t size;
				};

				FileHeader header = {};
				m_file.read((char*)&header, sizeof(FileHeader));

				std::vector<BlockHeader> headers_list(header.block_header_count);
				
				m_file.seekg(header.block_header_begin, std::ios::beg);
				m_file.read((char*)headers_list.data(), header.block_header_count * sizeof(BlockHeader));

				for (auto i : headers_list)
				{
					auto str = CraftEngine::Util::StringTool::fromUtf8(i.name);

					if (i.children_count == 0)
					{
						std::wcout << L"file:[" << i.size << "]" << str.c_str() << std::endl;
					}
					else
					{
						std::wcout << L"folder:[" << i.size << "]" << str.c_str() << std::endl;
					}
				}


				int index = 1;

				GeneralTree<Folder>::Iterator it = m_package.getRoot();

				it.setData(new Folder);
				it.getData()->begin = headers_list[0].begin;
				it.getData()->size = headers_list[0].size;
				it.getData()->name = headers_list[0].name;

				std::function<void(BlockHeader*, int&, GeneralTree<Folder>::Iterator)> contruct_tree;
				contruct_tree = [&](BlockHeader* headers_list, int& cur_index, GeneralTree<Folder>::Iterator it)
				{
					int temp_index = cur_index;
					for (int i = 0; i < headers_list[temp_index].children_count; i++)
					{
						Folder* folder = new Folder;
						folder->name = headers_list[cur_index].name;
						folder->size = headers_list[cur_index].size;
						folder->begin = headers_list[cur_index].begin;
						//it.addChild();
						it.addChild(folder);
						cur_index++;
						contruct_tree(headers_list, cur_index, it.getChild(i));
					}
				};
				contruct_tree(headers_list.data(), index, m_package.getRoot());




				return true;
			}



			std::vector<uint8_t> load(std::string path)
			{
				auto&& paths = parsePath(path);
				auto node = m_package.getRoot();
				for (int i = 0; i < paths.size(); i++)
				{
					bool found = false;
					for (int j = 0; j < node.getChildrenCount(); j++)
					{
						if (node.getChild(j).getData()->name == path)
						{
							node = node.getChild(j);
							found = true;
							break;
						}
					}
					if (!found)
					{
						return std::vector<uint8_t>();
					}
				}
				std::vector<uint8_t> data(node.getData()->size);
				m_file.seekg(node.getData()->begin, std::ios::beg);
				m_file.read((char*)data.data(), data.size());
				return data;
			}


			static std::vector<std::string> parsePath(const std::string& path)
			{
				std::vector<std::string> paths;
				int i = 0;
				int j = 0;
				while (i < path.size())
				{

					if (path[i] == '/')
					{
						if (i - j > 0)
						{
							std::string sub_path = path.substr(j, i - j);
							paths.push_back(sub_path);
							j = i + 1;
						}	
					}
					i++;
				}
				return paths;
			}


		};




		class PackageMaker
		{

			struct Folder
			{
				std::wstring src_path;
				std::wstring name;
			};

		public:
			bool make(std::wstring directory, std::wstring dstFile)
			{
				auto dir = Win32::getDirectory(directory);
				if (dir.getRoot().getChildrenCount() == 0)
					return false;


				union FileHeader
				{
					struct
					{
						char id[16];
						uint8_t hash[16];
						int32_t block_header_begin;
						int32_t block_header_size;
						int32_t block_header_count;
					};
					uint8_t unused[64];
				};

				struct BlockHeader
				{
					char name[108];
					uint32_t children_count;
					uint64_t begin;
					uint64_t size;
				};


				FileHeader header = {};
				std::vector<BlockHeader> headers_list;


				std::function<void(CraftEngine::Util::GeneralTree<CraftEngine::Win32::Folder>::Iterator)> func;
				func = [&](CraftEngine::Util::GeneralTree<CraftEngine::Win32::Folder>::Iterator it)
				{
					BlockHeader block_header = {};
					std::string utf8_string = CraftEngine::Util::StringTool::toUtf8(it.getData()->name);
					strcpy_s(block_header.name, 108, utf8_string.c_str());

					std::cout << "pack file: " << block_header.name << std::endl;
					block_header.size = it.getData()->size;
					block_header.children_count = it.getChildrenCount();
					headers_list.push_back(block_header);
					for (int i = 0; i < it.getChildrenCount(); i++)
					{
						func(it.getChild(i));
					}
				};

				func(dir.getRoot());

				headers_list[0].size = 0;
				for (int i = 0; i < dir.getRoot().getChildrenCount(); i++)
					headers_list[0].size += dir.getRoot().getChild(i).getData()->size;

				header.block_header_begin = sizeof(FileHeader);
				header.block_header_count = headers_list.size();
				header.block_header_size = headers_list.size() * sizeof(BlockHeader);

				int block_index = 0;
				int64_t block_begin = sizeof(FileHeader) + header.block_header_size;
				std::function<void(CraftEngine::Util::GeneralTree<CraftEngine::Win32::Folder>::Iterator, int, int64_t)> func2;
				func2 = [&](CraftEngine::Util::GeneralTree<CraftEngine::Win32::Folder>::Iterator it, int index, int64_t begin)
				{
					int cur_block_index = block_index;
					headers_list[cur_block_index].begin = begin;
					int cur_block_begin = begin;
					for (int i = 0; i < it.getChildrenCount(); i++)
					{
						block_index++;
						func2(it.getChild(i), block_index, cur_block_begin);
						cur_block_begin += it.getChild(i).getData()->size;
					}
				};
				func2(dir.getRoot(), block_index, block_begin);



				std::ofstream out_file(dstFile, std::ios::binary);
				out_file.write((char*)&header, sizeof(header));
				out_file.write((char*)&headers_list[0], header.block_header_size);


				std::function<void(CraftEngine::Util::GeneralTree<CraftEngine::Win32::Folder>::Iterator)> func3;
				func3 = [&](CraftEngine::Util::GeneralTree<CraftEngine::Win32::Folder>::Iterator it)
				{
					if (it.getChildrenCount() == 0)
					{

						std::ifstream file(it.getData()->full_path, std::ios::binary);
						if (!file.is_open())
							throw std::runtime_error("");

						std::wcout << "pack path: " << it.getData()->full_path << std::endl;

						int temp_size = it.getData()->size;
						uint8_t buffer[4096];
						while (temp_size > 0)
						{
							int read_size = 4096;
							if (temp_size < 4096)
								read_size = temp_size;
							file.read((char*)buffer, read_size);
							out_file.write((char*)buffer, read_size);
							temp_size -= read_size;
						}
					}

					for (int i = 0; i < it.getChildrenCount(); i++)
					{
						func3(it.getChild(i));
					}
				};

				func3(dir.getRoot());

				out_file.close();
			}

		};



	}

}



#endif // !CRAFT_ENGINE_UTIL_PACKAGE_H_
