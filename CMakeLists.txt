if(CONFIG_ZMK_RUNTIME_COMBO_STUDIO_RPC)
  target_sources(app PRIVATE src/savefix.c)
  target_link_options(app PRIVATE
    "-Wl,--wrap=raise_zmk_studio_custom_notification"
    "-Wl,--wrap=zmk_runtime_combo_write"
    "-Wl,--wrap=zmk_runtime_combo_write_name"
  )
endif()
