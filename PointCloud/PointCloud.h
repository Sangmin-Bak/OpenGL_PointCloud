#pragma once

#ifdef _DLL_POINTCLOUD_
#define POINTCLOUD_DECLSPEC _declspec(dllexport)
#else
#define POINTCLOUD_DECLSPEC _declspec(dllimport)
#endif

#include "TypeDef.h"
#include <fstream>
#include <iostream>
#include <vector>

namespace PC
{
	class POINTCLOUD_DECLSPEC PointCloud
	{
	public:
		PointCloud();
		~PointCloud();

		template <typename T>
		bool LoadPCD(const char* pFilePath, std::vector<T>& pClouds);

		std::string comment;
		std::string version;
		std::string fields;
		std::string size;
		std::string type;
		std::string count;
		std::string width;
		std::string height;
		std::string viewpoint;
		std::string points;
		std::string data;
	};

	template <typename T>
	bool PointCloud::LoadPCD(const char* pFilePath, std::vector<T>& pClouds)
	{
		std::ifstream readFile;
		readFile.open(pFilePath);

		if (!readFile.is_open())
		{
			return false;
		}

		//HEADER::PcdHeader header;
		std::getline(readFile, comment);
		std::getline(readFile, version);
		std::getline(readFile, fields);
		std::getline(readFile, size);
		std::getline(readFile, type);
		std::getline(readFile, count);
		std::getline(readFile, width);
		std::getline(readFile, height);
		std::getline(readFile, viewpoint);
		std::getline(readFile, points);
		std::getline(readFile, data);

		while (!readFile.eof())
		{
			T cloud;
			readFile >> cloud.x;
			readFile >> cloud.y;
			readFile >> cloud.z;

			pClouds.push_back(cloud);
		}

		readFile.close();

		return true;
	}

}

