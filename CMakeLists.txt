cmake_minimum_required(VERSION 3.12)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(bm25_util VERSION ${PROJECT_VESRION})

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS tokenizer)
find_package(Boost COMPONENTS algorithm)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# Добавление исходных файлов
set(SOURCES
        main.cpp
        fixew_priority_queue.h BM25.cpp BM25.h FileReader.cpp FileReader.h IndexIO.cpp IndexIO.h IndexCommon.h)

# Добавление исполняемого файла
add_executable(bm25_util ${SOURCES})

if (MSVC)
    target_compile_options(bm25_util PRIVATE
            /W4
            )
else ()

    target_compile_options(bm25_util PRIVATE
            -Wall -Wextra -pedantic -Werror
            )

endif()



install(TARGETS bm25_util RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)


install(TARGETS bm25_util RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT puzanova679@gmail.com)

include(CPack)