#include <MulticastSyncApp.h>

using namespace ci;
using namespace ci::app;

using namespace MulticastSync;

MulticastSyncApp::MulticastSyncApp()
	: Role(NetworkRole::NetworkRoleMaster)
	, Sync()
	, Value(0.f)
	, Framerate(0.f)
	, Playback(false)
	, FrameCounter(0)
	, FramerateCounter(0)
{
}

void MulticastSyncApp::launch() 
{
	auto& args = getCommandLineArgs();
	bool server = std::find(args.begin(), args.end(), "--server") != args.end();
	Role = server ? NetworkRole::NetworkRoleMaster : NetworkRole::NetworkRoleClient;

	try
	{
		Sync = std::make_shared<LightSync>(server, App::get()->io_context());

		auto title = getWindow()->getTitle();
		getWindow()->setTitle(title + (server ? " [server]" : " [client]"));

		App::launch();
	}
	catch (const std::exception& e)
	{
		(void)e;

		if (server)
			assert(!"Server instance is already running");
		else
		{
			assert(!"Unexpected asio exception");
		}
	}
}

void MulticastSyncApp::mouseDrag(MouseEvent event) 
{
	if (Role != NetworkRole::NetworkRoleMaster)
		return;

	// Store the current mouse position in the list.
	Value = event.getPos().x / float(getWindowWidth());
	Value = glm::clamp(Value, 0.f, 1.f);

	Sync->SendState(Playback, Value);
}

void MulticastSyncApp::keyDown(KeyEvent event) 
{
	if (Role != NetworkRole::NetworkRoleMaster)
		return;

	if (event.getCode() == KeyEvent::KEY_SPACE)
	{
		Playback = !Playback;
		Sync->SendState(Playback, Value);
	}
}

void MulticastSyncApp::setup() 
{ 
	// Before application starts its update/draw calls, 
	// setup is called
	FramerateCounter = clock();
}

void MulticastSyncApp::update() 
{
	const float tick = .001f;

	if (clock() - FramerateCounter > CLOCKS_PER_SEC) 
	{
		Framerate = float(FrameCounter);
		FramerateCounter = clock();
		FrameCounter = 0;
	} 
	else
		FrameCounter++;

	if (Role != NetworkRole::NetworkRoleMaster)
	{
		bool p;
		float v;
		Sync->GetState(p, v);

		// On C's side track M's playback state.If playback doesn't match, switch playback and adjust value to
		// M's. Once C switches to playback mode, it runs on its own (update function increases its value by 1
		// tick – 0.001). Make sure the difference between both values is not larger than 1 tick. If it is, adjust
		// C's value. 

		if (p != Playback)
		{
			Playback = p;
			Value = v;
		}
		else if (!Playback)
		{ 
			Value = v;
		}
		else
		{
			if (std::abs(Value - v) > tick)
				Value = v;
		}
	}

	if (Playback) 
	{
		Value += tick;
		if (Value >= 1.f)
			Value = 0.f;

		if (Role == NetworkRole::NetworkRoleMaster)
			Sync->SendState(Playback, Value);
	}
}

void MulticastSyncApp::draw() 
{
	// Clear the contents of the window. This call will clear
	// both the color and depth buffers. 
	gl::clear(Color::gray(0.25f));

	// Set the current draw color to orange by setting values for
	// red, green and blue directly. Values range from 0 to 1.

	const float halfThickness = 4.f;
	float x = float(getWindowWidth()) * Value;
	float y = float(getWindowHeight());

	{
		gl::ScopedColor color(ci::Color::gray(.5f));
		gl::drawSolidRect(ci::Rectf(glm::vec2(x - halfThickness, 0.f), glm::vec2(x + halfThickness, y)));
	}
	{
		gl::ScopedColor color(ci::Color::gray(.75f));
		gl::drawStrokedRect(ci::Rectf(glm::vec2(x - halfThickness, 0.f), glm::vec2(x + halfThickness, y)), halfThickness * .5f);
	}
	
	gl::drawString("FPS: " + std::to_string(Framerate), glm::vec2(50.f));
	gl::drawString("Value: " + std::to_string(Value), glm::vec2(50.f, 100.f), Playback ? ci::Color(0.f, 1.f, 0.f) : ci::Color::white());
}

void MulticastSyncApp::PrepareSettings(Settings* settings)
{
	settings->setMultiTouchEnabled(false);
}

// This line tells Cinder to actually create and run the application.
CINDER_APP(MulticastSyncApp, RendererGl, MulticastSyncApp::PrepareSettings)
