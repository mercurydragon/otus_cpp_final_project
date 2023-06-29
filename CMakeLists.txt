cmake_minimum_required(VERSION 3.12)
project(BM25Util)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost COMPONENTS tokenizer)
find_package(Boost COMPONENTS algorithm)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# Добавление библиотеки NLTK
add_definitions(-DPYTHON_EXECUTABLE="${PYTHON_EXECUTABLE}")
add_definitions(-DPYTHON_INCLUDE_DIR="${PYTHON_INCLUDE_DIR}")
add_definitions(-DPYTHON_LIBRARY="${PYTHON_LIBRARY}")

# Добавление исходных файлов
set(SOURCES
        main.cpp
        fixew_priority_queue.cpp fixew_priority_queue.h)

# Добавление исполняемого файла
add_executable(bm25_util ${SOURCES})

set_target_properties(bm25_util PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        )

set (CMAKE_CXX_FLAGS "-lstdc++fs")
# Линковка с библиотеками Python и другими необходимыми библиотеками
target_link_libraries(bm25_util ${PYTHON_LIBRARIES})

# Укажите путь к библиотекам CppJieba и nlohmann/json
# set(CPPJIEBA_INCLUDE_DIR "/путь/к/директории/с/файлами/CppJieba")
# set(JSON_INCLUDE_DIR "/путь/к/директории/с/файлами/json")

# Добавление пути к заголовочным файлам CppJieba и nlohmann/json, если необходимо
# include_directories(${CPPJIEBA_INCLUDE_DIR})
# include_directories(${JSON_INCLUDE_DIR})

# Линковка с CppJieba и nlohmann/json, если необходимо
# target_link_libraries(bm25_util ${CPPJIEBA_LIBRARY})
# target_link_libraries(bm25_util ${JSON_LIBRARY})
