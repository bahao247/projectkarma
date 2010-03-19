//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_DEMO_HPP
#define OGRE_DEMO_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AdvancedOgreFramework.hpp"
#include "AppStateManager.hpp"

#include "MenuState.hpp"
#include "GameState.hpp"
#include "TestState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

class DemoApp
{
public:
	DemoApp();
	~DemoApp();

	void startDemo();

private:
	AppStateManager*	m_pAppStateManager;
	bool				m_bShutdown;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif 

//|||||||||||||||||||||||||||||||||||||||||||||||