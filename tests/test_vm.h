#pragma once

#include "../src/vm/vm.h"
#include "munit/munit.h" // Use the MUnit submodule

// Function prototype for the VM garbage collection test
MunitResult test_gc(const MunitParameter params[], void *user_data);
