//
// Created by miguelyermo on 6/8/21.
//

#pragma once

#include "LasFileReader.hpp"
#include "TxtFileReader.hpp"
#include <filesystem>

namespace fs = std::filesystem;

// ** Types of readers ** //

enum FileReader_t // Enumeration of the different types of FileReader
{
	txt_t, // txt type
	las_t, // las type
	err_t  // error type (no compatible extension were found)
};

FileReader_t chooseReaderType(const std::string& fExt);

/**
 * @author Miguel Yermo
 * @brief FileReader Factory class. Used to create the appropriate readers based on the type of file to be read
 */
class FileReaderFactory
{
	public:
	/**
	 * @brief Return the reader according to the chosen type
	 * @param type Type of reader to be created
	 * @param path Path to the file to be read
	 * @param numCols Number of columns of the txt file. Default = 0
	 * @return
	 */
	static std::shared_ptr<FileReader> makeReader(FileReader_t type, const fs::path& path)
	{
		switch (type)
		{
			case txt_t:
				return std::make_shared<TxtFileReader>(path);
			case las_t:
				return std::make_shared<LasFileReader>(path);
			default:
				std::cout << "Unable to create specialized FileReader\n";
				exit(-2);
				return nullptr;
		}
	}
};
