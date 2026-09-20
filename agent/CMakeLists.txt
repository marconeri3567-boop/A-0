cmake_minimum_required(VERSION 3.16)

project(LocalIntentAI VERSION 1.0.0 DESCRIPTION "Offline Intent Classifier" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
include(FetchContent)
find_package(Eigen3 QUIET)
if(NOT Eigen3_FOUND)
    FetchContent_Declare(eigen URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz)
    FetchContent_MakeAvailable(eigen)
endif()
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    FetchContent_Declare(nlohmann_json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
    FetchContent_MakeAvailable(nlohmann_json)
endif()
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)
FetchContent_Declare(tiny_dnn GIT_REPOSITORY https://github.com/tiny-dnn/tiny-dnn.git GIT_TAG master GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(tiny_dnn)
set(TINY_DNN_INCLUDE_DIR "${tiny_dnn_SOURCE_DIR}")
add_executable(local_intent_ai
    src/ContextStore.cpp
    src/DatasetConverter.cpp
    src/DatasetLoader.cpp
    src/Vocabulary.cpp
    src/NeuralModel.cpp
    src/Trainer.cpp
    src/Predictor.cpp
    src/ToolDispatcher.cpp
    src/Database.cpp
    src/main.cpp)
target_include_directories(local_intent_ai PRIVATE ${PROJECT_SOURCE_DIR}/include ${TINY_DNN_INCLUDE_DIR})
target_link_libraries(local_intent_ai PRIVATE Eigen3::Eigen SQLite::SQLite3 nlohmann_json::nlohmann_json Threads::Threads)
target_compile_options(local_intent_ai PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
target_compile_definitions(local_intent_ai PRIVATE LOCAL_INTENT_AI_PROJECT_DIR="${PROJECT_SOURCE_DIR}")
set_target_properties(local_intent_ai PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/models")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/data")
enable_testing()
