
function void
custom_tick(Application_Links *app, Frame_Info frame_info)
{
  linalloc_clear(&global_frame_arena);
  global_tooltip_count = 0;
  
  F4_TickColors(app, frame_info);
  F4_Index_Tick(app);
  // F4_CLC_Tick(frame_info);
  // F4_PowerMode_Tick(app, frame_info);
  // F4_UpdateFlashes(app, frame_info);
  
  // code_index_update_tick(app);
  
  if(tick_all_fade_ranges(app, frame_info.animation_dt)){
    animate_in_n_milliseconds(app, 0);
  }

  vim_animate_filebar(app, frame_info);
  vim_animate_cursor(app, frame_info);
#if VIM_DO_ANIMATE
  vim_cursor_blink += frame_info.animation_dt;
#endif
  
  fold_tick(app, frame_info);
  
  b32 enable_virtual_whitespace = def_get_config_b32(vars_save_string_lit("enable_virtual_whitespace"));
  if(enable_virtual_whitespace != def_enable_virtual_whitespace){
    def_enable_virtual_whitespace = enable_virtual_whitespace;
    clear_all_layouts(app);
  }
  // NOTE(rjf): Default tick stuff from the 4th dimension:
  default_tick(app, frame_info);
}
