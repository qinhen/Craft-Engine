#pragma once

#include "./Common.h"
#include "../advance/json/Json.h"

namespace CraftEngine
{
	namespace graphics
	{

		struct MaterialFile
		{
			using vec4 = math::vec4;
			enum MapSource
			{
				eDisableSrc,
				eTextureSrc,
				eColorSrc,
			};
			struct MapInfo
			{
				int       index;
				MapSource src;
			};
			union MaterialData
			{
				MapInfo mapList[Material::eMaxMap];
			};
			core::ArrayList<std::string>  mTextureList;
			core::ArrayList<vec4>         mColorList;
			core::ArrayList<MaterialData> mMaterialDataList;
			//core::ArrayList<core::Bitmap> textureDataList;

			bool loadFromFile(const char* path)
			{
				return loadFromString(core::readString(path));
			}

			bool writeToFile(const char* path) const
			{
				auto&& str = writeToString();
				if (str.size() == 0)
					return false;
				core::writeString(str, path);
				return true;
			}

			bool loadFromString(const std::string& str)
			{
				if (str.size() == 0)
					return false;

				rapidjson::Document doc;
				doc.Parse(str.c_str());

				auto& textures = doc["Textures"];
				mTextureList.resize(textures.Size());
				for (int i = 0; i < mTextureList.size(); i++)
					mTextureList[i] = (textures[i].GetString());
				auto& colors = doc["Colors"];
				mColorList.resize(colors.Size());
				for (int i = 0; i < mColorList.size(); i++)
				{
					auto& color = colors[i];
					mColorList[i] = (vec4(color[0].GetFloat(), color[1].GetFloat(), color[2].GetFloat(), color[3].GetFloat()));
				}
				auto& materials = doc["Materials"];
				mMaterialDataList.resize(materials.Size());

				static const char* materialMapTypeStrings[] = {
					"albedoMap",
					"normalMap",
					"roughnessMap",
					"metallicMap",
					"occasionMap",
					"specularMap",
					"heightMap",
				};

				for (int i = 0; i < mMaterialDataList.size(); i++)
				{
					auto& material = materials[i];
					for (auto it = material.MemberBegin(); it != material.MemberEnd(); it++)
					{
						std::cout << "Member: " << it->name.GetString() << std::endl;
					}

					for (int j = 0; j < Material::eMaxMap; j++)
					{
						auto& mapName = materialMapTypeStrings[j];

						if (material.HasMember(mapName))
						{
							std::cout << "have " << mapName << std::endl;
							auto& tex = material[mapName];
							std::string type = tex["type"].GetString();
							if (type == "Texture") mMaterialDataList[i].mapList[j].src = eTextureSrc;
							else if (type == "Color") mMaterialDataList[i].mapList[j].src = eColorSrc;
							else throw std::exception("");
							mMaterialDataList[i].mapList[j].index = tex["index"].GetInt();
						}
						else
						{
							std::cout << "no " << mapName << std::endl;
							mMaterialDataList[i].mapList[j].src = eDisableSrc;
						}
					}
				}
				return true;
			}

			std::string writeToString() const
			{
				json::JsonFile file;
				file.startObject();
				file.key("Textures");
				file.startArray();
				for (const auto& it : mTextureList)
					file.value(it);
				file.endArray();

				file.key("Colors");
				file.startArray();
				for (const auto& it : mColorList)
				{
					file.startArray();
					for (int i = 0; i < 4; i++)
						file.value(it[i]);
					file.endArray();
				}
				file.endArray();

				static const char* materialMapTypeStrings[] = {
					"albedoMap",
					"normalMap",
					"roughnessMap",
					"metallicMap",
					"occasionMap",
					"specularMap",
					"heightMap",
				};

				file.key("Materials");
				file.startArray();
				for (const auto& it : mMaterialDataList)
				{
					file.startObject();
					for (int i = 0; i < Material::eMaxMap; i++)
					{
						if (it.mapList[i].src != eDisableSrc)
						{

							file.key(materialMapTypeStrings[i]);
							file.startObject();
							file.key("type");
							if (it.mapList[i].src == eTextureSrc) file.value("Texture");
							else if (it.mapList[i].src == eColorSrc) file.value("Color");
							else throw std::exception("");
							file.key("index");
							file.value(it.mapList[i].index);
							file.endObject();
			
						}
					}
					file.endObject();
				}
				file.endArray();
				file.endObject();

				return file.getString();
			}
		};


	}
}


