set_languages("c99", "cxx17")
set_toolchains("clang")

target("jansson")
    set_kind("static")
	add_includedirs("../jansson")
	add_files("../jansson/*.c")
	add_defines("HAVE_CONFIG_H")
	add_cxflags("-fPIC", "-O3", "-g3")

target("test")
	set_kind("binary")
	add_deps("jansson")
	add_includedirs("../jansson")
	add_files("test.cpp")
	add_defines("HAVE_CONFIG_H")
	add_cxflags("-O3", "-g3", "-Wno-undefined-bool-conversion")


