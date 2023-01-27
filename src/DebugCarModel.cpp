#include "DebugCarModel.h"
#include "Car.h"

namespace Visual {

	DebugCarModel::DebugCarModel(Internal::Car& carData, Framework::ResourceSet& resourceBucket) :
		/* Called by VisualShell::load
		*/
		ICarModel(carData, resourceBucket)
	{
		addVectorLines();
		loadResources();
	}

	void DebugCarModel::render(Framework::Graphics::Renderer& renderer)
		/* Called by VisualShell::renderAll
		*/
	{
		update();
		mModel.sendRenderCommands(renderer);
	}

	void DebugCarModel::addVectorLines()
		/* Called by DebugCarModel::DebugCarModel
		 * Called once at load-time
		 * Adds vector line objects to the mVectorGroup object
		*/
	{
		using namespace glm;

		//3 vectors for each wheel
		for (unsigned char i = 0; i < mCarData.getWheelSystem().getAllWheelInterfaces().size(); i++) {

			//Wheel-to-ground displacement
			mVectorGroup.addVector(DebugVector(vec3(), vec3(), vec4(1.0f, 1.0f, 0.0f, 1.0f)));

			//Tyre force
			mVectorGroup.addVector(DebugVector(vec3(), vec3(), vec4(0.0f, 1.0f, 1.0f, 1.0f)));

			//Wheel interface velocity (world space)
			mVectorGroup.addVector(DebugVector(vec3(), vec3(), vec4(1.0f, 0.0f, 1.0f, 1.0f)));

			//Terrain normal
			mVectorGroup.addVector(DebugVector(vec3(), vec3(), vec4(0.5f, 0.5f, 1.0f, 1.0f)));
		}

		//Car's angular velocity
		mVectorGroup.addVector(DebugVector(vec3(), vec3(), vec4(0.0f, 0.0f, 0.0f, 1.0f)));

		//Car's linear velocity
		mVectorGroup.addVector(DebugVector(vec3(), vec3(), vec4(0.0f, 0.0f, 0.0f, 1.0f)));

		//Aerodynamic drag force
		mVectorGroup.addVector(DebugVector(vec3(), vec3(), vec4(1.0f, 0.5f, 0.0f, 1.0f)));
	}

	void DebugCarModel::loadResources()
		/* Called by DebugCarModel::DebugCarModel
		 * Called once at load-time
		*/
	{
		using namespace Framework::Graphics;

		Shader* debugShader = mResourceBucket.addShader("res/shaders/debug.vert", "res/shaders/debug.frag", "debugShader");
		debugShader->addUniform("modelMatrix");
		debugShader->addUniform("viewMatrix");
		debugShader->addUniform("projectionMatrix");

		populateWheelColourBufferData();
		carBaseResources();
		carWheelsResources();
		loadVectorLines();
	}

	void DebugCarModel::populateWheelColourBufferData()
		/* Called by DebugCarModel::loadResources
		 * Called once at load-time
		*/
	{
		//Fill one buffer with a red colour...
		for (unsigned int i = 0; i < 2 * mWheelNumSides; i++) {
			mWheelCollidingColourData.push_back(mWheelCollidingColour.x);
			mWheelCollidingColourData.push_back(mWheelCollidingColour.y);
			mWheelCollidingColourData.push_back(mWheelCollidingColour.z);
			mWheelCollidingColourData.push_back(mWheelCollidingColour.a);
		}

		//...and buffer with a green colour
		for (unsigned int i = 0; i < 2 * mWheelNumSides; i++) {
			mWheelNeutralColourData.push_back(mWheelNeutralColour.x);
			mWheelNeutralColourData.push_back(mWheelNeutralColour.y);
			mWheelNeutralColourData.push_back(mWheelNeutralColour.z);
			mWheelNeutralColourData.push_back(mWheelNeutralColour.a);
		}
	}

