#include "AbstractOpenGLHelper.hpp"

std::list<OpenGLCommand> OpenGL::command_buffer;
bool OpenGL::write_commands_to_buffer = false;
