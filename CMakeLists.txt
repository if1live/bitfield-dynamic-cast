CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( "bitfield_dynamic_cast" )

FILE( GLOB SRC_FILES "*.cpp" "*.h" )
ADD_EXECUTABLE( bitfield_dynamic_cast ${SRC_FILES} )