	void DebugCarModel::carBaseResources()
		/* Called by DebugCarModel::loadResources
		 * Instantiates and initialises a mesh for the rectangular base overlay of the Car
		*/
	{
		using namespace Framework::Graphics;

		//Add an (empty) mesh for the debug model
		Mesh* debugBaseMesh = mResourceBucket.addMesh("debugBaseMesh", GL_LINES, nullptr, mResourceBucket.getResource<Shader>("debugShader"));

		//Create a vertex position buffer and fill with data
		{
			std::vector<float> debugBasePositions;

			glm::dvec3 position;
			for (Internal::WheelInterface& w : mCarData.getWheelSystem().getAllWheelInterfaces()) {
				position = w.getPosition_car();

				debugBasePositions.push_back(position.x);
				debugBasePositions.push_back(position.y);
				debugBasePositions.push_back(position.z);
			}

			mResourceBucket.addVertexBuffer(GL_STATIC_DRAW, VertexFormat(0, 3, GL_FLOAT, false), debugBasePositions, "debugBasePositions");
		}

		//Create a vertex colour buffer and fill with data
		{
			std::vector<float> debugBaseColours = {
				1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 0.0f, 1.0f
			};
			mResourceBucket.addVertexBuffer(GL_STATIC_DRAW, VertexFormat(1, 4, GL_FLOAT, false), debugBaseColours, "debugBaseColours");
		}

		//Create an index buffer and fill with data
		{
			std::vector<unsigned int> debugBaseIndices = {
				0, 1,
				1, 3,
				3, 2,
				2, 0
			};
			mResourceBucket.addIndexBuffer(GL_STATIC_DRAW, debugBaseIndices, "debugBaseIndices");
		}

		//Add buffers to mesh
		debugBaseMesh->addBuffer(mResourceBucket.getResource<VertexBuffer>("debugBasePositions"));
		debugBaseMesh->addBuffer(mResourceBucket.getResource<VertexBuffer>("debugBaseColours"));
		debugBaseMesh->addIndexBuffer(mResourceBucket.getResource<IndexBuffer>("debugBaseIndices"));

		//Add the mesh to the model
		mModel.addMesh(debugBaseMesh);
	}

	void DebugCarModel::carWheelsResources()
		/* Called by DebugCarModel::loadResources
		 * Instantiates and initialises meshes for the car's wheels
		*/
	{
		using namespace Framework::Graphics;

		//Used to give the wheel mesh resources unique ID's
		std::string wheelId = "";
		Internal::Wheel* targetWheel = nullptr;

		//For each wheel on the vehicle...
		for (unsigned char i = 0; i < mCarData.getWheelSystem().getAllWheelInterfaces().size(); i++) {
			targetWheel = &mCarData.getWheelSystem()[i]->getWheel();
			wheelId = "debugWheel" + std::to_string(i);

			//Add an (empty) mesh for the debug model
			Mesh* debugWheelMesh = mResourceBucket.addMesh(wheelId + "mesh", GL_LINES, nullptr, mResourceBucket.getResource<Shader>("debugShader"));

			//Create a vertex position buffer and fill with data
			std::vector<float> debugWheelPositions;
			{
				const float sphereRadius = targetWheel->getTotalRadius();

				glm::vec2
					startPointer = glm::vec2(0.0f, 1.0f),
					currentPointer = startPointer;

				//Procedurally generates the vertex positions of circles by rotating a vector around the circumference,
				//like one of the hands of a clock, starting at 12 o'clock and sampling the position of the end of the hand
				//before adding it to the buffer.

				//z-aligned circle first
				for (unsigned char side = 0; side < mWheelNumSides; side++) {
					currentPointer = glm::rotate(startPointer, glm::radians(side / (float)mWheelNumSides * 360.0f)) * sphereRadius;

					debugWheelPositions.push_back(0.0f);             //x
					debugWheelPositions.push_back(currentPointer.y); //y
					debugWheelPositions.push_back(currentPointer.x); //z
				}

				//reset the pointer back up to 12 o'clock
				currentPointer = startPointer;

				//then x-aligned circle
				for (unsigned char side = 0; side < mWheelNumSides; side++) {
					currentPointer = glm::rotate(startPointer, glm::radians(side / (float)mWheelNumSides * 360.0f)) * sphereRadius;

					debugWheelPositions.push_back(currentPointer.x);             //x
					debugWheelPositions.push_back(currentPointer.y); //y
					debugWheelPositions.push_back(0.0); //z
				}

				mResourceBucket.addVertexBuffer(GL_STATIC_DRAW, VertexFormat(0, 3, GL_FLOAT, false), debugWheelPositions, wheelId + "positions");
			}

			//Create a vertex colour buffer and fill with data
			{
				std::vector<float> debugWheelColours;

				//Run through each vertex on each the circles
				for (unsigned char i = 0; i < debugWheelPositions.size() / 3; i++) {
					//And give it a blue colour
					debugWheelColours.push_back(0.0f); //r
					debugWheelColours.push_back(1.0f); //g
					debugWheelColours.push_back(0.0f); //b
					debugWheelColours.push_back(1.0f); //a
				}

				mResourceBucket.addVertexBuffer(GL_STATIC_DRAW, VertexFormat(1, 4, GL_FLOAT, false), debugWheelColours, wheelId + "colours");
			}

			//Create an index buffer and fill with data
			{
				std::vector<unsigned int> debugWheelIndices;

				const unsigned int numVertices = debugWheelPositions.size() / 3;
				for (unsigned int i = 0; i < numVertices - 1; i++) {
					debugWheelIndices.push_back(i);
					debugWheelIndices.push_back(i + 1);
				}

				debugWheelIndices.push_back(numVertices - 1);
				debugWheelIndices.push_back(0);

				mResourceBucket.addIndexBuffer(GL_STATIC_DRAW, debugWheelIndices, wheelId + "indices");
			}

			//Add buffers to mesh
			debugWheelMesh->addBuffer(mResourceBucket.getResource<VertexBuffer>(wheelId + "positions"));
			debugWheelMesh->addBuffer(mResourceBucket.getResource<VertexBuffer>(wheelId + "colours"));
			debugWheelMesh->addIndexBuffer(mResourceBucket.getResource<IndexBuffer>(wheelId + "indices"));

			//Add the Wheel mesh to the model
			mModel.addMesh(debugWheelMesh);
		}
	}

