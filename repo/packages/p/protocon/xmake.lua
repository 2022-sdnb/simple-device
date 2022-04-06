package("protocon")

    set_homepage("https://github.com/kejiyuan306/protocon-cpp")
    set_description("A lightwight protocol for IOT.")

    set_urls("https://github.com/kejiyuan306/protocon-cpp.git")

    on_install("linux", function (package)
        import("package.tools.xmake").install(package)
    end)
