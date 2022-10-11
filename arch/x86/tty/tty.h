#include <kinit/limine.h>

namespace tty
{
    void init(const limine_framebuffer_response* buf);    
}

// useful formatting macros
#define RED(c) "\033cr;" c "\033"
#define CYAN(c) "\033cc;" c "\033"
#define GREEN(c) "\033cg;" c "\033"
#define YELLOW(c) "\033cy;" c "\033"
