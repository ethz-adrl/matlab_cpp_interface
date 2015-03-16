/*
 * MxArrayWrapper.hpp
 *
 *  Created on: 13.03.2015
 *      Author: neunertm
 */

#ifndef MXARRAYWRAPPER_HPP_
#define MXARRAYWRAPPER_HPP_

#include <type_traits>

#include <Eigen/Core>

#include "matrix.h"

namespace matlab {

template <class ContentType>
class MxArrayWrapper
{
public:
	MxArrayWrapper() :
		_mxArray(NULL)
	{}

	MxArrayWrapper(const ContentType& value) :
		_mxArray(NULL)
	{
		set(value);
	}

	~MxArrayWrapper()
	{
		erase();
	}

	void erase()
	{
		if (_mxArray != NULL)
		{
			mxDestroyArray(_mxArray);
			_mxArray = NULL;
		}
	}

	void set(const ContentType& value)
	{
		erase();
		convertFrom(value);
	}


	void get(ContentType& value)
	{
		assert(_mxArray != NULL);
		convertTo(value);
	}

	mxArray* &mxArrayPtr() { return _mxArray; }


private:
	void convertFrom(const ContentType& content);

	void convertTo(ContentType& content);

	mxArray* _mxArray;


};

	// by default we assume an eigen matrix unless specified below
	template <typename ContentType>
	void MxArrayWrapper<ContentType>::convertFrom(const ContentType& content)
	{
		// if this throws an error your type is probably not implemented and the code tries to fall back to Eigen...
		Eigen::MatrixXd matrix = content.template cast<double>();

		_mxArray = mxCreateDoubleMatrix(matrix.rows(), matrix.cols(), mxREAL);

		assert(mxGetElementSize(_mxArray) == sizeof(double) && "Data types do not match");

	    // get pointer to _mxArray and copy over data
		double* _mxArrayData = mxGetPr(_mxArray);
		memcpy(_mxArrayData, matrix.data(), matrix.size()*sizeof(double));
	}


	void convertFromScalar(const double& value, mxArray* &mxArray)
	{
		mxArray = mxCreateDoubleScalar(value);
	}

	template<> void MxArrayWrapper<double>::convertFrom(const double& content) { convertFromScalar(static_cast<double>(content), _mxArray); }
	template<> void MxArrayWrapper<float>::convertFrom(const float& content) { convertFromScalar(static_cast<double>(content), _mxArray); }
	template<> void MxArrayWrapper<int>::convertFrom(const int& content) { convertFromScalar(static_cast<double>(content), _mxArray); }

	template<> void MxArrayWrapper<bool>::convertFrom(const bool& content)
	{
		_mxArray = mxCreateLogicalScalar(content);
	}

	template<> void MxArrayWrapper<std::string>::convertFrom(const std::string& content)
	{
		_mxArray = mxCreateString(content.c_str());
	}

	template<> void MxArrayWrapper<Eigen::MatrixXd>::convertTo(Eigen::MatrixXd& content)
	{
		assert(mxIsNumeric(_mxArray) && "Variable is not numeric");
		assert(!mxIsEmpty(_mxArray) && "Variable is empty!");
		assert(mxGetNumberOfDimensions(_mxArray) == 2 && "Variable is 2-dimensional");

		size_t rows = mxGetM(_mxArray);
		size_t cols = mxGetN(_mxArray);

		content.resize(rows, cols);

		void* _mxArrayData = mxGetData(_mxArray);

		assert(sizeof(content(0,0)) == mxGetElementSize(_mxArray) && "Datatype does not match");
		std::memcpy(content.data(), _mxArrayData, mxGetElementSize(_mxArray)*mxGetNumberOfElements(_mxArray));
	}


	// General template for scalars
	double convertToScalar(const mxArray* mxArray)
	{
		// Check type
		assert(mxIsNumeric(mxArray) && "Variable is not numeric (normally scalars are stored as doubles in Matlab)");
		assert(!mxIsEmpty(mxArray) && "Variable is empty!");
		assert(mxGetNumberOfElements(mxArray) == 1 && "Variable is not a scalar (has more than 1 element)");

		// read data
		return mxGetScalar(mxArray);
	}
	// explicit template deduction
	template<> void MxArrayWrapper<double>::convertTo(double& content) { content = convertToScalar(_mxArray); }
	template<> void MxArrayWrapper<float>::convertTo(float& content) { content = static_cast<double>(convertToScalar(_mxArray)); }
	template<> void MxArrayWrapper<int>::convertTo(int& content) { content = static_cast<double>(convertToScalar(_mxArray)); }

	template<> void MxArrayWrapper<bool>::convertTo(bool& content)
	{
		// Check type
		assert(mxIsLogical(_mxArray) && "Variable is not of boolean type");
		assert(!mxIsEmpty(_mxArray) && "Variable is empty!");
		assert(mxGetNumberOfElements(_mxArray) ==1 && "Variable is not a scalar (has more than 1 element)");

		// read data
		content = mxIsLogicalScalarTrue(_mxArray);
	}

	template<> void MxArrayWrapper<std::string>::convertTo(std::string& content)
	{
		// Check type
		assert(!mxIsEmpty(_mxArray) && "Variable is empty!");
		assert(mxIsChar(_mxArray) && "Variable is not a character/string");

		// read data
		// lenght + 1 because of 0 terminated string
		size_t lenght = mxGetNumberOfElements(_mxArray)+1;
		char* buffer = new char[lenght];
		buffer[lenght-1] = '\0';
		mxGetString(_mxArray, buffer, lenght);
		content = buffer;
	}

} // namespace matlab



#endif /* MXARRAYWRAPPER_HPP_ */
