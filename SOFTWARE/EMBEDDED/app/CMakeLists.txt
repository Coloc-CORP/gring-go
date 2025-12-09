#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app)

# NORDIC SDK APP START
target_sources(app PRIVATE src/main.c)

# NORDIC SDK APP END
zephyr_include_directories(
    include
    drivers
    subsys/algo
    subsys/ble
)

target_sources(app PRIVATE
    # Main file
    src/main.c

    # Layer 1: Hardware Drivers
    drivers/bhi260ap_driver.c
    drivers/max32664_driver.c
    drivers/stts22h_driver.c
    drivers/stwlc38_driver.c

    # Layer 2: Processing Algorithms and Monitors
    subsys/algo/motion_tracker.c
    subsys/algo/sleep_monitor.c
    subsys/algo/power_monitor.c

    # Layer 3: GATT Services (BLE Interface)
    subsys/ble/health_service.c 
    subsys/ble/battery_service.c 
)
