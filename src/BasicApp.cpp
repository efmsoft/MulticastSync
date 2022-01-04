#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

// We'll create a new Cinder Application by deriving from the App class.
class BasicApp : public App {

	public:

		// Cinder will call 'mouseDrag' when the user moves the mouse while holding one of its buttons.
		// See also: mouseMove, mouseDown, mouseUp and mouseWheel.
		void mouseDrag( MouseEvent event ) override;

		// Cinder will call 'keyDown' when the user presses a key on the keyboard.
		// See also: keyUp.
		void keyDown( KeyEvent event ) override;

		// Cinder will call 'update' each time the contents of the window need to be updated.
		void update() override;

		// Cinder will call 'draw' each time the contents of the window need to be redrawn.
		void draw() override;
		void setup() override;

		enum NetworkRole_ { 
			NetworkRole_Master = 0,
			NetworkRole_Client
		} role = NetworkRole_::NetworkRole_Master;

	private:

		float value = 0.f;
		float framerate = 0.f;
		bool playback = false;
		
	protected:

		int frameCounter = 0;
		time_t framerateCounter = 0;

};

void prepareSettings( BasicApp::Settings* settings ) {
	settings->setMultiTouchEnabled( false );
}

void BasicApp::mouseDrag( MouseEvent event ) {

	if (role != NetworkRole_Master)
		return;

	// Store the current mouse position in the list.
	value = event.getPos().x / float(getWindowWidth());
	value = glm::clamp(value, 0.f, 1.f);

}

void BasicApp::keyDown( KeyEvent event ) {

	if (role != NetworkRole_Master)
		return;

	if( event.getChar() == 'f' ) {

	} else if( event.getCode() == KeyEvent::KEY_SPACE ) {
		playback = !playback;
	}
}

void BasicApp::setup() { 
	// before application starts its update/draw calls, 
	// setup is called
	framerateCounter = clock();
}

void BasicApp::update() {

	if(clock() - framerateCounter > CLOCKS_PER_SEC) {
		framerate = frameCounter;
		framerateCounter = clock();
		frameCounter = 0;
	} else {
		frameCounter++;
	}

	if(playback) {
		value += .001;
		if (value >= 1.f)
			value = 0.f;
	}

}

void BasicApp::draw() {
	// Clear the contents of the window. This call will clear
	// both the color and depth buffers. 
	gl::clear( Color::gray( 0.25f ) );

	// Set the current draw color to orange by setting values for
	// red, green and blue directly. Values range from 0 to 1.

	const float halfThickness = 4.f;
	float x = float(getWindowWidth()) * value;
	float y = getWindowHeight();

	{
		gl::ScopedColor color(ci::Color::gray(.5f));
		gl::drawSolidRect(ci::Rectf(glm::vec2(x - halfThickness, 0.f), glm::vec2(x + halfThickness, y)));
	}
	{
		gl::ScopedColor color(ci::Color::gray(.75f));
		gl::drawStrokedRect(ci::Rectf(glm::vec2(x - halfThickness, 0.f), glm::vec2(x + halfThickness, y)), halfThickness * .5f);
	}
	
	gl::drawString("FPS: " + std::to_string(framerate), glm::vec2(50.f));
	gl::drawString("Value: " + std::to_string(value), glm::vec2(50.f, 100.f), playback ? ci::Color(0.f, 1.f, 0.f) : ci::Color::white());

}

// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl, prepareSettings )
