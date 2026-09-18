This project has been created as part of the 42 curriculum by marasolo.

# Codexion

## Description
Codexion simulates a concurrent coding environment where multiple coders compete for a circular ring of dongles. The simulation runs a compile/debug/refactor cycle, enforces a per-dongle cooldown, and stops either when a coder burns out or when all coders have achieved the required number of compilations.

## Instructions
Compile with:

make

Run with:

./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler

The scheduler must be either fifo or edf.

## Resources
The code uses pthreads, mutexes, condition variables, and a handcrafted min-heap to schedule contested dongles. Each coder thread does a compile cycle and each simulation uses a dedicated monitor thread to detect burnout and to stop the program cleanly.

## Blocking cases handled
The implementation prevents common deadlock patterns by acquiring dongles in a strict deterministic order and by releasing any partially acquired dongle if the second acquisition fails. Cooldown is enforced between releases and the next grant. A wake-up broadcast is used to avoid lost wakeups.

## Thread synchronization mechanisms
The project relies on mutexes for state, logging, scheduler, and resources. Condition variables are used to wait for dongles and to wake threads when a state change or cooldown change occurs. The simulation stop flag is protected by the same mutex to avoid races.
