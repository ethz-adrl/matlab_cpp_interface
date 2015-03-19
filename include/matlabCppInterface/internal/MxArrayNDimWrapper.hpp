/*
 * MxArrayNDimWrapper.hpp
 *
 *  Created on: 17.03.2015
 *      Author: neunertm
 */

#ifndef MXARRAYNDIMWRAPPER_HPP_
#define MXARRAYNDIMWRAPPER_HPP_

#include <Eigen/Core>

#include "matrix.h"

namespace matlab {

template <class ContentType, class AllocatorType>
class MxArrayNDimWrapper
{
public:
	MxArrayNDimWrapper() :
		_mxArray(NULL)
	{}

	MxArrayNDimWrapper(const std::vector<ContentType, AllocatorType>& value) :
		_mxArray(NULL)
	{
		set(value);
	}

	~MxArrayNDimWrapper()
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


	void set(const std::vector<ContentType, AllocatorType>& value)
	{
		erase();
		convertFrom(value);
	}


	void get(std::vector<ContentType, AllocatorType>& value)
	{
		assert(_mxArray != NULL);
		convertTo(value);
	}

	mxArray* &mxArrayPtr() { return _mxArray; }


private:
	void convertFrom(const std::vector<ContentType, AllocatorType>& content);

	void convertTo(std::vector<ContentType, AllocatorType>& content);

	mxArray* _mxArray;


};

template <class ContentType, class AllocatorType>
void MxArrayNDimWrapper<ContentType, AllocatorType>::convertFrom(const std::vector<ContentType, AllocatorType>& content)
{
	if (content.size() == 0)
	{
		throw "Vector is empty.";
	}

	const size_t nDims = 3;
	size_t dims[nDims];
	dims[2] = content.size();
	dims[0] = content[0].rows();
	dims[1] = content[0].cols();

	_mxArray = mxCreateNumericArray(nDims, dims, mxDOUBLE_CLASS, mxREAL);

	for (size_t i=0; i<content.size(); i++)
	{
		if (content[i].rows() != dims[0] || content[i].cols() != dims[1])
		{
			throw "Not all matrices in vector are of equal size.";
		}

		Eigen::MatrixXd matrix = content[i].template cast<double>();

		double* mxArrayData = mxGetPr(_mxArray);
		memcpy(&mxArrayData[i*matrix.size()], matrix.data(), matrix.size()*sizeof(double));
	}
}

template <class ContentType, class AllocatorType>
void MxArrayNDimWrapper<ContentType, AllocatorType>::convertTo(std::vector<ContentType, AllocatorType>& content)
{
	const size_t nDims = 3;

	assert(mxIsNumeric(_mxArray) && "Variable is not numeric");
	assert(!mxIsEmpty(_mxArray) && "Variable is empty!");
	assert(mxGetNumberOfDimensions(_mxArray) == nDims && "Variable is not 3-dimensional");
	assert(sizeof(double) == mxGetElementSize(_mxArray) && "Currently only matlab double arrays are supported");

	const size_t* dims = mxGetDimensions(_mxArray);

	content.resize(dims[2]);
	Eigen::MatrixXd matrix(dims[0], dims[1]);

	for (size_t i=0; i<content.size(); i++)
	{
		double* mxArrayData = mxGetPr(_mxArray);
		memcpy(matrix.data(), &mxArrayData[i*matrix.size()], matrix.size()*sizeof(double));
		content[i] = matrix.cast<typename ContentType::Scalar>();
	}
}


} // matlab

#endif /* MXARRAYNDIMWRAPPER_HPP_ */
