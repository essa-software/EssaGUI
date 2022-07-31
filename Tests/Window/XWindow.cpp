#include <LLGL/Window/Impls/XWindow.hpp>

int main()
{
    llgl::XWindow window;
    window.set_title("test");
    window.set_size({500, 500});
    window.open();
    for(;;)
        ;
    return 0;
}
