/*
 * MxArrayNDimWrapper.cpp
 *
 *  Created on: 05.05.2015
 *      Author: neunertm
 */

#include <matlabCppInterface/internal/MxArrayNDimWrapper.hpp>

namespace matlab {

// helper for doubles, will be used by all scalar types
void convertToDoubleVector(std::vector<double, std::allocator<double> >& content, mxArray* &mxArray)
{
	const size_t nDims = 2;

	if(!mxIsNumeric(mxArray)) throw std::runtime_error("Variable is not numeric");
	if(mxIsEmpty(mxArray)) throw std::runtime_error("Variable is empty!");
	if(mxGetNumberOfDimensions(mxArray) != nDims) throw std::runtime_error("Variable is not 2-dimensional");
	if(mxGetElementSize(mxArray) != sizeof(double)) throw std::runtime_error("Currently only matlab double arrays are supported");

	const size_t* dims = mxGetDimensions(mxArray);

	content.resize(dims[1]);

	const double* mxArrayData = mxGetPr(mxArray);

	memcpy(content.data(), &mxArrayData[0], content.size()*sizeof(double));
}

// helper for doubles, will be used by all scalar types
void convertFromDoubleVector(const std::vector<double, std::allocator<double> >& content, mxArray* &mxArray)
{
	if (content.size() == 0)
	{
		throw "Vector is empty.";
	}

	const size_t nDims = 2;
	size_t dims[nDims];
	dims[1] = content.size();
	dims[0] = 1;

	mxArray = mxCreateNumericArray(nDims, dims, mxDOUBLE_CLASS, mxREAL);
	double* mxArrayData = mxGetPr(mxArray);

	memcpy(&mxArrayData[0], content.data(), content.size()*sizeof(double));
}


// explicit template deduction
template<> void MxArrayNDimWrapper<double, std::allocator<double> >::convertFrom(const std::vector<double, std::allocator<double> >& content) {
	convertFromDoubleVector(content, _mxArray);
}

template<> void MxArrayNDimWrapper<float, std::allocator<float> >::convertFrom(const std::vector<float, std::allocator<float> >& content) {
	convertFromDoubleVector(castToDoubleVector(content), _mxArray);
}

template<> void MxArrayNDimWrapper<int, std::allocator<int> >::convertFrom(const std::vector<int, std::allocator<int> >& content) {
	convertFromDoubleVector(castToDoubleVector(content), _mxArray);
}

template<> void MxArrayNDimWrapper<size_t, std::allocator<size_t> >::convertFrom(const std::vector<size_t, std::allocator<size_t> >& content) {
	convertFromDoubleVector(castToDoubleVector(content), _mxArray);
}




template<> void MxArrayNDimWrapper<double, std::allocator<double> >::convertTo(std::vector<double, std::allocator<double> >& content)
{
	convertToDoubleVector(content, _mxArray);
}

template<> void MxArrayNDimWrapper<float, std::allocator<float> >::convertTo(std::vector<float, std::allocator<float> >& content)
{
	std::vector<double> contentDouble;
	convertToDoubleVector(contentDouble, _mxArray);
	content = castFromDoubleVector<float>(contentDouble);
}
template<> void MxArrayNDimWrapper<int, std::allocator<int> >::convertTo(std::vector<int, std::allocator<int> >& content)
{
	std::vector<double> contentDouble;
	convertToDoubleVector(contentDouble, _mxArray);
	content = castFromDoubleVector<int>(contentDouble);
}
template<> void MxArrayNDimWrapper<size_t, std::allocator<size_t> >::convertTo(std::vector<size_t, std::allocator<size_t> >& content)
{
	std::vector<double> contentDouble;
	convertToDoubleVector(contentDouble, _mxArray);
	content = castFromDoubleVector<size_t>(contentDouble);
}

}
