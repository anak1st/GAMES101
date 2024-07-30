add_requires("eigen 3.4.0", "opencv 4.10.0", "fmt 11.0.2")

target("BezierCurve")
    set_kind("binary")
    set_languages("c++20")
    add_packages("eigen", "opencv", "fmt")

    add_files("src/*.c")
    add_files("src/*.cpp")
    add_files("src/*.cc")
