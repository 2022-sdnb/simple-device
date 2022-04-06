add_requires("benchmark 1.5.5")

target("Benches")
    set_kind("binary")
    set_default(false)
    add_deps("Demo")
    add_files("*.cpp")
    if is_plat("windows") then
        add_ldflags("/subsystem:console")
    end
    add_packages("benchmark")
