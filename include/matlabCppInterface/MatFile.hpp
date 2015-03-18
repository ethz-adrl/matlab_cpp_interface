/*
 * MatFile.hpp
 *
 *  Created on: 13.03.2015
 *      Author: neunertm
 */

#ifndef MATFILE_HPP_
#define MATFILE_HPP_

#include <vector>

#include <matlabCppInterface/internal/helpers.hpp>
#include <matlabCppInterface/internal/MxArrayWrapper.hpp>
#include <matlabCppInterface/internal/MxArrayNDimWrapper.hpp>

#include <mat.h>

namespace matlab {

class MatFile {

public:
	enum OPEN_MODE{
		READ = 0, // read only
		UPDATE,
		WRITE, // normal write
		WRITE_COMPRESSED, // write compressed (Matlab standard)
		WRITE_HDF5 // for big data > 2GB
	};

	MatFile();

	MatFile(const std::string& filename, OPEN_MODE mode);

	~MatFile();

	// open a mat file
	// [in] string - filename
	// [in] ioFlag - either 'r' for read, 'w' for write or 'u' for update (read/write)
	bool open(const std::string& filename, OPEN_MODE mode);

	bool isOpen() { return _isOpen; }

	bool isWritable() { return _isWritable; }

	bool isModifyable() { return _isModifyable; }

	bool close();

	template <typename ValueType>
	bool put(const std::string& name, const ValueType& value, bool globalVariable = false);

	template <typename ValueType>
	bool get(const std::string& name, ValueType& rValue);

	template <typename ValueType, typename AllocatorType>
	bool put(const std::string& name, const std::vector<ValueType, AllocatorType>& value, bool globalVariable = false);

	template <typename ValueType, typename AllocatorType>
	bool get(const std::string& name, std::vector<ValueType, AllocatorType>& rValue);

	bool deleteVariable(const std::string& name);

	bool getVariableList(std::vector<std::string>& variableList);

	void printVariableList(bool verbose = true);

	bool getVariableInfo(const std::string& variableName, int& dimensions, bool& isGlobalVariable);


private:
	MATFile* _file;
	std::string _filename;
	bool _isOpen;
	bool _isWritable;
	bool _isModifyable;
};


template <typename ValueType>
bool MatFile::put(const std::string& name, const ValueType& value, bool globalVariable)
{
	if (!_isOpen || !_isWritable) { return false; }
	helpers::assertValidVariableName(name);

	MxArrayWrapper<ValueType> mxArray(value);

	// send data and verify
	int success = -1;
	if (globalVariable)
	{
		success = matPutVariableAsGlobal(_file, name.c_str(), mxArray.mxArrayPtr());
	} else
	{
		success = matPutVariable(_file, name.c_str(), mxArray.mxArrayPtr());
	}

	if (success == 0)
	{
		return true;
	}
	return false;
}

template <typename ValueType>
bool MatFile::get(const std::string& name, ValueType& rValue)
{
	if (!_isOpen) { return false; }
	helpers::assertValidVariableName(name);

	// Get variable from matlab
	MxArrayWrapper<ValueType> mxArray;
	mxArray.mxArrayPtr() = matGetVariable(_file, name.c_str());

	mxArray.get(rValue);

	if(mxArray.mxArrayPtr() == NULL)
	{
		return false;
	}

	return true;
}

template <typename ValueType, typename AllocatorType>
bool MatFile::put(const std::string& name, const std::vector<ValueType, AllocatorType>& value, bool globalVariable)
{
	if (!_isOpen) { return false; }
	helpers::assertValidVariableName(name);

	MxArrayNDimWrapper<ValueType, AllocatorType> mxArray(value);

	// send data and verify
	int success = -1;
	if (globalVariable)
	{
		success = matPutVariableAsGlobal(_file, name.c_str(), mxArray.mxArrayPtr());
	} else
	{
		success = matPutVariable(_file, name.c_str(), mxArray.mxArrayPtr());
	}

	if (success == 0)
	{
		return true;
	}

	return false;
}


template <typename ValueType, typename AllocatorType>
bool MatFile::get(const std::string& name, std::vector<ValueType, AllocatorType>& rValue)
{
	if (!_isOpen) { return false; }
	helpers::assertValidVariableName(name);

	// Get variable from matlab
	MxArrayNDimWrapper<ValueType, AllocatorType> mxArrayNDimWrapped;
	mxArrayNDimWrapped.mxArrayPtr() = matGetVariable(_file, name.c_str());

	if(mxArrayNDimWrapped.mxArrayPtr() == NULL)
	{
		return false;
	}

	mxArrayNDimWrapped.get(rValue);
	return true;
}


}



#endif /* MATFILE_HPP_ */
