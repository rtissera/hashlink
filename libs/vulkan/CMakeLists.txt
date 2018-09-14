if(WIN32)
    set(SDL2_PATH ${INCLUDES_BASE_DIR}/sdl)
endif()
find_package(SDL2 REQUIRED)
find_package(VULKAN REQUIRED)

add_library(vulkan.hdll SHARED
    vulkan.c
)
set_as_hdll(vulkan)
target_include_directories(vulkan.hdll
    PRIVATE
    ${SDL2_INCLUDE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)
target_link_libraries(vulkan.hdll
    libhl
    ${SDL2_LIBRARY}
    ${Vulkan_LIBRARY}
)

if(WIN32)
    target_link_libraries(vulkan.hdll
	vulkan
    )
endif()

if(APPLE)
    target_include_directories(vulkan.hdll
        PRIVATE
        ${Vulkan_INCLUDE_DIRS}
    )
    target_link_libraries(vulkan.hdll
        libhl
	${Vulkan_LIBRARIES}
    )
endif()


install(
    TARGETS vulkan.hdll
    DESTINATION ${HDLL_DESTINATION}
)
