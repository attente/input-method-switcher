cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

configure_file(20_proxy.conf.in 20_proxy.conf)
configure_file(20_proxy.rc.in 20_proxy.rc)
configure_file(com.ubuntu.InputMethodSwitcher.xml.in com.ubuntu.InputMethodSwitcher.xml)
configure_file(com.ubuntu.InputMethodSwitcher.service.in com.ubuntu.InputMethodSwitcher.service)
configure_file(com.ubuntu.input-method-switcher.gschema.xml.in com.ubuntu.input-method-switcher.gschema.xml)

install(DIRECTORY frameworks DESTINATION share/input-method-switcher USE_SOURCE_PERMISSIONS)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.ubuntu.InputMethodSwitcher.service DESTINATION share/dbus-1/services)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/com.ubuntu.input-method-switcher.gschema.xml DESTINATION share/glib-2.0/schemas)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/20_proxy.conf ${CMAKE_CURRENT_BINARY_DIR}/20_proxy.rc DESTINATION share/im-config/data)
