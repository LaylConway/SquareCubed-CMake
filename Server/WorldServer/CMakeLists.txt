# Get All Files to Build
file(GLOB WorldServer_SRC
	"*.h"
	"*.cpp"
	"*.h.in"
	"*.cpp.in"
)

# Add Executable Build Output
add_executable (WorldServer ${WorldServer_SRC})

# Link Required Libraries
target_link_libraries (WorldServer WorldServerBase)
target_link_libraries (WorldServer ServerRakNetNetwork)

# Add Installer Details
install (TARGETS WorldServer DESTINATION ./ COMPONENT Server)

# Set Directory
set_property (TARGET WorldServer PROPERTY FOLDER "Server")