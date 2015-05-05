/*
 * MatFileTest.hpp
 *
 *  Created on: 13.03.2015
 *      Author: neunertm
 */

#ifndef MATFILETEST_HPP_
#define MATFILETEST_HPP_

#include <matlabCppInterface/MatFile.hpp>

void testOpenClose()
{
	matlab::MatFile file;

	file.open("nonexisting.mat", matlab::MatFile::READ);
	assert(!file.isOpen());
	assert(!file.isWritable());
	assert(!file.isModifyable());
	file.close();

	file.open("nonexisting.mat", matlab::MatFile::UPDATE);
	assert(!file.isOpen());
	assert(file.isWritable());
	assert(file.isModifyable());
	file.close();

	file.open("test.mat", matlab::MatFile::WRITE);
	assert(file.isOpen());
	assert(file.isWritable());
	assert(!file.isModifyable());
	file.close();

	file.open("test.mat", matlab::MatFile::READ);
	assert(file.isOpen());
	assert(!file.isWritable());
	assert(!file.isModifyable());
	file.close();

	file.open("test.mat", matlab::MatFile::UPDATE);
	assert(file.isOpen());
	assert(file.isWritable());
	assert(file.isModifyable());
	file.close();

	file.open("test.mat", matlab::MatFile::WRITE_COMPRESSED);
	assert(file.isOpen());
	assert(file.isWritable());
	assert(!file.isModifyable());
	file.close();

	file.open("test.mat", matlab::MatFile::WRITE_HDF5);
	assert(file.isOpen());
	assert(file.isWritable());
	assert(!file.isModifyable());
	file.close();
}

void testWriteRead()
{
	matlab::MatFile file;

	assert(file.open("test.mat", matlab::MatFile::WRITE_COMPRESSED));
	assert(file.isOpen());
	assert(file.isWritable());
	double a = 12312.1;
	bool b = true;
	int c = 1322;
	assert(file.put("a", a));
	assert(file.put("b", b));
	assert(file.put("c", c, true));
	assert(file.close());

	assert(file.open("test.mat", matlab::MatFile::READ));
	assert(file.isOpen());
	assert(!file.isWritable());
	double aTest = 0;
	bool bTest = !b;
	int cTest = 0;
	assert(file.get("a", aTest));
	assert(file.get("b", bTest));
	assert(file.get("c", cTest));

	std::vector<std::string> variableList;
	file.getVariableList(variableList);

	assert(variableList.size() == 3);
	//variables not in order
	for (size_t i=0; i<variableList.size(); i++)
	{
		assert(variableList[i] == "a" || variableList[i] == "b" || variableList[i] == "c");
	}
	std::cout<<"Will now print variable list."<<std::endl;
	std::cout<<"Variables should all be two dimensional. a, b are non-global but c is global"<<std::endl;
	file.printVariableList();

	assert(file.close());
	assert(file.open("test.mat", matlab::MatFile::UPDATE));

	assert(file.deleteVariable("c"));
	file.getVariableList(variableList);

	assert(variableList.size() == 2);
	//variables not in order
	for (size_t i=0; i<variableList.size(); i++)
	{
		assert(variableList[i] == "a" || variableList[i] == "b");
	}

	file.close();

	assert(a == aTest);
	assert(b == bTest);
	assert(c == cTest);
}


void testWriteEigen()
{
	matlab::MatFile file;

	assert(file.open("test.mat", matlab::MatFile::WRITE_COMPRESSED));
	assert(file.isOpen());
	assert(file.isWritable());
	Eigen::MatrixXd a(2, 2);
	a << 1.0, 2.23, -3.13, 4;
	Eigen::Vector3i b;
	b << 1, 2, 3;

	std::vector<Eigen::MatrixXd> a_vec;
	a_vec.push_back(a);
	a_vec.push_back(2.0*a);

	// have to use aligned alocator here, see: http://eigen.tuxfamily.org/dox/group__TopicStlContainers.html
	std::vector<Eigen::Vector3i, Eigen::aligned_allocator<Eigen::Vector3i> > b_vec;
	b_vec.push_back(b);
	b_vec.push_back(2*b);

	assert(file.put("a", a));
	//assert(file.put("b", b));
	assert(file.put("a_vec", a_vec));
	assert(file.put("b_vec", b_vec));

	assert(file.close());

	assert(file.open("test.mat", matlab::MatFile::READ));
	assert(file.isOpen());
	assert(!file.isWritable());
	Eigen::MatrixXd a_test(2, 2);
	//Eigen::Vector3i b_test;
	std::vector<Eigen::MatrixXd> a_vec_test(2);
	//std::vector<Eigen::Vector3i, Eigen::aligned_allocator<Eigen::Vector3i> > b_vec_test(2);

	assert(file.get("a", a_test));
	//assert(file.get("b", b_test));
	assert(file.get("a_vec", a_vec_test));
	//assert(file.get("b_vec", b_vec_test));

	assert(a == a_test);
	//assert(b == b_test);

	assert(a_vec.size() == a_vec_test.size());
	//assert(b_vec.size() == b_vec_test.size());

	for (size_t i=0; i<a_vec.size(); i++)
	{
		assert(a_vec[i] == a_vec_test[i]);
	}

	assert(file.close());
}




void testWriteScalarVectors()
{
	matlab::MatFile file;

	assert(file.open("test.mat", matlab::MatFile::WRITE_COMPRESSED));
	assert(file.isOpen());
	assert(file.isWritable());

	std::vector<double> a(10);
	std::vector<float> b(3);
	std::vector<size_t> c(5);
	std::vector<int> d(12);

	for (size_t i=0; i<d.size(); i++)
	{
		if (i<a.size()) { a[i] = i; }
		if (i<b.size()) { b[i] = i; }
		if (i<c.size()) { c[i] = i; }
		if (i<d.size()) { d[i] = i; }
	}

	assert(file.put("a", a));
	assert(file.put("b", b));
	assert(file.put("c", c));
	assert(file.put("d", d));

	assert(file.close());

	assert(file.open("test.mat", matlab::MatFile::READ));
	assert(file.isOpen());
	assert(!file.isWritable());

	std::vector<double> a_test;
	std::vector<float> b_test;
	std::vector<size_t> c_test;
	std::vector<int> d_test;

	assert(file.get("a", a_test));
	assert(file.get("b", b_test));
	assert(file.get("c", c_test));
	assert(file.get("d", d_test));

	assert(a.size() == a_test.size());
	assert(b.size() == b_test.size());
	assert(c.size() == c_test.size());
	assert(d.size() == d_test.size());

	assert(a == a_test);
	assert(b == b_test);
	assert(c == c_test);
	assert(d == d_test);

	for (size_t i=0; i<d.size(); i++)
	{
		if (i<a_test.size()) { assert(a_test[i] == i); }
		if (i<b_test.size()) { assert(b_test[i] == i); }
		if (i<c_test.size()) { assert(c_test[i] == i); }
		if (i<d_test.size()) { assert(d_test[i] == i); }
	}

	assert(file.close());
}

#endif /* MATFILETEST_HPP_ */
