#include "GameCarModel.h"
#include "Car.h"

namespace Visual {

	GameCarModel::GameCarModel(Internal::Car& carData, Framework::ResourceSet& resourceBucket) :
		/* Called by VisualShell::load
		*/
		ICarModel(carData, resourceBucket)
	{
		loadResources();
	}

	void GameCarModel::loadResources() 
		/* Called by GameCarModel::GameCarModel
		 * All graphical resources such as meshes, shaders etc are instantiated here
		*/
	{
		//Add all meshes, textures, shaders belonging to the game model of the car should be added in here
		using namespace Framework::Graphics;
		using namespace glm;

		Shader* bodyShader = mResourceBucket.addShader("res/shaders/body.vert", "res/shaders/body.frag", "bodyShader");
		bodyShader->addUniform("modelMatrix");
		bodyShader->addUniform("viewMatrix");
		bodyShader->addUniform("projectionMatrix");
		bodyShader->addUniform("skyColour");
		bodyShader->addUniform("sunDirection");
		bodyShader->addUniform("fogDensity");
		bodyShader->addUniform("fogGradient");
		bodyShader->addUniformWithDefault("bodyColour",  vec3(0.1f, 0.2f, 0.5f));

		Mesh* bodyMesh = mResourceBucket.addOBJMesh("carBody", "res/models/CarBody.obj", GL_TRIANGLES, nullptr, bodyShader);

		//Chassis
		mModel.addMesh(bodyMesh);

		//Wheels interfaces
		{
			Mesh* tempMesh = nullptr;
			for (unsigned char i = 0; i < mCarData.getWheelSystem().getAllWheelInterfaces().size(); i++) {
				//Wheels
				tempMesh = mResourceBucket.addOBJMesh("wheel" + std::to_string(i), "res/models/Wheel.obj", GL_TRIANGLES, nullptr, bodyShader);
				mModel.addMesh(tempMesh);

				//Suspension springs
				tempMesh = mResourceBucket.addOBJMesh("spring" + std::to_string(i), "res/models/Spring.obj", GL_TRIANGLES, nullptr, bodyShader);
				mModel.addMesh(tempMesh);
			}
		}

		//Steering wheel
		mModel.addMesh(mResourceBucket.addOBJMesh("steeringWheel", "res/models/SteeringWheel.obj", GL_TRIANGLES, nullptr, bodyShader));
	}

	void GameCarModel::setShaderUniforms(float fogDensity, float fogGradient, glm::vec3 skyColour, glm::vec3 sunDirection) 
		/* Called by VisualShell::load
		 * Called once at load-time to give shader uniforms initial values
		*/
	{
		Framework::Graphics::Shader* bodyShader = mResourceBucket.getResource<Framework::Graphics::Shader>("bodyShader");

		bodyShader->bind();
		bodyShader->setUniform(3, skyColour);
		bodyShader->setUniform(4, sunDirection);
		bodyShader->setUniform(5, fogDensity);
		bodyShader->setUniform(6, fogGradient);
	}

	void GameCarModel::update()
		/* Called by
		 * Updates the transformation matrices of all meshes belonging to the Model
		*/
	{
		using namespace glm;

		unsigned char meshCount = 0;

		//Chassis
		mat4 carToWorld_car = mCarData.getState().getLocalToWorld_position();
		mModel.getMesh(meshCount)->setWorldTransform(carToWorld_car);
		meshCount++;

		//Wheels
		{
			mat4
				totalTransform,
				translation,
				rotation,
				steeringAngleRot,
				axleRot;

			Internal::Wheel* currentWheel = nullptr;

			vec3 displacement;

			Internal::WheelSystem& wheelSystem = mCarData.getWheelSystem();
			for (unsigned char i = 0; i < wheelSystem.getAllWheelInterfaces().size(); i++) {
				currentWheel = &wheelSystem[i]->getWheel();

				//Wheels
				{
					displacement = currentWheel->getPosition_car();

					translation = translate(displacement),
						steeringAngleRot = rotate((float)radians(currentWheel->getSteeringAngle()), vec3(0.0f, 1.0f, 0.0f)),
						axleRot = rotate((float)currentWheel->getAngularPosition(), vec3(1.0f, 0.0f, 0.0f)),
						rotation = translate(steeringAngleRot * axleRot, displacement) * translate(mat4(), -displacement);

					totalTransform = carToWorld_car * translation * rotation;
					mModel.getMesh(meshCount)->setWorldTransform(totalTransform);
					meshCount++;
				}

				//Suspension springs
				{
					const double
						modelHeight = 0.39983,
						springLength = wheelSystem[i]->getSuspension().getLength();

					translation = translate(vec3(displacement.x * 0.8f, displacement.y, displacement.z));
					totalTransform = carToWorld_car * translation * scale(vec3(1.0f, (springLength + 0.55f) / modelHeight, 1.0f));
					mModel.getMesh(meshCount)->setWorldTransform(totalTransform);
					meshCount++;
				}
			}
		}

		//Steering wheel
		{
			mat4
				translation_car = translate(vec3(0.5f, 0.523f, -0.817f)),
				steeringRotation_car = rotate((float)radians(-mCarData.getControlSystem().getSteeringWheelAngle()), vec3(0.0f, 1.0f, 0.0f)),
				pitchRotation_car = rotate((float)radians(60.0f), vec3(1.0f, 0.0f, 0.0f));

			mModel.getMesh(meshCount)->setWorldTransform(carToWorld_car * translation_car * pitchRotation_car * steeringRotation_car);
		}
	}

}