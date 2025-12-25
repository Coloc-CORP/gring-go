#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app)

# Spécifie explicitement le chemin de sortie dans le dossier de build
generate_inc_file_for_target(app 
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/bhi260ap_arc_fw.bin 
    ${CMAKE_CURRENT_BINARY_DIR}/bhi260ap_arc_fw.bin.inc
)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# NORDIC SDK APP START
target_sources(app PRIVATE src/main.c)

# NORDIC SDK APP END
zephyr_include_directories(
    assets
    include
    drivers
    subsys/algo
    subsys/ble
)

target_sources(app PRIVATE
    # Main file
    src/main.c

    # Layer 1: Hardware Drivers
    drivers/imu_driver.c
    drivers/bio_driver.c
    drivers/temp_driver.c
    drivers/charge_driver.c
    drivers/haptic_driver.c

    # Layer 2: Processing Algorithms and Monitors
    subsys/algo/sensors_wrapper.c
    subsys/algo/power_monitor.c

    # Layer 3: GATT Services (BLE Interface)
    subsys/ble/health_service.c 
    subsys/ble/battery_service.c
    subsys/ble/alert_service.c 
)
