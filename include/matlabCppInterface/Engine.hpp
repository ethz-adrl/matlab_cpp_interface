#ifndef SM_MATLAB_ENGINE_HPP
#define SM_MATLAB_ENGINE_HPP

#include <string>
#include <iostream>

#include <Eigen/Core>

#include <matlabCppInterface/internal/helpers.hpp>
#include <matlabCppInterface/internal/MxArrayWrapper.hpp>
#include <matlabCppInterface/internal/MxArrayNDimWrapper.hpp>

#include <engine.h>

namespace matlab {

// Settings
enum SETTINGS {
	OUTPUT_BUFFER_SIZE = 256,
	INPUT_BUFFER_SIZE = 256
};

///
/// @class Engine
/// @brief a class that wraps the matlab C engine.
/// @todo describe how this wrapper work
///
class Engine
{
public:
  ///
  /// Default constructor.
  ///
  Engine();

  ///
  /// Default constructor.
  ///
  Engine(bool startMatlabAtInitialization);

  ///
  /// Initialization
  ///
  bool initialize();
  bool stop();

  ///
  /// Status check
  ///
  bool isInitialized();
  bool good();

#ifdef UNIX
  ///
  /// Constructor. Opens the Matlab instance on a different host (UNIX only!)
  ///
  /// @param hostename the name of the host on which the client is opened
  ///
  Engine(std::string hostename);
#endif

  ~Engine();

#ifdef WIN32

  ///
  /// The Matlab instance is set to visible/invisible (Windows only!)
  ///
  /// @param visible defines if Matlab is visible
  /// @return true if successful
  ///
  bool setVisibility(bool visible) { return engSetVisible(_engine, static_cast<int>(visible)) == 0; }

  ///
  /// The Matlab instance is set to visible/invisible (Windows only!)
  ///
  /// @return true if successful
  ///
  bool isVisible()
  {
	bool visible;
	int success = engSetVisible(_engine, visible);
	assert(success == 0 && "Determining the visibility was not possible. Matlab error");
	return visible;
  }
#endif

  ///
  /// Executes a command in Matlab
  ///
  /// @param command The command to be executed (in Matlab syntax)
  /// @return the Matlab output of the command
  ///
  std::string executeCommand(const std::string& command);

  ///
  /// Opens Matlab's workspace window
  ///
  /// @return the Matlab output of the command
  ///
  std::string showWorkspace();

  ///
  /// Changes the working directory using the Matlab command "cd"
  ///
  /// @param directory The working directory
  /// @return the Matlab output of the command
  ///
  std::string changeWorkingDirectory(const std::string& directory);

  ///
  /// Provides a command line where matlab commands can be directly typed and executed
  ///
  void openCommandLine();


  // TESTERS

  bool exists(const std::string& name);
  bool isScalar(const std::string& name);
  bool isEmpty(const std::string& name);
  bool isCharOrString(const std::string& name);
  bool getDimensions(const std::string& name, size_t& rows, size_t& cols);


  // SETTERS
  template <typename ValueType>
  bool put(const std::string& name, const ValueType& value);

  template <typename ValueType>
  bool get(const std::string& name, ValueType& rValue);

  template <typename ValueType, typename AllocatorType>
  bool put(const std::string& name, const std::vector<ValueType, AllocatorType>& value);

  template <typename ValueType, typename AllocatorType>
  bool get(const std::string& name, std::vector<ValueType, AllocatorType>& rValue);



private:
  void assertIsInitialized() const;

  /// The handle for the matlab engine
  ::Engine *_engine;

  /// The output buffer in which returns of a command are stored
  char _outputBuffer[OUTPUT_BUFFER_SIZE+1];
  char _inputBuffer[OUTPUT_BUFFER_SIZE+1];
};


template <typename ValueType>
bool Engine::put(const std::string& name, const ValueType& value)
{
	assertIsInitialized();
	helpers::assertValidVariableName(name);

	MxArrayWrapper<ValueType> mxArray(value);

	// send data and verify
	int success = engPutVariable(_engine, name.c_str(), mxArray.mxArrayPtr());
	if (success == 0)
	{
		return true;
	}
	return false;
}

template <typename ValueType>
bool Engine::get(const std::string& name, ValueType& rValue)
{
	assertIsInitialized();
	helpers::assertValidVariableName(name);

	// Get variable from matlab
	MxArrayWrapper<ValueType> mxArrayWrapped;
	mxArrayWrapped.mxArrayPtr() = engGetVariable(_engine, name.c_str());
	if(mxArrayWrapped.mxArrayPtr() == NULL)
	{
		return false;
	}

	mxArrayWrapped.get(rValue);
	return true;
}



template <typename ValueType, typename AllocatorType>
bool Engine::put(const std::string& name, const std::vector<ValueType, AllocatorType>& value)
{
	assertIsInitialized();
	helpers::assertValidVariableName(name);

	MxArrayNDimWrapper<ValueType, AllocatorType> mxArray(value);

	// send data and verify
	int success = engPutVariable(_engine, name.c_str(), mxArray.mxArrayPtr());
	if (success == 0)
	{
		return true;
	}
	return false;
}


template <typename ValueType, typename AllocatorType>
bool Engine::get(const std::string& name, std::vector<ValueType, AllocatorType>& rValue)
{
	assertIsInitialized();
	helpers::assertValidVariableName(name);

	// Get variable from matlab
	MxArrayNDimWrapper<ValueType, AllocatorType> mxArrayNDimWrapped;
	mxArrayNDimWrapped.mxArrayPtr() = engGetVariable(_engine, name.c_str());
	if(mxArrayNDimWrapped.mxArrayPtr() == NULL)
	{
		return false;
	}

	mxArrayNDimWrapped.get(rValue);
	return true;
}


} // namespace matlab
  
#endif /* SM_MATLAB_ENGINE_HPP */
