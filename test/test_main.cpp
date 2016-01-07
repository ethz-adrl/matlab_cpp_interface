#define DEBUG
#undef NDEBUG

#include <MatlabInterfaceTests.hpp>
#include <MatFileTest.hpp>

/// Run all the tests that were declared with TEST()
int main(int argc, char **argv){

	std::cout<<"Starting matlab engine test"<<std::endl;
	testInit();
	testCommand();
	testPut();
	testPutEigen();
	testGet();
	testGetEigen();
	testMixedPut();
	testGui();
	std::cout<<"Completed matlab engine test"<<std::endl;

	std::cout<<"Starting mat-file test"<<std::endl;
	testOpenClose();
	testWriteRead();
	testWriteEigen();
	testWriteScalarVectors();
	std::cout<<"Completed mat-file test"<<std::endl;
}
