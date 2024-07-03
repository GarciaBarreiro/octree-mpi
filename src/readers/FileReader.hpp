//
// Created by miguelyermo on 6/8/21.
//

/*
* FILENAME :  FileReader.h  
* PROJECT  :  rule-based-classifier-cpp
* DESCRIPTION :
*  
*
*
*
*
* AUTHOR :    Miguel Yermo        START DATE : 14:28 6/8/21
*
*/

#pragma once

#include "Box.hpp"
#include "Lpoint.hpp"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

/**
 * @author Miguel Yermo
 * @brief Abstract class defining common behavor for all file readers
 */
class FileReader
{
	protected:
	/**
	 * @brief Path to file to be written
	 */
	fs::path path{};

	public:
	// ***  CONSTRUCTION / DESTRUCTION  *** //
	// ************************************ //

	/**
	 * @brief Instantiate a FileReader which reads a file from a given path
	 * @param path
	 */
	FileReader(const fs::path& path) : path(path){};
	virtual ~FileReader(){}; // Every specialization of this class must manage its own destruction
	virtual std::vector<Lpoint> read() = 0;
    virtual std::vector<Lpoint> readOverlap(const Box& box, const Box& overlap) = 0;
    virtual std::pair<Point, Point> getMinMax() = 0;
};
