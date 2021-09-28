#pragma once

#include <map>
#include <vector>
#include <string>

namespace PC
{
	namespace HEADER
	{
		struct _PcdHeader
		{
			std::string comment;
			float version;
			std::string fields;
			std::map<std::string, std::vector<int>> size;
			std::map<std::string, std::vector<char>> type;
			std::map<std::string, std::vector<int>> count;
			std::map<std::string, int> width;
			std::map<std::string, int> height;
			std::map<std::string, std::vector<int>> viewpoint;
			int points;
			std::string data;
		};

		struct PCDHeader
		{
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

		struct PcdHeader
		{
			std::string comment;
			std::map<std::string, float> version;
			std::map<std::string, std::vector<char>> fields;
			std::map<std::string, std::vector<int>> size;
			std::map<std::string, std::vector<char>> type;
			std::map<std::string, std::vector<int>> count;
			std::map<std::string, int> width;
			std::map<std::string, int> height;
			std::map<std::string, std::vector<int>> viewpoint;
			std::map<std::string, int> points;
			std::map<std::string, std::string> data;
		};
	}

	namespace DATA
	{
		struct PointCloudData
		{
			float x;
			float y;
			float z;
		};

		struct PointCloudRGBData
		{
			float x;
			float y;
			float z;

			unsigned char red;
			unsigned char green;
			unsigned char blue;
		};

		typedef std::vector<PointCloudData> PointClouds;
		typedef std::vector<PointCloudRGBData> PointCloudRGBs;
	}
}