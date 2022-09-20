add_rules("mode.debug", "mode.release")
set_license("MIT")

if is_mode("debug") then
    add_defines("CHANZY_DEBUG")
else
    add_undefines("CHANZY_DEBUG")
end

target("chanzy_container")
    set_kind("headeronly")
    add_headerfiles("src/*.hpp")
target_end()

target("chanzy_container_test")
    set_kind("binary")
    add_deps("chanzy_container")
    add_files("src/test.cpp")
target_end()