	void DebugCarModel::loadVectorLines()
		/* Called by DebugCarModel::loadResources
		 * Instantiates and populates the position, colour and index buffer of the vectors
		*/
	{
		using namespace Framework::Graphics;

		Mesh* debugVectorLinesMesh = mResourceBucket.addMesh("debugVectors", GL_LINES, nullptr, mResourceBucket.getResource<Shader>("debugShader"));

		//Create the buffers
		mResourceBucket.addVertexBuffer(GL_STATIC_DRAW, VertexFormat(0, 3, GL_FLOAT, false), mVectorGroup.getPositionBuffer(), "debugVectorPositions");
		mResourceBucket.addVertexBuffer(GL_STATIC_DRAW, VertexFormat(1, 4, GL_FLOAT, false), mVectorGroup.getColourBuffer(), "debugVectorColours");
		mResourceBucket.addIndexBuffer(GL_STATIC_DRAW, mVectorGroup.getIndexBuffer(), "debugVectorIndices");

		//Add buffers to mesh
		debugVectorLinesMesh->addBuffer(mResourceBucket.getResource<VertexBuffer>("debugVectorPositions"));
		debugVectorLinesMesh->addBuffer(mResourceBucket.getResource<VertexBuffer>("debugVectorColours"));
		debugVectorLinesMesh->addIndexBuffer(mResourceBucket.getResource<IndexBuffer>("debugVectorIndices"));

		//Add the finished mesh to the model
		mModel.addMesh(debugVectorLinesMesh);

		//Give the buffers data
		mResourceBucket.getResource<VertexBuffer>("debugVectorPositions")->updateData(mVectorGroup.getPositionBuffer());
		mResourceBucket.getResource<VertexBuffer>("debugVectorColours")->updateData(mVectorGroup.getColourBuffer());
		mResourceBucket.getResource<IndexBuffer>("debugVectorIndices")->updateData(mVectorGroup.getIndexBuffer());
	}

	void DebugCarModel::update()
		/* Called by DebugCarModel::render
		 * Each frame, the colours of specific meshes must be updated to represent a collision
		 * The transforms of these meshes are also updated
		*/
	{
		using namespace glm;

		unsigned char meshCount = 0;

		glm::mat4 carToWorld_car = mCarData.getState().getLocalToWorld_position();

		//The base panel of the car
		{
			//Mesh transforms
			{
				mModel.getMesh(meshCount)->setWorldTransform(carToWorld_car);
				meshCount++;
			}

			//Mesh colours
			std::vector<float> newBaseColours = {
				1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 0.0f, 1.0f
			};

			{
				if (mCarData.getState().getPosition_world().y <= 0.01)
					newBaseColours = {
						0.0f, 1.0f, 0.0f, 1.0f,
						0.0f, 1.0f, 0.0f, 1.0f,
						0.0f, 1.0f, 0.0f, 1.0f,
						0.0f, 1.0f, 0.0f, 1.0f
				};
			}

			mResourceBucket.getResource<Framework::Graphics::VertexBuffer>("debugBaseColours")->updateData(newBaseColours);
		}

		//The wheels on the car
		{
			mat4
				totalWheelTransform,
				translation,
				rotation,
				steeringAngleRot,
				axleRot;

			Internal::Wheel* currentWheel = nullptr;

			vec3 displacement;

			Internal::WheelSystem& wheelSystem = mCarData.getWheelSystem();
			for (unsigned char i = 0; i < wheelSystem.getAllWheelInterfaces().size(); i++) {
				//Mesh transforms
				{
					currentWheel = &wheelSystem[i]->getWheel();
					displacement = currentWheel->getPosition_car();

					translation = translate(mat4(), displacement),
						steeringAngleRot = rotate(mat4(), (float)radians(currentWheel->getSteeringAngle()), vec3(0.0f, 1.0f, 0.0f)),
						axleRot = rotate(mat4(), (float)currentWheel->getAngularPosition(), vec3(1.0f, 0.0f, 0.0f)),

						rotation = translate(steeringAngleRot * axleRot, displacement) * translate(mat4(), -displacement);

					totalWheelTransform = carToWorld_car * translation * rotation;
					mModel.getMesh(i + meshCount)->setWorldTransform(totalWheelTransform);
				}

				//Mesh colours
				{
					mResourceBucket.getResource<Framework::Graphics::VertexBuffer>("debugWheel" + std::to_string(i) + "colours")->updateData(mWheelNeutralColourData);

					if (wheelSystem[i]->collisionRegistered())
						mResourceBucket.getResource<Framework::Graphics::VertexBuffer>("debugWheel" + std::to_string(i) + "colours")->updateData(mWheelCollidingColourData);
				}
			}
		}

		updateVectorLines();
	}

