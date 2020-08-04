#include "cLuaBrain.h"

#include <iostream>

#include "../cGameObject.h"
#include "../cCommandGroup.h"
#include "../cMoveTo.h"
#include "../cMoveToRelative.h"
#include "../cScaleTo.h"
#include "../cOrientToRelative.h"
#include "../cOrientTo.h"
#include "../cFollowCurve.h"
#include "../cFollowObject.h"
#include "../cTrigger.h"

extern std::map<std::string, iCommand*> map_NameToCommand;
extern std::vector<iCommand*> vec_FollowObjectCommands;
extern std::vector<iCommand*> vec_Triggers;
extern iCommand* pSceneCG;
extern std::map<std::string, iCommand*> map_ParalleledSerialCG;

cLuaBrain::cLuaBrain()
{
	this->m_p_vecGOs = nullptr;

	// Create new Lua state.
	// NOTE: this is common to ALL script in this case
	this->m_pLuaState = luaL_newstate();

	luaL_openlibs(this->m_pLuaState);					/* Lua 5.3.3 */

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_MoveTo);
	lua_setglobal(this->m_pLuaState, "MoveTo");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_MoveToRelative);
	lua_setglobal(this->m_pLuaState, "MoveToRelative");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_OrientTo);
	lua_setglobal(this->m_pLuaState, "OrientTo");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_OrientToRelative);
	lua_setglobal(this->m_pLuaState, "OrientToRelative");
	
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_FollowCurve);
	lua_setglobal(this->m_pLuaState, "FollowCurve");
	
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_FollowObject);
	lua_setglobal(this->m_pLuaState, "FollowObject");
	
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_Trigger);
	lua_setglobal(this->m_pLuaState, "Trigger");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_TriggerAddCommandParallel);
	lua_setglobal(this->m_pLuaState, "TriggerAddCommandParallel");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_AddCommandSerial);
	lua_setglobal(this->m_pLuaState, "AddCommandSerial");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_AddCommandParallel);
	lua_setglobal(this->m_pLuaState, "AddCommandParallel");
	
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_MakeParalleledSerialCG);
	lua_setglobal(this->m_pLuaState, "MakeParalleledSerialCG");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_AddParalleledSerialCG);
	lua_setglobal(this->m_pLuaState, "AddParalleledSerialCG");
	
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_SetLoopTimesPSCG);
	lua_setglobal(this->m_pLuaState, "SetLoopTimesPSCG");
	
	return;
}

cLuaBrain::~cLuaBrain()
{
	lua_close(this->m_pLuaState);
	return;
}


// Saves (and overwrites) any script
// scriptName is just so we can delete them later
void cLuaBrain::LoadScript( std::string scriptName, 
					        std::string scriptSource )
{
	this->m_mapScripts[scriptName] = scriptSource;
	return;
}


void cLuaBrain::DeleteScript( std::string scriptName )
{
	this->m_mapScripts.erase(scriptName);
	return;
}

// Passes a pointer to the game object vector
void cLuaBrain::SetObjectVector( std::vector< cGameObject* >* p_vecGOs )
{
	this->m_p_vecGOs = p_vecGOs;
	return;
}

// Call all the active scripts that are loaded
void cLuaBrain::Update(float deltaTime)
{
//	std::cout << "cLuaBrain::Update() called" << std::endl;
	for( std::map< std::string /*name*/, std::string /*source*/>::iterator itScript = 
		 this->m_mapScripts.begin(); itScript != this->m_mapScripts.end(); itScript++ )
	{

		// Pass the script into Lua and update
//		int error = luaL_loadbuffer(L, buffer, strlen(buffer), "line");

		std::string curLuaScript = itScript->second;

		int error = luaL_loadstring( this->m_pLuaState, 
									 curLuaScript.c_str() );

		if ( error != 0 /*no error*/)	
		{
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			continue;
		}

		// execute funtion in "protected mode", where problems are 
		//  caught and placed on the stack for investigation
		error = lua_pcall(this->m_pLuaState,	/* lua state */
						  0,	/* nargs: number of arguments pushed onto the lua stack */
						  0,	/* nresults: number of results that should be on stack at end*/
						  0);	/* errfunc: location, in stack, of error function. 
								   if 0, results are on top of stack. */
		if ( error != 0 /*no error*/)	
		{
			std::cout << "Lua: There was an error..." << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

			std::string luaError;
			// Get error information from top of stack (-1 is top)
			luaError.append( lua_tostring(this->m_pLuaState, -1) );

			// Make error message a little more clear
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << luaError << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			// We passed zero (0) as errfunc, so error is on stack)
			lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */

			continue;
		}

	}

	return;
}

