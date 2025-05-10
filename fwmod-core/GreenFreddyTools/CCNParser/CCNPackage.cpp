#include "CCNPackage.h"
#include "Chunks/Chunks.h"
#include "../../Logger.h"

static Logger CCNPackageLogger;


CCNPackage::CCNPackage() : runtimeVersion(0), runtimeSubversion(0), productVersion(0), productBuild(0) {
	this->header = std::string();
	this->unpackheader = std::string();
	CCNPackageLogger.AddHandler(Logger::CreateFileHandler("CCNPackageReader.log"));
    CCNPackageLogger.Debug("CCNPackage Initialized");
}

CCNPackage::~CCNPackage() {
	CCNPackageLogger.Debug("CCNPackage destructor called");
}

void CCNPackage::ReadCCN(BinaryReader& buffer) {
	this->unpackheader = buffer.ReadFixedLengthString(28 + 4);

	// TODO: read pack files
	// buffer.Skip(4); // already skipping uint when reading unpackheader
	this->header = buffer.ReadFixedLengthString(4);
	CCNPackageLogger.Info("Header: " + this->header);

	this->runtimeVersion = buffer.ReadInt16();
	this->runtimeSubversion = buffer.ReadInt16();
	CCNPackageLogger.Info("Runtime Version: " + std::to_string(this->runtimeVersion) + "." + std::to_string(this->runtimeSubversion));

	this->productVersion = buffer.ReadInt32();
	this->productBuild = buffer.ReadInt32();
	CCNPackageLogger.Info("Proudct. Version: " + std::to_string(this->productVersion) + " Build: " + std::to_string(this->productBuild));
}

void CCNPackage::WriteCCN(BinaryWriter& buffer) {
	buffer.WriteFixedLengthString(this->unpackheader);
	buffer.WriteFixedLengthString(this->header);
	buffer.WriteInt16(this->runtimeVersion);
	buffer.WriteInt16(this->runtimeSubversion);
	buffer.WriteInt32(this->productVersion);
	buffer.WriteInt32(this->productBuild);
}