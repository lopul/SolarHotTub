add_executable(SolarHotTub
        SolarHotTub.c
        )

# pull in common dependencies
target_link_libraries(SolarHotTub pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(SolarHotTub)

# add url via pico_set_program_url
example_auto_set_url(SolarHotTub)
