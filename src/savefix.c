#include <stdbool.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <cormoran/runtime_combo/runtime_combo.h>

int __real_zmk_runtime_combo_write(uint32_t index,
                                   const struct zmk_runtime_combo_config *combo,
                                   bool persist);
int __real_zmk_runtime_combo_write_name(uint32_t index, const char *name, bool persist);

int __wrap_zmk_runtime_combo_write(uint32_t index,
                                   const struct zmk_runtime_combo_config *combo,
                                   bool persist) {
    ARG_UNUSED(persist);
    return __real_zmk_runtime_combo_write(index, combo, true);
}

int __wrap_zmk_runtime_combo_write_name(uint32_t index, const char *name, bool persist) {
    ARG_UNUSED(persist);
    return __real_zmk_runtime_combo_write_name(index, name, true);
}
