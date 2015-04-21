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

// Matlab's mxArray stuff
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

		static_assert(std::is_same<typename ContentType::Scalar, double>::value, "YOU ARE TRYING TO PUT A TYPE THAT IS NOT SUPPORTED BY THE INTERFACE. MAYBE YOU ARE TRYING TO PUT AN EIGEN MATRIX/VECTOR WITH A SCALAR TYPE THAT IS NOT DOUBLE.");

	    // get pointer to _mxArray and copy over data
		double* _mxArrayData = mxGetPr(_mxArray);
		memcpy(_mxArrayData, matrix.data(), matrix.size()*sizeof(double));
	}


	void convertFromScalar(const double& value, mxArray* &mxArray);

	template<> void MxArrayWrapper<double>::convertFrom(const double& content);
	template<> void MxArrayWrapper<float>::convertFrom(const float& content);
	template<> void MxArrayWrapper<int>::convertFrom(const int& content);
	template<> void MxArrayWrapper<size_t>::convertFrom(const size_t& content);

	template<> void MxArrayWrapper<bool>::convertFrom(const bool& content);

	template<> void MxArrayWrapper<std::string>::convertFrom(const std::string& content);


	// General template to catch non-supported types
	class FalseType {};
	template <typename ContentType>
	void MxArrayWrapper<ContentType>::convertTo(ContentType& content)
	{
		static_assert(std::is_same<ContentType, FalseType>::value, "YOU ARE TRYING TO GET A TYPE THAT IS NOT SUPPORTED BY THE INTERFACE. MAYBE YOU ARE TRYING TO GET AN EIGEN MATRIX/VECTOR WITH FIXED SIZE OR WITH A SCALAR TYPE THAT IS NOT DOUBLE.");
	}

	// General template specialization for Eigen
	template<> void MxArrayWrapper<Eigen::MatrixXd>::convertTo(Eigen::MatrixXd& content);
	template<> void MxArrayWrapper<Eigen::VectorXd>::convertTo(Eigen::VectorXd& content);

	// General template for scalars
	double convertToScalar(const mxArray* mxArray);

	// explicit template deduction
	template<> void MxArrayWrapper<double>::convertTo(double& content);
	template<> void MxArrayWrapper<float>::convertTo(float& content);
	template<> void MxArrayWrapper<int>::convertTo(int& content);
	template<> void MxArrayWrapper<size_t>::convertTo(size_t& content);

	template<> void MxArrayWrapper<bool>::convertTo(bool& content);

	template<> void MxArrayWrapper<std::string>::convertTo(std::string& content);

} // namespace matlab



#endif /* MXARRAYWRAPPER_HPP_ */
