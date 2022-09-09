#include "ImageImporter.h"
#include <fstream>
#include "FileUtility.h"
#include <filesystem>
namespace fs = std::filesystem;

void ImageImporter::Import(std::string InputFile, std::string CurrentFilePath)
{
	std::string FileName = std::string(GetFileNameFromPath(InputFile));
	std::string FileNameWithoutExtension = FileName.substr(0, FileName.find_last_of("."));
	std::string OutputFileName = CurrentFilePath + "/" + FileName;
	fs::copy(InputFile, OutputFileName);
}
