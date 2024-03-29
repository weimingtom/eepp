#include <eepp/ee.hpp>

cWindow * win			= NULL;
cVertexBuffer * VBO		= NULL;
cVertexBuffer * VBO2	= NULL;
cFrameBuffer * FBO		= NULL;

// The batch renderer class is designed to take control of almost all the rendering needed by the engine.
// Controls that the rendering is only done when is needed, preventing redundant OpenGL API calls
// Usually the user will not need to use this class manually, since eepp controls this internally.
// The engine uses the singleton class cGlobalBatchRenderer instance to render textures and primitives.
cBatchRenderer * Batch = eeNew( cBatchRenderer, () );

eeFloat ang = 0, scale = 1;
bool side = false;

void MainLoop()
{
	// Clear the screen buffer
	win->Clear();

	// Update the input
	win->GetInput()->Update();

	// Check if ESCAPE key is pressed
	if ( win->GetInput()->IsKeyDown( KEY_ESCAPE ) ) {
		// Close the window
		win->Close();
	}

	// Bind the Frame Buffer, everything rendered from here will be rendered in the frame buffer
	FBO->Bind();
	{
		// Bind the buffered data ( activate the buffer )
		VBO->Bind();

		// Draw the buffered data
		VBO->Draw();

		// Unbind the buffered data
		VBO->Unbind();

		// Same as above
		VBO2->Bind();
		VBO2->Draw();
		VBO2->Unbind();
	}
	// Unbind the frame buffer. Stops rendering to the frame buffer
	FBO->Unbind();

	// Draw the frame buffer many times
	for ( int y = 0; y < 5; y++ ) {
		for ( int x = 0; x < 5; x++ ) {
			FBO->GetTexture()->Draw( x * 200, y * 200, -ang, eeVector2f::One, eeColorA(255,255,255,100) );
		}
	}

	eeFloat HWidth	= win->GetWidth() * 0.5f;
	eeFloat HHeight	= win->GetHeight() * 0.5f;

	// The batch can be rotated, scale and moved
	Batch->BatchRotation( ang );
	Batch->BatchScale( scale );
	Batch->BatchCenter( eeVector2f( HWidth, HHeight ) );

	// Create a quad to render
	eeFloat aX = HWidth - 256.f;
	eeFloat aY = HHeight - 256.f;
	eeQuad2f TmpQuad(
		eeVector2f( aX	   , aY 		),
		eeVector2f( aX	   , aY + 32.f  ),
		eeVector2f( aX + 32.f, aY + 32.f  ),
		eeVector2f( aX + 32.f, aY 		)
	);
	TmpQuad.Rotate( ang, eeVector2f( aX + 16.f, aY + 16.f ) );

	// Begin drawing quads
	Batch->QuadsBegin();

	// Add some quads to the batch renderer
	for ( Uint32 z = 0; z < 16; z++ ) {
		for ( Uint32 y = 0; y < 16; y++ ) {
			eeFloat tmpx = (eeFloat)z * 32.f;
			eeFloat tmpy = (eeFloat)y * 32.f;

			// Add the quad to the batch
			Batch->QuadsSetColor( eeColorA( z * 16, 255, 255, 150 ) );
			Batch->BatchQuadFree( TmpQuad[0].x + tmpx, TmpQuad[0].y + tmpy, TmpQuad[1].x + tmpx, TmpQuad[1].y + tmpy, TmpQuad[2].x + tmpx, TmpQuad[2].y + tmpy, TmpQuad[3].x + tmpx, TmpQuad[3].y + tmpy );
		}
	}

	// Draw the batched quads
	Batch->Draw();

	// Add the rotation angle
	ang+=win->Elapsed().AsMilliseconds() * 0.1f;
	ang = (ang>=360) ? 0 : ang;

	// Change the scale value
	if (scale>=1.5f) {
		scale = 1.5f;
		side = true;
	} else if (scale<=0.5f) {
		side = false;
		scale = 0.5f;
	}
	scale = (!side) ? scale+win->Elapsed().AsMilliseconds() * 0.00025f : scale-win->Elapsed().AsMilliseconds() * 0.00025f;

	// Draw frame
	win->Display();
}

EE_MAIN_FUNC int main (int argc, char * argv [])
{
	// Create a new window
	win = cEngine::instance()->CreateWindow( WindowSettings( 1024, 768, "eepp - VBO - FBO and Batch Rendering" ), ContextSettings( true ) );

	// Set window background color
	win->BackColor( eeColor( 50, 50, 50 ) );

	// Check if created
	if ( win->Created() ) {
		eePolygon2f Poly( eePolygon2f::CreateRoundedRectangle( 0, 0, 200, 50 ) );

		// Create the Vertex Buffer, the vertex buffer stores the vertex data in the GPU, making the rendering much faster
		// In the case that Vertex Buffer Object is not supported by the GPU, it will fallback to a inmediate-mode vertex buffer
		VBO		= cVertexBuffer::New( VERTEX_FLAGS_PRIMITIVE, DM_TRIANGLE_FAN );
		VBO2	= cVertexBuffer::New( VERTEX_FLAGS_PRIMITIVE, DM_TRIANGLE_FAN );

		// Add the vertex and vertex colors to the Vertex Buffer
		if ( NULL != VBO && NULL != VBO2 ) {
			for ( Uint32 i = 0; i < Poly.Size(); i++ ) {
				VBO->AddVertex( Poly[i] );
				VBO->AddColor( eeColorA( 100 + i, 255 - i, 150 + i, 100 ) );
			}

			Poly.Rotate( 90, Poly.ToAABB().Center() );

			for ( Uint32 i = 0; i < Poly.Size(); i++ ) {
				VBO2->AddVertex( Poly[i] );
				VBO2->AddColor( eeColorA( 100 + i, 255 - i, 150 + i, 100 ) );
			}

			// Compile the Vertex Buffer, this uploads the data to the GPU
			VBO->Compile();
			VBO2->Compile();
		}

		// Create a new frame buffer. It will use Framebuffer Objects if available, otherwise it will try to fallback to PBuffers.
		FBO = cFrameBuffer::New( 200, 200 );

		// Application loop
		win->RunMainLoop( &MainLoop );

		// Release the allocated objects ( VBOs and FBOs need to be released manually )
		eeSAFE_DELETE( VBO );
		eeSAFE_DELETE( VBO2 );
		eeSAFE_DELETE( FBO );
		eeSAFE_DELETE( Batch );
	}

	// Destroy the engine instance. Destroys all the windows and engine singletons.
	cEngine::DestroySingleton();

	// If was compiled in debug mode it will print the memory manager report
	MemoryManager::ShowResults();

	return EXIT_SUCCESS;
}
