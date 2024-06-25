#pragma once
#include <string>

enum DynamicCollectionExclusionType
{
	StartsWith = 0,
	Contains = 1,
	Both = 2
};

struct DynamicCollectionExclusion
{
	DynamicCollectionExclusionType type;
	std::string name;
};

inline DynamicCollectionExclusionType getExclusionType(int type) {
	switch (type)
	{
	case 0:
		return StartsWith;
	case 1:
		return Contains;
	case 2:
		return Both;
	default:
		return StartsWith;
	}
}