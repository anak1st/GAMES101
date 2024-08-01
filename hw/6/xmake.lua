add_requires("fmt 11.0.2")

target("RayTracing")
    set_kind("binary")
    set_languages("c++20")
    add_cxflags("-O2")
    add_packages("fmt")

    add_files("src/*.c")
    add_files("src/*.cpp")
    add_files("src/*.cc")
