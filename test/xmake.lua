add_requires("unity_test")

target("test_stv")
set_kind("binary")
add_files("./*.c")
add_packages("unity_test")
