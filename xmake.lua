set_project("Demo")
set_version("0.0.1", {build = "%Y%m%d%H%M"})

set_xmakever("2.6.1")

set_warnings("all", "error")
set_languages("cxx20")

add_rules("mode.debug", "mode.release")

target("Demo")
    set_kind("static")
    add_files("src/*.cpp")
    add_includedirs("include", { public = true })
    add_headerfiles("include/(Demo/*.h)")

includes("tests")
includes("benches")
includes("examples")
