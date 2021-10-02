#include "arch/interface/kinit.h"

int main ()
{
    pre_kernel_init();
    kernel_init();
}
