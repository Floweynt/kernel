#include "handlers.h"

void handlers::handle_gp(uint64_t c, uint64_t) { debug::panic("#GP"); }
