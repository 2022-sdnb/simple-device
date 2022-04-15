set_project("simple-device")
set_version("0.0.1", {build = "%Y%m%d%H%M"})

set_xmakever("2.6.1")

set_warnings("all", "error")
set_languages("cxx20")

add_rules("mode.debug", "mode.release")

add_requires("spdlog v1.9.2")
add_requires("nlohmann_json v3.10.0")

add_repositories("repo repo")
add_requires("protocon main")

target("simple-device")
    set_kind("binary")
    add_files("src/*.cpp")
    add_ldflags("-pthread")
    add_packages("spdlog")
    add_packages("nlohmann_json")
    add_packages("protocon")

includes("benches")
