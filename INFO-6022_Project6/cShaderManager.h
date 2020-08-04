#ifndef _cShaderManager_HG_
#define _cShaderManager_HG_

#include <string>
#include <vector>
#include <map>

class cShaderManager
{
public:
	class cShader
	{
	public:
		cShader();
		~cShader();

		enum eShaderType
		{
			VERTEX_SHADER,
			FRAGMENT_SHADER,
			UNKNOWN
		};

		eShaderType shaderType;

		std::string getShaderTypeString(void);

		unsigned int ID;
		std::vector<std::string> vecSource;		
		std::string fileName;
	};

	class cShaderProgram
	{
	public:
		cShaderProgram() : ID(0) {};
		~cShaderProgram() {};
		unsigned int ID;
		std::string friendlyName;
	};

	cShaderManager();
	~cShaderManager();

	bool createProgramFromFile(std::string friendlyName, cShader &vertexShad, cShader &fragShader);
	unsigned int getIDFromFriendlyName(std::string friendlyName);
	std::string getLastError(void);

private:
	
	std::string m_basepath;
	bool m_loadSourceFromFile(cShader &shader);
	bool m_compileShaderFromSource(cShader &shader, std::string &error);	
	bool m_wasThereACompileError(unsigned int shaderID, std::string &errorText);
	bool m_wasThereALinkError(unsigned int progID, std::string &errorText);

	std::string m_lastError;

	std::map< unsigned int /*ID*/, cShaderProgram > m_ID_to_Shader;
	std::map< std::string /*name*/, unsigned int /*ID*/ > m_name_to_ID;
};

#endif