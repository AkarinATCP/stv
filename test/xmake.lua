add_requires("unity_test")

target("test_stv")
    set_kind("binary")
    add_files("./test_stv.c")
    add_packages("unity_test")
