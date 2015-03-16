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

#endif /* MATFILETEST_HPP_ */
