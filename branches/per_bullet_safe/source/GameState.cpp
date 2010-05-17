//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameState.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;
//|||||||||||||||||||||||||||||||||||||||||||||||

GameState::GameState()
{
	m_MoveSpeed			= 0.1;
	m_RotateSpeed		= 0.3;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::enter()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering GameState...");
	
	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "GameSceneMgr");
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));		

	m_pCamera = m_pSceneMgr->createCamera("GameCamera");
	m_pCamera->setNearClipDistance(5);
	
	SceneNode *node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode", Vector3(0, 0, 30));
	node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("FakeNode", Vector3(0, 10, 30));
	node = m_pSceneMgr->getSceneNode("FakeNode")->createChildSceneNode("LolNode", Vector3(0, 0, 0));
	node = m_pSceneMgr->getSceneNode("LolNode")->createChildSceneNode("CamNode1", Vector3(0, 0, 25));
    node->attachObject(m_pCamera);

	m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) / 
							  Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));
	
	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	OgreFramework::getSingletonPtr()->m_pKeyboard->setEventCallback(this);
	OgreFramework::getSingletonPtr()->m_pMouse->setEventCallback(this);
	
	OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(m_pSceneMgr);

	OgreFramework::getSingletonPtr()->m_pGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow"); 
    CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow"); 
	const OIS::MouseState state = OgreFramework::getSingletonPtr()->m_pMouse->getMouseState();
	CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition(); 
	CEGUI::System::getSingleton().injectMouseMove(state.X.abs-mousePos.d_x,state.Y.abs-mousePos.d_y);

	m_pMainWnd = CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME");
	m_pChatWnd = CEGUI::WindowManager::getSingleton().getWindow("ChatWnd");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(m_pMainWnd);

	CEGUI::PushButton* pExitButton = (CEGUI::PushButton*)m_pMainWnd->getChild("ExitButton_Game");
	pExitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onExitButtonGame, this));
	
	m_bLMouseDown = m_bRMouseDown = false;
	m_bQuit = false;
	m_bChatMode = false;

	setUnbufferedMode();
	createScene();
}

//|||||||||||||||||||||||||||||||||||||||||||||||