// Runs a script, but doesn't save it (originally used to set the ObjectID)
void cLuaBrain::RunScriptImmediately(std::string script)
{

	int error = luaL_loadstring( this->m_pLuaState, 
								 script.c_str() );

	if ( error != 0 /*no error*/)	
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return;
	}

	// execute funtion in "protected mode", where problems are 
	//  caught and placed on the stack for investigation
	error = lua_pcall(this->m_pLuaState,	/* lua state */
						0,	/* nargs: number of arguments pushed onto the lua stack */
						0,	/* nresults: number of results that should be on stack at end*/
						0);	/* errfunc: location, in stack, of error function. 
								if 0, results are on top of stack. */
	if ( error != 0 /*no error*/)	
	{
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

		std::string luaError;
		// Get error information from top of stack (-1 is top)
		luaError.append( lua_tostring(this->m_pLuaState, -1) );

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack)
		lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */
	}

	return;
}

int cLuaBrain::l_MoveTo(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	map_NameToCommand[lua_tostring(L, 2)] = new cMoveTo();
	map_NameToCommand[lua_tostring(L, 2)]->SetGameObject(pGO);
	map_NameToCommand[lua_tostring(L, 2)]->setName(lua_tostring(L, 2));
	std::vector<sPair> vecParams;
	vecParams.push_back(sPair(glm::vec3(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5))));
	vecParams.push_back(sPair(glm::vec3(lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8))));
	vecParams.push_back(sPair(glm::vec3(lua_tonumber(L, 9), lua_tonumber(L, 10), lua_tonumber(L, 11))));
	map_NameToCommand[lua_tostring(L, 2)]->Init(vecParams);	

	lua_pushboolean(L, true);	// index is OK

	return 1;
}

int cLuaBrain::l_MoveToRelative(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	map_NameToCommand[lua_tostring(L, 2)] = new cMoveToRelative();
	map_NameToCommand[lua_tostring(L, 2)]->SetGameObject(pGO);
	map_NameToCommand[lua_tostring(L, 2)]->setName(lua_tostring(L, 2));
	std::vector<sPair> vecParams;
	vecParams.push_back( sPair(glm::vec3(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5))) );
	vecParams.push_back( sPair(glm::vec3(lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8))) );
	map_NameToCommand[lua_tostring(L, 2)]->Init(vecParams);

	//std::cout << lua_tostring(L, 2) << std::endl;

	lua_pushboolean(L, true);	// index is OK

	return 1;
}

int cLuaBrain::l_OrientTo(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);

	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	
		lua_pushboolean(L, false);		
		return 1;
	}

	map_NameToCommand[lua_tostring(L, 2)] = new cOrientTo();
	map_NameToCommand[lua_tostring(L, 2)]->SetGameObject(pGO);
	map_NameToCommand[lua_tostring(L, 2)]->setName(lua_tostring(L, 2));
	std::vector<sPair> vecParams;
	vecParams.push_back(sPair(glm::vec3(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5))));
	vecParams.push_back(sPair(lua_tonumber(L, 6)));
	vecParams.push_back(sPair(lua_tonumber(L, 7)));
	map_NameToCommand[lua_tostring(L, 2)]->Init(vecParams);	

	lua_pushboolean(L, true);

	return 1;
}

