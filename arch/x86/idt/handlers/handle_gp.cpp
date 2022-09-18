#include "handlers.h"

void handlers::handle_gp(std::uint64_t c, std::uint64_t) { debug::panic("#GP"); }
