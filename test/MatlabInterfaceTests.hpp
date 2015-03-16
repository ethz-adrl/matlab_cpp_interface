// Bring in the Matlab Interface
#include <matlabCppInterface/Engine.hpp>

void testInit()
{
  std::cout<<"Performing initialization test"<<std::endl;
  matlab::Engine engine;
  assert(!engine.isInitialized() && "initialization error");
  assert(engine.initialize() && "initialization error");
  assert(engine.isInitialized() && "initialization error");
  assert(engine.stop() && "initialization error");
  assert(!engine.isInitialized() && "initialization error");
  std::cout<<"Finished initialization test"<<std::endl;
}

void testCommand()
{
  std::cout<<"Testing commands"<<std::endl;
  matlab::Engine engine;
  engine.initialize();
  std::string ret = engine.executeCommand("disp('test')");
  std::cout<<"Should return 'test', is returning: "<<ret<<std::endl;
  assert(engine.executeCommand("disp('test')")==">> test");
  std::cout<<"Finished testing commands"<<std::endl;
}

void testPut()
{
	std::cout<<"Testing standard type putting"<<std::endl;
  matlab::Engine engine;
  engine.initialize();
  
  double a = -12321.12;
  int b = static_cast<int>(-12321);
  bool c = true;
  std::string d = "test";
  
  engine.put("a", a);
  engine.put("b", b);
  engine.put("c", c); 
  engine.put("d", d);

  std::cout<<"Finished standard type putting"<<std::endl;
}

void testPutEigen()
{
	std::cout<<"Testing eigen type putting"<<std::endl;

  matlab::Engine engine;
  engine.initialize();

  Eigen::Matrix2d A = Eigen::Matrix2d::Random();
  Eigen::Matrix3i B = Eigen::Matrix3i::Random();
  Eigen::Vector3d C = Eigen::Vector3d::Random();
  
  engine.put("A", A);
  engine.put("B", B);
  engine.put("C", C);

  std::cout<<"Finished eigen type putting"<<std::endl;
}

void testGet()
{
  std::cout<<"Testing standard type putting/getting"<<std::endl;

  matlab::Engine engine;
  engine.initialize();
  
  double a = 123213.132;
  int b = static_cast<int>(2131);
  bool c = true;
  std::string d = "test";
  
  engine.put("a", a);
  engine.put("b", b);
  engine.put("c", c); 
  engine.put("d", d);

  engine.executeCommand("ab = a*b;");
  double abTest = 0;
  engine.get("ab", abTest);
  assert(abTest == a*static_cast<double>(b));
  
  std::string dTest;
  engine.get("d", dTest);
  assert(d == dTest);

  std::cout<<"Finished standard type putting/getting"<<std::endl;
}

void testGetEigen()
{
  std::cout<<"Testing eigen type putting/getting"<<std::endl;

  matlab::Engine engine;
  engine.initialize();
  
  Eigen::Matrix3d A = Eigen::Matrix3d::Random();
  Eigen::Matrix3i B = Eigen::Matrix3i::Random();
  Eigen::Matrix3d C = Eigen::Matrix3d::Random();
  Eigen::Vector2d D = Eigen::Vector2d::Random();
  
  engine.put("A", A);
  engine.put("B", B);
  engine.put("C", C);
  engine.put("D", D);
  
  engine.executeCommand("AC = A+C;");
  Eigen::MatrixXd ACTest;
  Eigen::Matrix3d AC = A+C;
  engine.get("AC", ACTest);
  assert(ACTest == AC);

  Eigen::MatrixXd DTest;
  engine.get("D", DTest);
  assert(DTest == D);

  std::cout<<"Finished eigen type putting/getting"<<std::endl;
}

void testMixedPut()
{
  std::cout<<"Testing mixed type putting/getting"<<std::endl;

  matlab::Engine engine;
  engine.initialize();
  
  double a = -123.12;
  std::string b = "test";
  Eigen::Matrix3d A = Eigen::Matrix3d::Random();
  
  engine.put("a", a);
  engine.put("b", b);
  engine.put("A", A);
  
  assert(engine.exists("a"));
  std::cout<<"Test will check for non-existing variable x."<<std::endl;
  std::cout<<"Possible 'Variable x not found' warnings are expected."<<std::endl;
  try {
	  assert(!engine.exists("x"));
  } catch(...)
  {
	  std::cout<<"Test successful, did not find x."<<std::endl;
  }
  assert(engine.isScalar("a"));
  assert(!engine.isScalar("b"));
  assert(!engine.isEmpty("a"));
  assert(engine.isCharOrString("b"));
  assert(!engine.isCharOrString("a"));

  size_t rows, cols;
  engine.getDimensions("a", rows, cols);
  assert((rows==1 && cols==1));
  
  engine.getDimensions("A", rows, cols);
  assert((rows==3 && cols==3));

  std::cout<<"Finished mixed type putting/getting"<<std::endl;
}

void testGui()
{
  std::cout<<"Will test GUI now"<<std::endl;
  std::cout<<"Will open workspace and interactive console"<<std::endl;
  std::cout<<"Exit console to complete test"<<std::endl;

  matlab::Engine engine;
  engine.initialize();

  std::cout<<"Opening Workspace now."<<std::endl;
  engine.showWorkspace();
  std::cout<<"Opening Command Line now. Exit to complete test."<<std::endl;
  engine.openCommandLine();
}