int cLuaBrain::l_OrientToRelative(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	map_NameToCommand[lua_tostring(L, 2)] = new cOrientToRelative();
	map_NameToCommand[lua_tostring(L, 2)]->SetGameObject(pGO);
	map_NameToCommand[lua_tostring(L, 2)]->setName(lua_tostring(L, 2));
	std::vector<sPair> vecParams;
	vecParams.push_back( sPair(glm::vec3(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5))) );
	vecParams.push_back( sPair(lua_tonumber(L, 6)) );
	vecParams.push_back( sPair(lua_tonumber(L, 7)) );
	map_NameToCommand[lua_tostring(L, 2)]->Init(vecParams);

	//std::cout << lua_tostring(L, 2) << std::endl;

	lua_pushboolean(L, true);	// index is OK

	return 1;
}

int cLuaBrain::l_FollowCurve(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	
		lua_pushboolean(L, false);		
		return 1;
	}

	map_NameToCommand[lua_tostring(L, 2)] = new cFollowCurve();
	map_NameToCommand[lua_tostring(L, 2)]->SetGameObject(pGO);
	map_NameToCommand[lua_tostring(L, 2)]->setName(lua_tostring(L, 2));
	std::vector<sPair> vecParams;
	vecParams.push_back(sPair(glm::vec3(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5))));
	vecParams.push_back(sPair(glm::vec3(lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8))));
	vecParams.push_back(sPair(glm::vec3(lua_tonumber(L, 9), lua_tonumber(L, 10), lua_tonumber(L, 11))));
	vecParams.push_back(sPair(lua_tonumber(L, 12)));
	vecParams.push_back(sPair(lua_tonumber(L, 13)));
	map_NameToCommand[lua_tostring(L, 2)]->Init(vecParams);	
	
	lua_pushboolean(L, true);

	return 1;
}

int cLuaBrain::l_FollowObject(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);
	int targetID = (int)lua_tonumber(L, 3);

	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);
	cGameObject* pTargetGO = cLuaBrain::m_findObjectByID(targetID);

	if (pGO == nullptr || pTargetGO == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	map_NameToCommand[lua_tostring(L, 2)] = new cFollowObject();
	map_NameToCommand[lua_tostring(L, 2)]->SetGameObject(pGO);
	map_NameToCommand[lua_tostring(L, 2)]->setName(lua_tostring(L, 2));
	map_NameToCommand[lua_tostring(L, 2)]->SetTarget(pTargetGO);
	std::vector<sPair> vecParams;
	vecParams.push_back(sPair(glm::vec3(lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6))));
	vecParams.push_back(sPair(lua_tonumber(L, 7)));
	vecParams.push_back(sPair(lua_tonumber(L, 8)));
	vecParams.push_back(sPair(lua_tonumber(L, 9)));	
	map_NameToCommand[lua_tostring(L, 2)]->Init(vecParams);

	if (lua_tonumber(L, 10) == 0)		// Update stand alone from other commands and never be over
	{
		vec_FollowObjectCommands.push_back(map_NameToCommand[lua_tostring(L, 2)]);
	}

	lua_pushboolean(L, true);

	return 1;
}

int cLuaBrain::l_Trigger(lua_State* L)
{
	int objectID = (int)lua_tonumber(L, 1);

	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	map_NameToCommand[lua_tostring(L, 2)] = new cTrigger();
	map_NameToCommand[lua_tostring(L, 2)]->SetGameObject(pGO);
	map_NameToCommand[lua_tostring(L, 2)]->setName(lua_tostring(L, 2));
	std::vector<sPair> vecParams;
	vecParams.push_back(sPair(glm::vec3(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5))));
	vecParams.push_back(sPair(lua_tonumber(L, 6)));
	map_NameToCommand[lua_tostring(L, 2)]->Init(vecParams);
	
	vec_Triggers.push_back(map_NameToCommand[lua_tostring(L, 2)]);

	lua_pushboolean(L, true);

	return 1;
}

