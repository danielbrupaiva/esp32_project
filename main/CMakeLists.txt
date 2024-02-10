
file(GLOB SOURCES "src/*.c")
idf_component_register(
        SRCS "main.c" ${SOURCES}
        INCLUDE_DIRS "."
        INCLUDE_DIRS "./include"
)
