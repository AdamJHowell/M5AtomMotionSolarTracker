cmake_minimum_required( VERSION 3.23 )
project( M5AtomMotionSolarTracker )

set( CMAKE_CXX_STANDARD 14 )

include_directories( .
		"C:/Users/Adam/AppData/Local/Arduino15/packages/m5stack/hardware/esp32/2.1.0/cores/esp32/"
		"C:/Users/Adam/AppData/Local/Arduino15/packages/m5stack/hardware/esp32/2.1.0/libraries/Wire/src/"
		"C:/Users/Adam/AppData/Local/Arduino15/packages/m5stack/hardware/esp32/2.1.0/tools/sdk/esp32/include/freertos/include/"
		"C:/Users/Adam/AppData/Local/Arduino15/packages/m5stack/hardware/esp32/2.1.0/tools/sdk/esp32/include/soc/esp32/include/"
		"C:/Users/Adam/AppData/Local/Arduino15/packages/esp32/hardware/esp32/2.0.14/variants/m5stack_atom/"
		"C:/Users/Adam/Documents/Arduino/libraries/M5-DLight/src/"
		"C:/Users/Adam/Documents/Arduino/libraries/M5Atom/src/"
)

add_executable( M5AtomMotionSolarTracker
		M5AtomMotionSolarTracker.h
		M5AtomMotionSolarTracker.ino
		AtomMotion.h
		AtomMotion.cpp
)

set_target_properties( M5AtomMotionSolarTracker PROPERTIES LINKER_LANGUAGE CXX )
