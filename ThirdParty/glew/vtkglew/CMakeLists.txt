set(sources
  src/glew.c)

set(headers
  include/GL/glew.h
  include/GL/glxew.h
  include/GL/vtk_glew_mangle.h
  include/GL/wglew.h)

vtk_module_add_module(VTK::glew
  HEADER_DIRECTORIES
  SOURCES ${sources}
  HEADERS ${headers}
  HEADERS_SUBDIR "vtkglew")
target_compile_definitions(glew
  PRIVATE
    GLEW_NO_GLU)


if(VTK_OPENGL_HAS_OSMESA)
  target_compile_definitions(glew
    PRIVATE
      GLEW_OSMESA)
  target_link_libraries(glew
    PRIVATE
      ${CMAKE_DL_LIBS})
endif()

if(VTK_OPENGL_HAS_EGL)
  target_compile_definitions(glew
    PRIVATE
      GLEW_EGL)
endif()

if (BUILD_SHARED_LIBS)
  if (WIN32)
    target_compile_definitions(glew
      PRIVATE
        GLEW_BUILD)
    if(MINGW)
      # https://github.com/nigels-com/glew/issues/157
      target_link_libraries(glew
        PRIVATE
          -nostdlib)
      # https://discourse.vtk.org/t/1898
      target_compile_options(glew
        PRIVATE
          -minline-all-stringops)
    endif()
  endif ()
else ()
  target_compile_definitions(glew
    PUBLIC
      GLEW_STATIC)
endif ()

target_include_directories(glew
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include")
