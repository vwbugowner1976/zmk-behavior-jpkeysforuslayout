#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <zmk/studio/custom.h>

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

int __real_raise_zmk_studio_custom_notification(struct zmk_studio_custom_notification data);

int __wrap_raise_zmk_studio_custom_notification(struct zmk_studio_custom_notification data) {
    if (is_custom_settings_subsystem(data.subsystem_index)) {
        return 0;
    }

    return __real_raise_zmk_studio_custom_notification(data);
}
