#pragma once
#ifndef CCNPACKAGE_H
#define CCNPACKAGE_H
#include "iostream"
#include "../BinaryTools/BinaryReader.h" // Include BinaryReader
#include "../BinaryTools/BinaryWriter.h" // Include BinaryWriter


class CCNPackage
{
public:
	std::string unpackheader;
	std::string header; // "PAMU" or "PAME
	short runtimeVersion;
	short runtimeSubversion;
	int productVersion;
	int productBuild;
	CCNPackage();
	~CCNPackage();
	virtual void ReadCCN(BinaryReader& buffer);
	virtual void WriteCCN(BinaryWriter& buffer);
};

#endif // !CCNPACKAGE_H
