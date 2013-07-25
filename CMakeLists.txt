project(kopete-whiteboard)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(Kopete REQUIRED)
find_package(QCA2 REQUIRED)

add_subdirectory( icons )
add_subdirectory( icons/whiteboarding )

#include (MacroLibrary)
#include(MacroOptionalAddSubdirectory)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${QCA2_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ${KOPETE_INCLUDE_DIR} ${KOPETE_INCLUDE_DIR}/kopete ${QCA2_INCLUDE_DIR})

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=14319) #/usr/share/kde4/config/kdebug.areas
#add_definitions(-lqca)
add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS )
########### next target ###############
set(
	kopete_whiteboard_PART_SRCS
	whiteboardplugin.cpp
	whiteboardguiclient.cpp
	whiteboarddialog.cpp
	palettewidget.cpp
	wbwidget.cpp
	wbitems.cpp
	wbscene.cpp
)

kde4_add_kcfg_files(kopete_whiteboard_PART_SRCS kopete_whiteboard.kcfgc )
kde4_add_ui_files(kopete_whiteboard_PART_SRCS whiteboardview.ui )

kde4_add_plugin(kopete_whiteboard ${kopete_whiteboard_PART_SRCS})

#target_link_libraries(kopete_whiteboard ${KDE4_KDEUI_LIBS} kopete)
target_link_libraries(
	kopete_whiteboard
	#${KDE4_KIO_LIBS}
	${KDE4_KDEUI_LIBS}
	#${KDE4_KDECORE_LIBS}
	#${KDE4_KHTML_LIBS}
	#${KDE4_PHONON_LIBS}
	${QT_QTXML_LIBRARY}
	#${QT_QTCORE_LIBRARY}
	#${QT_QTGUI_LIBRARY}
	#${QT_QT3SUPPORT_LIBRARY}
	${QCA2_LIBRARIES}
	#${QT_LIBRARIES}
	 kopete )
install(TARGETS kopete_whiteboard DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kcm_kopete_whiteboard_PART_SRCS   whiteboardpreferences.cpp )

kde4_add_ui_files(kcm_kopete_whiteboard_PART_SRCS whiteboardprefs.ui )

kde4_add_kcfg_files(kcm_kopete_whiteboard_PART_SRCS ${kwb_PART_SRCS} kopete_whiteboard.kcfgc )

kde4_add_plugin(kcm_kopete_whiteboard ${kcm_kopete_whiteboard_PART_SRCS})

#target_link_libraries(kcm_kopete_whiteboard ${KDE4_KDEUI_LIBS} kopete)
target_link_libraries(kcm_kopete_whiteboard  ${KDE4_KUTILS_LIBS} ${KDE4_KIO_LIBS} kopete )

install(TARGETS kcm_kopete_whiteboard  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############
install( FILES kopete_whiteboard.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
install( FILES kopete_whiteboard.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kopete_whiteboard_config.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kconfiguredialog)
install( FILES whiteboardchatui.rc  DESTINATION ${DATA_INSTALL_DIR}/kopete_whiteboard)
kde4_install_icons( ${DATA_INSTALL_DIR}/kopete/icons  )