int cLuaBrain::l_TriggerAddCommandParallel(lua_State* L)
{
	std::map<std::string, iCommand*>::iterator itTrigger = map_NameToCommand.find(lua_tostring(L, 1));
	if (itTrigger == map_NameToCommand.end())
	{
		lua_pushboolean(L, false);
		return 1;
	}
	else
	{
		std::map<std::string, iCommand*>::iterator itCommand = map_NameToCommand.find(lua_tostring(L, 2));
		if (itCommand == map_NameToCommand.end())
		{
			lua_pushboolean(L, false);
			return 1;
		}
		else
		{
			itTrigger->second->AddCommandParallel(itCommand->second);
			//iCommand* pTrigger = itTrigger->second;
			lua_pushboolean(L, true);
			return 1;
		}
	}	
}

int cLuaBrain::l_AddCommandSerial(lua_State* L)
{
	std::map<std::string, iCommand*>::iterator itCommand = map_NameToCommand.find(lua_tostring(L, 1));
	if (itCommand == map_NameToCommand.end())
	{
		lua_pushboolean(L, false);		
		return 1;
	}
	else
	{
		pSceneCG->AddCommandSerial(itCommand->second);

		lua_pushboolean(L, true);
		return 1;
	}
}

int cLuaBrain::l_AddCommandParallel(lua_State* L)
{

	std::map<std::string, iCommand*>::iterator itCommand = map_NameToCommand.find(lua_tostring(L, 1));
	if (itCommand == map_NameToCommand.end())
	{
		lua_pushboolean(L, false);
		return 1;
	}
	else
	{
		pSceneCG->AddCommandParallel(itCommand->second);

		lua_pushboolean(L, true);
		return 1;
	}
}

int cLuaBrain::l_MakeParalleledSerialCG(lua_State* L)
{
	map_ParalleledSerialCG[lua_tostring(L, 1)] = new cCommandGroup();

	lua_pushboolean(L, true);
	return 1;
}

int cLuaBrain::l_AddParalleledSerialCG(lua_State* L)
{
	std::map<std::string, iCommand*>::iterator itCommand = map_NameToCommand.find(lua_tostring(L, 2));
	if (itCommand == map_NameToCommand.end())
	{
		lua_pushboolean(L, false);
		return 1;
	}
	else
	{	
		std::map<std::string, iCommand*>::iterator itPSCG = map_ParalleledSerialCG.find(lua_tostring(L, 1));
		if (itPSCG == map_ParalleledSerialCG.end())
		{
			lua_pushboolean(L, false);
			return 1;
		}
		else
		{
			map_ParalleledSerialCG[lua_tostring(L, 1)]->AddCommandSerial(itCommand->second);
			lua_pushboolean(L, true);
			return 1;
		}		
	}	
}

int cLuaBrain::l_SetLoopTimesPSCG(lua_State* L)
{
	std::map<std::string, iCommand*>::iterator itPSCG = map_ParalleledSerialCG.find(lua_tostring(L, 1));
	if (itPSCG == map_ParalleledSerialCG.end())
	{
		lua_pushboolean(L, false);
		return 1;
	}
	else
	{
		map_ParalleledSerialCG[lua_tostring(L, 1)]->SetLoopTimes(lua_tonumber(L, 2));
		lua_pushboolean(L, true);
		return 1;
	}
}



/*static*/ 
std::vector< cGameObject* >* cLuaBrain::m_p_vecGOs;

// returns nullptr if not found
/*static*/ cGameObject* cLuaBrain::m_findObjectByID( int ID )
{
	for ( std::vector<cGameObject*>::iterator itGO = cLuaBrain::m_p_vecGOs->begin(); 
		  itGO != cLuaBrain::m_p_vecGOs->end(); itGO++ )
	{
		if ( (*itGO)->getUniqueID() == ID )
		{	// Found it!
			return (*itGO);
		}
	}//for ( std::vector<cGameObject*>::iterator itGO...
	// Didn't find it
	return nullptr;
}

//iCommand* m_findCommandByName(std::string commandName)
//{
//}

std::string cLuaBrain::m_decodeLuaErrorToString( int error )
{
	switch ( error )
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error"; 
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}//switch ( error )

	// Who knows what this error is?
	return "Lua: UNKNOWN error";
}
