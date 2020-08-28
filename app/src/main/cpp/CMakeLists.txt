cmake_minimum_required( VERSION 3.4.1 )
add_library( misery-native-lib SHARED misery_native.cpp ECS.cpp Transform.cpp assets.cpp RenderContext.cpp)

add_library( assimp-lib SHARED IMPORTED )
set_target_properties( assimp-lib PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/assimp/${ANDROID_ABI}/libassimpd.so)

add_library( libc++ SHARED IMPORTED )
set_target_properties( libc++ PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/assimp/${ANDROID_ABI}/libc++_shared.so )

include_directories( assimp/include assimp/${ANDROID_ABI} )

target_link_libraries( misery-native-lib assimp-lib libc++ android log GLESv3 )