	void DebugCarModel::updateVectorLines()
		/* Called by DebugCarModel::update
		 * The vector models' positions, directions and lengths are updated to represent the vector quantities in the simulation
		*/
	{
		using namespace Framework::Graphics;
		using namespace glm;
		using namespace External;

		Internal::WheelInterface* currentWheelInterface = nullptr;

		dvec3 temp;

		//Used to track the index of the start of the current vector line's data
		unsigned int indexTracker = 0;

		double terrainHeight = 0.0;

		for (unsigned int i = 0; i < mCarData.getWheelSystem().getAllWheelInterfaces().size(); i++) {
			currentWheelInterface = mCarData.getWheelSystem()[i];
			temp = currentWheelInterface->getPosition_world();
			terrainHeight = Environment::mTerrain.getHeight(dvec2(temp.x, temp.z));

			//Wheel-to-ground displacement
			{
				mVectorGroup[i * mNumVectorsPerWheelInterface]->setPosition_world(temp);
				mVectorGroup[i * mNumVectorsPerWheelInterface]->setDirection_world(dvec3(0.0, terrainHeight - temp.y, 0.0));
			}

			//Tyre force
			{
				mVectorGroup[i * mNumVectorsPerWheelInterface + 1]->setPosition_world(dvec3(mCarData.getState().getLocalToWorld_position() * dvec4(mCarData.getWheelSystem()[i]->getWheel().getContactPatchPosition_car(), 1.0)));
				mVectorGroup[i * mNumVectorsPerWheelInterface + 1]->setDirection_world(normalize(mCarData.getWheelSystem()[i]->getWheel().getTyreForce_world()));
			}

			//Wheel interface velocity
			{
				mVectorGroup[i * mNumVectorsPerWheelInterface + 2]->setPosition_world(currentWheelInterface->getPosition_world());
				mVectorGroup[i * mNumVectorsPerWheelInterface + 2]->setDirection_world(currentWheelInterface->getVelocity_world());
			}

			//Terrain normal
			{
				mVectorGroup[i * mNumVectorsPerWheelInterface + 3]->setPosition_world(dvec3(temp.x, terrainHeight, temp.z));
				mVectorGroup[i * mNumVectorsPerWheelInterface + 3]->setDirection_world(Environment::mTerrain.getNormal_world(dvec2(temp.x, temp.z)));
			}

			indexTracker += mNumVectorsPerWheelInterface;
		}

		dvec3 carPosition_world = mCarData.getState().getPosition_world();

		//Car angular velocity
		{
			mVectorGroup[indexTracker]->setPosition_world(carPosition_world);
			mVectorGroup[indexTracker]->setDirection_world(mCarData.getState().getAngularVelocity_world());
			indexTracker++;
		}

		//Car velocity
		{
			mVectorGroup[indexTracker]->setPosition_world(carPosition_world);
			mVectorGroup[indexTracker]->setDirection_world(mCarData.getState().getVelocity_world());
			indexTracker++;
		}

		//Aerodynamic drag
		{
			mVectorGroup[indexTracker]->setPosition_world(carPosition_world);

			dvec3 drag = mCarData.getAeroDrag_world();
			if(length(drag) > 1.0)
				mVectorGroup[indexTracker]->setDirection_world(normalize(drag));
			else
				mVectorGroup[indexTracker]->setDirection_world(drag);

			indexTracker++;
		}

		mVectorGroup.update();

		mResourceBucket.getResource<VertexBuffer>("debugVectorPositions")->updateData(mVectorGroup.getPositionBuffer());
	}

}