#include "Gamestate.h"


//################################################################################################################
//				COMMON FUNCTIONS
////##############################################################################################################
bool Gamestate::getFinished()
{
	return isFinished;
}

En_Gamestate Gamestate::getNextState()
{
	return nextState;
}