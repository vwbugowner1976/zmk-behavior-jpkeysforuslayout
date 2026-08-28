#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zmk/studio/custom.h>
#include <cormoran/runtime_combo/runtime_combo.h>
#include <cormoran/zmk/custom_settings.h>

static bool is_custom_settings_subsystem(uint8_t index) {
    size_t subsystem_count;
    STRUCT_SECTION_COUNT(zmk_rpc_custom_subsystem, &subsystem_count);

    if (index >= subsystem_count) {
        return false;
    }

    struct zmk_rpc_custom_subsystem *subsystem;
    STRUCT_SECTION_GET(zmk_rpc_custom_subsystem, index, &subsystem);

    return subsystem && subsystem->identifier &&
           strcmp(subsystem->identifier, "cormoran_custom_settings") == 0;
}

static bool is_runtime_combo_setting(const struct zmk_custom_setting *setting) {
    return setting && setting->custom_subsystem_id &&
           strcmp(setting->custom_subsystem_id, ZMK_RUNTIME_COMBO_SUBSYSTEM_ID) == 0;
}

int __real_raise_zmk_studio_custom_notification(struct zmk_studio_custom_notification data);
int __real_zmk_runtime_combo_write(uint32_t index,
                                   const struct zmk_runtime_combo_config *combo,
                                   bool persist);
int __real_zmk_runtime_combo_write_name(uint32_t index, const char *name, bool persist);
int __real_zmk_custom_setting_write_array_element(
    const struct zmk_custom_setting *setting,
    const struct zmk_custom_setting_value *value,
    uint32_t array_size,
    enum zmk_custom_setting_write_mode mode);

int __wrap_raise_zmk_studio_custom_notification(struct zmk_studio_custom_notification data) {
    if (is_custom_settings_subsystem(data.subsystem_index)) {
        return 0;
    }

    return __real_raise_zmk_studio_custom_notification(data);
}

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

int __wrap_zmk_custom_setting_write_array_element(
    const struct zmk_custom_setting *setting,
    const struct zmk_custom_setting_value *value,
    uint32_t array_size,
    enum zmk_custom_setting_write_mode mode) {
    enum zmk_custom_setting_write_mode effective_mode = mode;

    if (is_runtime_combo_setting(setting)) {
        effective_mode = ZMK_CUSTOM_SETTING_WRITE_MODE_PERSIST;
        printk("[combo-savefix] write %s/%s idx=%u size=%u mode=%d->%d\n",
               setting->custom_subsystem_id,
               setting->array_key ? setting->array_key : setting->key,
               (unsigned int)setting->array_index,
               (unsigned int)array_size,
               (int)mode,
               (int)effective_mode);
    }

    int ret = __real_zmk_custom_setting_write_array_element(setting, value, array_size,
                                                             effective_mode);

    if (is_runtime_combo_setting(setting)) {
        printk("[combo-savefix] write ret=%d\n", ret);
    }

    return ret;
}
