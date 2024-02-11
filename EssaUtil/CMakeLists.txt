add_library(Util SHARED
    ArgParser.cpp
    Base64.cpp
    Buffer.cpp
    Color.cpp
    Config.cpp
    CoordinateSystem/GeoCoords.cpp
    DisplayError.cpp
    Math/Plane.cpp
    Math/Ray.cpp
    SimulationClock.cpp
    Stream/File.cpp
    Stream/MemoryStream.cpp
    Stream/Reader.cpp
    Stream/StandardStreams.cpp
    Stream/Writer.cpp
    System.cpp
    UnitDisplay.cpp
    UString.cpp
    UStringBuilder.cpp
    Utf8.cpp
)
add_library(Essa::Util ALIAS Util)

set_target_properties(Util PROPERTIES OUTPUT_NAME "essa-util")
set_target_properties(Util PROPERTIES SOVERSION ${ESSA_GIT_REVISION})
essautil_setup_target(Util)
install(TARGETS Util EXPORT Essa DESTINATION lib)
target_link_libraries(Util PUBLIC fmt::fmt)

# FIXME: This breaks build of eml/AST.cpp somewhere in Value::string()
target_compile_options(Util PRIVATE -Wno-restrict)
