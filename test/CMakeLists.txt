cmake_minimum_required(VERSION 3.13)
project(TejasviniTests C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/Tejasvini_Controller
)

# Shared sources
set(SHARED_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/shared/Types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/shared/ErrorCodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/shared/Protocol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/shared/Serialization.cpp
)

# Controller state machine sources
set(STATE_MACHINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/Tejasvini_Controller/ProfileEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../firmware/Tejasvini_Controller/MachineStateMachine.cpp
)

# 1. Protocol Tests
add_executable(test_protocol
    protocol/test_protocol.cpp
    ${SHARED_SOURCES}
)
add_test(NAME ProtocolTests COMMAND test_protocol)

# 2. Serialization Tests
add_executable(test_serialization
    serialization/test_serialization.cpp
    ${SHARED_SOURCES}
)
add_test(NAME SerializationTests COMMAND test_serialization)

# 3. State Machine Tests
add_executable(test_state_machine
    state-machine/test_state_machine.cpp
    ${SHARED_SOURCES}
    ${STATE_MACHINE_SOURCES}
)
add_test(NAME StateMachineTests COMMAND test_state_machine)

# 4. End-to-End Simulation Tests
add_executable(test_end_to_end_sim
    integration/test_end_to_end_sim.cpp
    ${SHARED_SOURCES}
    ${STATE_MACHINE_SOURCES}
)
add_test(NAME EndToEndSimulationTests COMMAND test_end_to_end_sim)
