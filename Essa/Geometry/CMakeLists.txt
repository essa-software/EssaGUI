add_library(Geometry SHARED
    Distance.cpp
    Intersection.cpp
    Line2D.cpp
)
add_library(Essa::Geometry ALIAS Geometry)

set_target_properties(Geometry PROPERTIES OUTPUT_NAME "essa-eg")
set_target_properties(Geometry PROPERTIES SOVERSION ${ESSA_GIT_REVISION})
essautil_setup_target(Geometry)
target_link_libraries(Geometry PUBLIC Util)
