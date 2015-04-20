#include <matlabCppInterface/Engine.hpp>
#include <cctype>
#include <stdio.h>

namespace matlab {


Engine::Engine()
{
	_engine = NULL;
	// make sure the output buffer is NULL terminated
	_outputBuffer[OUTPUT_BUFFER_SIZE] = '\0';
}

Engine::Engine(bool startMatlabAtInitialization)
{
	// make sure the output buffer is NULL terminated
	_outputBuffer[OUTPUT_BUFFER_SIZE] = '\0';

	if (startMatlabAtInitialization)
	{
		_engine = engOpen(NULL);

		// tell Matlab where to store the output
		engOutputBuffer(_engine, _outputBuffer, OUTPUT_BUFFER_SIZE);
	}
}

#ifdef UNIX
Engine::Engine(std::string hostename)
{
	// it would be possible to run Matlab on a different Machine under Linux
	// due to time constraints and lack of need, this is not implemnted yet
	throw std::runtime_error("Not implemented!");
}
#endif

Engine::~Engine()
{
	if (_engine!=NULL)
	{
		int success = engClose(_engine);
		if(success != 0) throw(std::runtime_error("Closing Matlab was not possible. Maybe already closed."));
	}
}

bool Engine::initialize()
{
	if (isInitialized())
		return true;

	_engine = engOpen(NULL);

	// tell Matlab where to store the output
	engOutputBuffer(_engine, _outputBuffer, OUTPUT_BUFFER_SIZE);

	return (_engine!=NULL);
}

bool Engine::stop()
{
	bool success = true;
	if (_engine != NULL)
	{
		success = (engClose(_engine) == 0);
		_engine = NULL;
	}
	return success;
}

bool Engine::isInitialized()
{
	return (_engine != NULL);
}

bool Engine::good()
{
	if (!isInitialized())
		return false;

	bool success = false;
	try {
		success = executeCommand("disp('sm::Matlab Engine-Test')") == ">> sm::Matlab Engine-Test";
	}
	catch (...)
	{
		return false;
	}
	return success;
}

std::string Engine::executeCommand(const std::string& command)
{
	assertIsInitialized();

	// execute the command
	int success = engEvalString(_engine, command.c_str());
	// check for failures
	assert(success == 0 && "Failed to execute command. Maybe Matlab is already closed. Note: This assert is NOT thrown due to invalid Matlab syntax");
	// return the result
	std::string output(_outputBuffer);
	// remove line break
	if(output.size()>0)
		output.resize(output.size() - 1);

	return output;
}

std::string Engine::showWorkspace()
{
	return executeCommand("workspace");
}

std::string Engine::changeWorkingDirectory(const std::string& directory)
{
	return executeCommand("cd ('" + directory + "');");
};

void Engine::openCommandLine()
{
	assertIsInitialized();
	// Display info
	std::cout<<"Enter a MATLAB command to evaluate."<<std::endl;
	std::cout<<"Type 'quit' or 'exit' to leave command line mode"<<std::endl;

	// allow the user to enter multiple (an infinite number) commands
	while (true)
	{
		// Prepare input line
		std::cout<<">> ";

		// Read from console
		if (!fgets(_inputBuffer, INPUT_BUFFER_SIZE, stdin))
			break;

		// Convert to string
		std::string command(_inputBuffer);
		// remove line break
		command.resize(command.size() - 1);
		
		// Check if user wants to exit
		// this intentionally blocks the MATLAB commands "exit" and "quit"
		if (command == "quit" || command == "exit")
			return;

		// execute and display command
		std::cout<<executeCommand(command);
	}
}

// TESTERS
// *******

  bool Engine::exists(const std::string& name)
  {
	assertIsInitialized();
	helpers::assertValidVariableName(name);
	// Get variable from matlab
	mxArray* mxArray = engGetVariable(_engine, name.c_str());
	// Check if variable exists
	return (mxArray!=NULL);
  }

  bool Engine::isScalar(const std::string& name)
  {
	assertIsInitialized();
	helpers::assertValidVariableName(name);
	// Get variable from matlab
	mxArray* mxArray = engGetVariable(_engine, name.c_str());
	// Check if variable exists
	if (mxArray==NULL) { std::cout<<"Variable "+name+" does not exist."<<std::endl; throw; }

	return mxGetNumberOfElements(mxArray) == 1;
  }

  bool Engine::isEmpty(const std::string& name)
  {
	assertIsInitialized();
	helpers::assertValidVariableName(name);
	// Get variable from matlab
	mxArray* mxArray = engGetVariable(_engine, name.c_str());
	// Check if variable exists
	if (mxArray==NULL) { std::cout<<"Variable "+name+" does not exist."<<std::endl; throw; }

	return mxIsEmpty(mxArray) == 1;
  }

  bool Engine::isCharOrString(const std::string& name)
  {
	assertIsInitialized();
	helpers::assertValidVariableName(name);
	// Get variable from matlab
	mxArray* mxArray = engGetVariable(_engine, name.c_str());
	// Check if variable exists
	if (mxArray==NULL) { std::cout<<"Variable "+name+" does not exist."<<std::endl; throw; }

	return mxIsChar(mxArray) == 1;
  }

  bool Engine::getDimensions(const std::string& name, size_t& rows, size_t& cols)
  {
	assertIsInitialized();
	helpers::assertValidVariableName(name);
	// Get variable from matlab
	mxArray* mxArray = engGetVariable(_engine, name.c_str());
	// Check if variable exists
	if(!mxArray)
		return false;

	rows = mxGetM(mxArray);
	cols = mxGetN(mxArray);

	return true;
  }

void Engine::assertIsInitialized() const
{
	if(_engine == NULL) throw std::runtime_error("Matlab Engine is not initialized");
}


} // namespace matlab
