set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt section generated by Qt Creator 9.0.0

find_package("Qt${VTK_QT_VERSION}" REQUIRED COMPONENTS Core Quick)

if (NOT "${Qt${VTK_QT_VERSION}_DIR}" STREQUAL "")
  message(STATUS "Qt${VTK_QT_VERSION}_DIR:" ${Qt${VTK_QT_VERSION}_DIR})
endif ()
message(STATUS "QT_VERSION: Qt${QT_VERSION_MAJOR}")

if (QT_VERSION_MAJOR GREATER_EQUAL 6)
  qt_add_executable(${MYNAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
  )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${MYNAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else ()
  if (ANDROID)
    add_library(${MYNAME} SHARED
      ${PROJECT_SOURCES}
    )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else ()
    add_executable(${MYNAME}
      ${PROJECT_SOURCES}
    )
  endif ()
endif ()

target_compile_definitions(${MYNAME} PRIVATE
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>"
)

target_link_libraries(${MYNAME}
  PRIVATE Qt${VTK_QT_VERSION}::Core Qt${VTK_QT_VERSION}::Quick)

set_target_properties(${MYNAME} PROPERTIES
  MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE TRUE
  WIN32_EXECUTABLE TRUE
)

install(TARGETS ${MYNAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (VTK_QT_VERSION EQUAL 6)
  qt_import_qml_plugins(${MYNAME})
  qt_finalize_executable(${MYNAME})
endif ()
