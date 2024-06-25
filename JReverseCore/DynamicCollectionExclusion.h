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