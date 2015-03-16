/*
 * MatFile.cpp
 *
 *  Created on: 13.03.2015
 *      Author: neunertm
 */

#include <iostream>

#include <matlabCppInterface/MatFile.hpp>

namespace matlab {

MatFile::MatFile() :
	_file(NULL),
	_isOpen(false),
	_isWritable(true),
	_isModifyable(false)
{};

MatFile::MatFile(const std::string& filename, OPEN_MODE mode) :
	_file(NULL),
	_isOpen(false),
	_isWritable(true),
	_isModifyable(false)
{
	open(filename, mode);
}

MatFile::~MatFile()
{
	close();
}

// open a mat file
// [in] string - filename
// [in] ioFlag - either 'r' for read, 'w' for write or 'u' for update (read/write)
bool MatFile::open(const std::string& filename, OPEN_MODE mode)
{
	if (_file != NULL) { matClose(_file); }

	std::string ioflags = "";
	_isWritable = true;
	_isModifyable = false;

	switch (mode)
	{
		case READ: { ioflags = "r"; _isWritable = false; break; }
		case UPDATE: { ioflags = "u"; _isModifyable = true; break; }
		case WRITE: { ioflags = "w"; break; }
		case WRITE_COMPRESSED: { ioflags = "wz"; break; }
		case WRITE_HDF5: { ioflags = "w7.3"; break; }
	}

	if (ioflags != "" && filename != "")
	{
		_file = matOpen(filename.c_str(), ioflags.c_str());
	}

	if (_file)
	{
		_filename = filename;
		_isOpen = true;
		return true;
	}

	return false;
}


bool MatFile::close()
{
	_isWritable = false;
	if (_file)
	{
		if (matClose(_file) == 0)
		{
			_isOpen = false;
			_file = NULL;
			return true;
		}
	}
	return false;
}


bool MatFile::deleteVariable(const std::string& name)
{
	if (!_isModifyable || !_isOpen || !_isWritable) { return false; }

	if (matDeleteVariable(_file, name.c_str()) == 0)
	{
		return true;
	}
	return false;
}

bool MatFile::getVariableList(std::vector<std::string>& variableList)
{
	if (!_isOpen) { return false; }

	variableList.clear();

	int nVariables = 0;
	char** dir = matGetDir(_file, &nVariables);
	if (dir == NULL) {
		return false;
	} else {
		for (int i=0; i < nVariables; i++)
		{
			variableList.push_back(dir[i]);
		}
  }
  mxFree(dir);
  return true;
}

void MatFile::printVariableList(bool verbose)
{
	if (!_isOpen) {
		std::cout<<"No file opened."<<std::endl;
		return;
	}

	std::vector<std::string> variableList;
	getVariableList(variableList);

	std::cout<<"Variables in "<<_filename<<std::endl;
	if (verbose) { std::cout << "dim " << "global "; }

	std::cout << "name"<<std::endl;

	for (size_t i=0; i<variableList.size(); i++)
	{
		if (verbose)
		{
			int dimensions = 0;
			bool isGlobal = false;
			getVariableInfo(variableList[i], dimensions, isGlobal);
			std::cout<<dimensions<<"   ";
			if (isGlobal) { std::cout<<"yes    "; } else { std::cout<<"no     "; }
		}
		std::cout<<variableList[i]<<std::endl;
	}
}

bool MatFile::getVariableInfo(const std::string& variableName, int& dimensions, bool& isGlobalVariable)
{
	if (!_isOpen) { return false; }

	mxArray* mxArray = matGetVariableInfo(_file, variableName.c_str());

	if (mxArray == NULL)
	{
		return false;
	}

	dimensions = mxGetNumberOfDimensions(mxArray);

	isGlobalVariable = mxIsFromGlobalWS(mxArray);

	mxDestroyArray(mxArray);

	return true;
}

} // namespace matlab
