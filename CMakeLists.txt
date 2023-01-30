project(vehicle-dynamics-sim)

SET(APP_EXE VehicleDynamicsSim)

ADD_EXECUTABLE(${APP_EXE}
    src/test_main.cpp)

# todo: link to game framework
#TARGET_LINK_LIBRARIES(${APP_EXE} game-framework-graphics)