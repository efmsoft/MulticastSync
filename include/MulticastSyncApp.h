#pragma once

#include <LightSync.h>

#include <cinder/app/App.h>
#include <cinder/app/RendererGl.h>
#include <cinder/gl/gl.h>

namespace MulticastSync
{
	using namespace ci;
	using namespace ci::app;

	// We'll create a new Cinder Application by deriving from the App class.
	class MulticastSyncApp : public App
	{
	public:
		MulticastSyncApp();

		// This is called from executeLaunch(), subclasses implement to launch the application in a platform-specific manner
		void launch() override;

		// Cinder will call 'mouseDrag' when the user moves the mouse while holding one of its buttons.
		// See also: mouseMove, mouseDown, mouseUp and mouseWheel.
		void mouseDrag(MouseEvent event) override;

		// Cinder will call 'keyDown' when the user presses a key on the keyboard.
		// See also: keyUp.
		void keyDown(KeyEvent event) override;

		// Cinder will call 'update' each time the contents of the window need to be updated.
		void update() override;

		// Cinder will call 'draw' each time the contents of the window need to be redrawn.
		void draw() override;
		void setup() override;

		static void PrepareSettings(Settings* settings);

	private:
		enum class NetworkRole
		{
			NetworkRoleMaster = 0,
			NetworkRoleClient
		};

		NetworkRole Role = NetworkRole::NetworkRoleMaster;
		LightSyncPtr Sync;

		float Value = 0.f;
		float Framerate = 0.f;
		bool Playback = false;

		int FrameCounter = 0;
		time_t FramerateCounter = 0;
	};
}
