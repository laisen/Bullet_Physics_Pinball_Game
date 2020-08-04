#include "cCommandGroup.h"
#include <iostream>

cCommandGroup::cCommandGroup()
{
	this->m_CurrentSerialIndex = 0;
	this->m_LoopTimes = 1;
	return;
}



void cCommandGroup::Update(double deltaTime)
{
	// For serial commands, run the "next" command until it's done.
	//if ( ! this->vecSerial.empty() )
	//{
	//	iCommand* pCurrent = *this->vecSerial.begin();
	//	if ( !pCurrent->IsDone())
	//	{
	//		pCurrent->Update(deltaTime);
	//	}
	//	//else
	//	//{
	//	//	delete pCurrent;
	//	//	// remove this command from the vector
	//	//	this->vecSerial.erase( this->vecSerial.begin() );
	//	//}
	//}

	if (this->m_LoopTimes == 0)
	{
		return;		// Loop over
	}

	if (this->vecSerial.begin() + this->m_CurrentSerialIndex != this->vecSerial.end())
	{
		iCommand* pCurrent = *(this->vecSerial.begin() + this->m_CurrentSerialIndex);
		if (!pCurrent->IsDone())
		{
			pCurrent->Update(deltaTime);
		}
		else
		{
			if (this->m_CurrentSerialIndex != vecSerial.size())
			{
				this->m_CurrentSerialIndex++;
			}			
		}		
	}
	
	for ( std::vector< iCommand* >::iterator itCommand = this->vecParallel.begin(); 
		  itCommand != this->vecParallel.end(); itCommand++ )
	{		
		iCommand* pCurrent = *itCommand;

		if ( !pCurrent->IsDone() )
		{
			pCurrent->Update(deltaTime);
		}		
	}

	if (IsDone())
	{
		if (this->m_LoopTimes >= 1)
		{
			this->m_LoopTimes--;
			Reset();		// Make it repeatable
		}
		else if (this->m_LoopTimes == -1/*infinite loop code*/)
		{
			Reset();
		}
	}
	

	return;
}

bool cCommandGroup::IsDone(void)
{
	bool bAllDone = true; 

	// Serial commands
	for (std::vector< iCommand* >::iterator itCommand = this->vecSerial.begin();
		 itCommand != this->vecSerial.end(); itCommand++)
	{
		iCommand* pCurrent = *itCommand;

		if ( !pCurrent->IsDone() )
		{
			bAllDone = false;
		}
	}

	// Parallel commands
	for (std::vector< iCommand* >::iterator itCommand = this->vecParallel.begin();
		 itCommand != this->vecParallel.end(); itCommand++)
	{
		iCommand* pCurrent = *itCommand;

		if (!pCurrent->IsDone())
		{
			bAllDone = false;
		}
	}

	return bAllDone;
}

void cCommandGroup::Reset(void)
{
	this->m_CurrentSerialIndex = 0;

	// Serial commands
	for (std::vector< iCommand* >::iterator itCommand = this->vecSerial.begin();
		itCommand != this->vecSerial.end(); itCommand++)
	{
		iCommand* pCurrent = *itCommand;

		pCurrent->Reset();
	}

	// Parallel commands
	for (std::vector< iCommand* >::iterator itCommand = this->vecParallel.begin();
		itCommand != this->vecParallel.end(); itCommand++)
	{
		iCommand* pCurrent = *itCommand;

		pCurrent->Reset();
	}
}

void cCommandGroup::AddCommandSerial(iCommand* pCommand)
{
	this->vecSerial.push_back(pCommand);
	return;
}

void cCommandGroup::AddCommandParallel(iCommand* pCommand)
{	
	this->vecParallel.push_back(pCommand);
	return;
}