 IF(WIN32)
ADD_LIBRARY( Torque_Position_UDP_Simulink SHARED Torque_Position_UDP_Simulink.cpp
)

target_compile_features(Torque_Position_UDP_Simulink PRIVATE cxx_std_17)
TARGET_LINK_LIBRARIES(Torque_Position_UDP_Simulink
	XMLInterpreter
	xmlInput
	FileIO
	${Boost_LIBRARIES}
	XercesC::XercesC
	wsock32
	ws2_32
)



ADD_EXECUTABLE(testUDPPlugin testUDPPlugin.cpp
)

target_compile_features(testUDPPlugin PRIVATE cxx_std_17)
TARGET_LINK_LIBRARIES(testUDPPlugin
	Torque_Position_UDP_Simulink
)

ADD_EXECUTABLE(testServerPosition testServerPosition.cpp
)

TARGET_LINK_LIBRARIES(testServerPosition
	wsock32
	ws2_32
)

ADD_EXECUTABLE(testServerTorqueID testServerTorqueID.cpp
)

TARGET_LINK_LIBRARIES(testServerTorqueID
	wsock32
	ws2_32
)

ADD_EXECUTABLE(testServerTorque testServerTorque.cpp
)

TARGET_LINK_LIBRARIES(testServerTorque
	wsock32
	ws2_32
)

ADD_EXECUTABLE(testServerFibLen testServerFibLen.cpp
)

TARGET_LINK_LIBRARIES(testServerFibLen
	wsock32
	ws2_32
)

ADD_EXECUTABLE(testServerFibVel testServerFibVel.cpp
)

TARGET_LINK_LIBRARIES(testServerFibVel
	wsock32
	ws2_32
)

ADD_EXECUTABLE(testServerMusFor testServerMusFor.cpp
)

TARGET_LINK_LIBRARIES(testServerMusFor
	wsock32
	ws2_32
)
endif()