bool GameState::pause()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing GameState...");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);
	OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(0);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::resume()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming GameState...");

	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
	OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(m_pSceneMgr);

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI_GAME"));

	m_bQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::exit()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving GameState...");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);
	OgreFramework::getSingletonPtr()->m_pGUIRenderer->setTargetSceneManager(0);

   delete m_ColWorld;
   delete m_Dispatcher;
   delete m_ColConfig;
	delete m_Broadphase;
	m_pSceneMgr->destroyCamera(m_pCamera);
	if(m_pSceneMgr)
		OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::createScene()
{
	m_pSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	m_pSceneMgr->createLight("Light")->setPosition(5.0, 12.0, 30.0);

	//DotScene
    CDotScene scene;
    scene.parseDotScene("Scene.scene", "General", m_pSceneMgr);
	//Bullet
    btVector3 worldAabbMin(-1000,-1000,-1000);
    btVector3 worldAabbMax(1000,1000,1000);           
    m_Broadphase = new btAxisSweep3(worldAabbMin, worldAabbMax); // broadphase
    m_ColConfig = new btDefaultCollisionConfiguration();
    m_Dispatcher = new btCollisionDispatcher(m_ColConfig); // narrowphase pair-wise checking
    m_constraintSolver = new btSequentialImpulseConstraintSolver();

	m_ColWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase,m_constraintSolver, m_ColConfig);
	registerAllEntitiesAsColliders(m_pSceneMgr, m_ColWorld);
	//Player
    m_Player = new Player(m_pSceneMgr, m_ColWorld);
    m_Player->init();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(m_bChatMode == true)
	{
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectKeyDown(keyEventRef.key);
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectChar(keyEventRef.text);
	}
	

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_TAB))
	{
		m_bChatMode = !m_bChatMode;
		
		if(m_bChatMode)
			setBufferedMode();
		else
			setUnbufferedMode();
		
		return true;
	}

	if(m_bChatMode && OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RETURN) ||
		OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPADENTER))
	{
		CEGUI::Editbox* pChatInputBox = (CEGUI::Editbox*)m_pChatWnd->getChild("ChatInputBox");
		CEGUI::MultiLineEditbox *pChatContentBox = (CEGUI::MultiLineEditbox*)m_pChatWnd->getChild("ChatContentBox");
		pChatContentBox->setText(pChatContentBox->getText() + pChatInputBox->getText() + "\n");
		pChatInputBox->setText("");
		pChatContentBox->setCaratIndex(pChatContentBox->getText().size());
		pChatContentBox->ensureCaratIsVisible();
	}
	
	if(!m_bChatMode || (m_bChatMode && !OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_O)))
		OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyReleased(const OIS::KeyEvent &keyEventRef)
{

	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseMoved(const OIS::MouseEvent &evt)
{
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseWheelChange(evt.state.Z.rel);
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
	
	if(m_bRMouseDown)
	{
		m_pSceneMgr->getSceneNode("FakeNode")->yaw(Degree(evt.state.X.rel * -0.1), Node::TS_LOCAL);
		m_pSceneMgr->getSceneNode("LolNode")->pitch(Degree(evt.state.Y.rel * -0.1), Node::TS_LOCAL);
	}
	
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
    {
        onLeftPressed(evt);
        m_bLMouseDown = true;
		
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonDown(CEGUI::LeftButton);
    } 
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = true;
    } 
	
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
    {
        m_bLMouseDown = false;

		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonUp(CEGUI::LeftButton);
    } 
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = false;
    }
	
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::onLeftPressed(const OIS::MouseEvent &evt)
{

}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::onExitButtonGame(const CEGUI::EventArgs &args)
{
	m_bQuit = true;
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::move()
{
		if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W) ||OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S) ||OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A) || OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D) )
		{	
		Vector3 dirre =  m_pSceneMgr->getSceneNode("CamNode1")->_getDerivedPosition() - m_pSceneMgr->getSceneNode("FakeNode")->_getDerivedPosition();
		dirre.y=0;
		Vector3 src = m_pSceneMgr->getSceneNode("NinjaNode")->getOrientation() * (Vector3::UNIT_Z);
		Ogre::Quaternion quat = src.getRotationTo(dirre);
		m_pSceneMgr->getSceneNode("NinjaNode")->rotate(quat);
		}

		m_pSceneMgr->getSceneNode("FakeNode")->translate(m_TranslateVector, Node::TS_LOCAL);
        //mSceneMgr->getSceneNode("NinjaNode")->translate(mDirection * evt.timeSinceLastFrame, Node::TS_LOCAL);
		Vector3 dirzor = m_TranslateVector;
		m_Player->move(dirzor);
        m_ColWorld->performDiscreteCollisionDetection();
		if (!(m_Player->vardetettvalidmove()))
			m_Player->move(-dirzor);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::getInput()
{
	if(m_bChatMode == false)
	{
		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A))
		{
			m_TranslateVector.x = -m_MoveScale;
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D))
		{
			m_TranslateVector.x = m_MoveScale;
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
		{
			m_TranslateVector.z = -m_MoveScale;
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
		{
			m_TranslateVector.z = m_MoveScale;
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LEFT))
		{
			m_pCamera->yaw(m_RotScale);
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RIGHT))
		{
			m_pCamera->yaw(-m_RotScale);
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_UP))
		{
			m_pCamera->pitch(m_RotScale);
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_DOWN))
		{
			m_pCamera->pitch(-m_RotScale);
		}
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::update(double timeSinceLastFrame)
{
	if(m_bQuit == true)
	{
		this->pushAppState(findByName("MenuState"));
		this->popAppState();
		return;
	}

	m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
	m_RotScale  = m_RotateSpeed * timeSinceLastFrame;
		
	m_TranslateVector = Vector3::ZERO;


	getInput();
	move();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::setBufferedMode()
{
	CEGUI::Editbox* pModeCaption = (CEGUI::Editbox*)m_pMainWnd->getChild("ModeCaption");
	pModeCaption->setText("Buffered Input Mode");

	CEGUI::Editbox* pChatInputBox = (CEGUI::Editbox*)m_pChatWnd->getChild("ChatInputBox");
	pChatInputBox->setText("");
	pChatInputBox->activate();
	pChatInputBox->captureInput();

	CEGUI::MultiLineEditbox* pControlsPanel = (CEGUI::MultiLineEditbox*)m_pMainWnd->getChild("ControlsPanel");
	pControlsPanel->setText("[Tab] - To switch between input modes\n\nAll keys to write in the chat box.\n\nPress [Enter] or [Return] to send message.\n\n[Print] - Take screenshot\n\n[Esc] - Quit to main menu");
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::setUnbufferedMode()
{
	CEGUI::Editbox* pModeCaption = (CEGUI::Editbox*)m_pMainWnd->getChild("ModeCaption");
	pModeCaption->setText("Unuffered Input Mode");

	CEGUI::MultiLineEditbox* pControlsPanel = (CEGUI::MultiLineEditbox*)m_pMainWnd->getChild("ControlsPanel");
	pControlsPanel->setText("[Tab] - To switch between input modes\n\n[W] - Forward\n[S] - Backwards\n[A] - Left\n[D] - Right\n\nPress [Shift] to move faster\n\n[O] - Toggle Overlays\n[Print] - Take screenshot\n\n[Esc] - Quit to main menu");
}

//|||||||||||||||||||||||||||||||||||||||||||||||