# Install script for directory: /tronlong/HansAiCameraApp/third_party/ElaWidgetTools

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib" TYPE SHARED_LIBRARY FILES "/tronlong/HansAiCameraApp/build_host/third_party/ElaWidgetTools/libElaWidgetTools.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/libElaWidgetTools.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/include" TYPE FILE FILES
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaAcrylicUrlCard.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaActionCommander.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaAppBar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaApplication.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaBreadcrumbBar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaCalendar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaCalendarPicker.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaCheckBox.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaColorDialog.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaComboBox.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaContentDialog.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaDef.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaDialog.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaDockWidget.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaDoubleSpinBox.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaDrawerArea.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaDxgiManager.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaEventBus.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaExponentialBlur.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaFlowLayout.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaIcon.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaIconButton.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaImageCard.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaInteractiveCard.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaKeyBinder.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaLCDNumber.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaLineEdit.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaListView.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaLog.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaMenu.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaMenuBar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaMessageBar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaMessageButton.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaMultiSelectComboBox.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaNavigationBar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaPivot.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaPlainTextEdit.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaPopularCard.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaProgressBar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaProgressRing.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaPromotionCard.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaPromotionView.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaProperty.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaPushButton.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaRadioButton.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaReminderCard.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaRoller.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaRollerPicker.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaScrollArea.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaScrollBar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaScrollPage.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaScrollPageArea.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaSingleton.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaSlider.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaSpinBox.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaStatusBar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaSuggestBox.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaTabBar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaTabWidget.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaTableView.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaText.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaTheme.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaToggleButton.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaToggleSwitch.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaToolBar.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaToolButton.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaToolTip.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaTreeView.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaWidget.h"
    "/tronlong/HansAiCameraApp/third_party/ElaWidgetTools/ElaWindow.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets.cmake"
         "/tronlong/HansAiCameraApp/build_host/third_party/ElaWidgetTools/CMakeFiles/Export/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake" TYPE FILE FILES "/tronlong/HansAiCameraApp/build_host/third_party/ElaWidgetTools/CMakeFiles/Export/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake" TYPE FILE FILES "/tronlong/HansAiCameraApp/build_host/third_party/ElaWidgetTools/CMakeFiles/Export/ElaWidgetTools/lib/cmake/ElaWidgetToolsTargets-noconfig.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/ElaWidgetTools/lib/cmake" TYPE FILE FILES
    "/tronlong/HansAiCameraApp/build_host/third_party/ElaWidgetTools/ElaWidgetToolsConfig.cmake"
    "/tronlong/HansAiCameraApp/build_host/third_party/ElaWidgetTools/ElaWidgetToolsConfigVersion.cmake"
    )
endif()

