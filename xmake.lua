set_languages("c++latest", "clatest")
add_rules("mode.debug", "mode.release")

target("raw_polymorphism_c")
    set_kind("binary")
    add_files("src/raw_polymorphism_c.c")