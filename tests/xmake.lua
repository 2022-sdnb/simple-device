add_requires("gtest 1.11.0")

target("Tests")
    set_kind("binary")
    set_default(false)
    add_deps("Demo")
    add_files("*.cpp")
    if is_plat("windows") then
        add_ldflags("/subsystem:console")
    end
    add_packages("gtest")
