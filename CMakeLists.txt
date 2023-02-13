cmake_minimum_required(VERSION 3.17)
project(vehicle-dynamics-sim)

set(LIB VehicleDynamicsSimLibrary)

add_library(${LIB}
    src/add_function.cpp
)

#add_executable(${APP_EXE}
#    src/AllCameras.cpp
#    src/CameraSystem.cpp
#    src/Car.cpp
#    src/ControlSystem.cpp
#    src/DebugCarModel.cpp
#    src/DebugVectorGroup.cpp
#    src/Environment.cpp
#    src/EnvironmentModel.cpp
#    src/GameCarModel.cpp
#    src/ICarModel.cpp
#    src/main.cpp
#    src/PacejkaMagicFormula.cpp
#    src/Terrain.cpp
#    src/TerrainModel.cpp
#    src/test_main.cpp
#    src/Track.cpp
#    src/Tyre.cpp
#    src/UILayer.cpp
#    src/VehicleSimulation.cpp
#    src/VisualShell.cpp
#    src/Wheel.cpp
#    src/WheelInterface.cpp
#    src/WheelSystem.cpp)

target_include_directories(${LIB} PRIVATE include/)

# todo: link to game framework
#TARGET_LINK_LIBRARIES(${APP_EXE} game-framework-graphics)

set(APP VehicleDynamicsSim)

add_executable(${APP}
    src/dummy_main.cpp
)

target_include_directories(${APP} PRIVATE include/)
target_link_libraries(${APP} PRIVATE ${LIB})