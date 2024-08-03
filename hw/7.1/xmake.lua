add_requires("fmt 11.0.2", "opencv 4.10.0")

target("RayTracing")
    set_kind("binary")
    set_languages("c++20")
    add_cxflags("-O2")
    add_packages("fmt", "opencv")

    add_files("src/*.c")
    add_files("src/*.cpp")
    add_files("src/*.cc")
