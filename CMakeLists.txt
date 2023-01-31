cmake_minimum_required(VERSION 3.17)
project(vehicle-dynamics-sim)

SET(APP_EXE VehicleDynamicsSim)

ADD_EXECUTABLE(${APP_EXE}
    src/AllCameras.cpp
    src/CameraSystem.cpp
    src/Car.cpp
    src/ControlSystem.cpp
    src/DebugCarModel.cpp
    src/DebugVectorGroup.cpp
    src/Environment.cpp
    src/EnvironmentModel.cpp
    src/GameCarModel.cpp
    src/ICarModel.cpp
    src/main.cpp
    src/PacejkaMagicFormula.cpp
    src/Terrain.cpp
    src/TerrainModel.cpp
    src/test_main.cpp
    src/Track.cpp
    src/Tyre.cpp
    src/UILayer.cpp
    src/VehicleSimulation.cpp
    src/VisualShell.cpp
    src/Wheel.cpp
    src/WheelInterface.cpp
    src/WheelSystem.cpp)

target_include_directories(${APP_EXE} PUBLIC
    include/)

# todo: link to game framework
#TARGET_LINK_LIBRARIES(${APP_EXE} game-framework-graphics)
