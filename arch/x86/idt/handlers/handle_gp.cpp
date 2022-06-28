#include "handlers.h"
#include <panic.h>

void handlers::handle_gp(uint64_t c, uint64_t) { std::panic("#GP"); }
