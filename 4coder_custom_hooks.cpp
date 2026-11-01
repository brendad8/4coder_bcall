
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
  
  if(tick_all_fade_ranges(app, frame_info.animation_dt))
  {
    animate_in_n_milliseconds(app, 0);
  }

  vim_animate_filebar(app, frame_info);
  vim_animate_cursor(app, frame_info);
#if VIM_DO_ANIMATE
  vim_cursor_blink += frame_info.animation_dt;
#endif
  
  fold_tick(app, frame_info);
  
  b32 enable_virtual_whitespace = def_get_config_b32(vars_save_string_lit("enable_virtual_whitespace"));
  if(enable_virtual_whitespace != def_enable_virtual_whitespace)
  {
    def_enable_virtual_whitespace = enable_virtual_whitespace;
    clear_all_layouts(app);
  }
  // NOTE(rjf): Default tick stuff from the 4th dimension:
  default_tick(app, frame_info);
}


function BUFFER_HOOK_SIG(custom_begin_buffer)
{
    Scratch_Block scratch(app);
    b32 treat_as_code = false;
    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer_id);
    String_Const_u8 buffer_name = push_buffer_base_name(app, scratch, buffer_id);
    
    // NOTE(rjf): Treat as code if the config tells us to.
    if(treat_as_code == false)
    {
        if(file_name.size > 0)
        {
            String_Const_u8 treat_as_code_string = def_get_config_string(scratch, vars_save_string_lit("treat_as_code"));
            String_Const_u8_Array extensions = parse_extension_line_to_extension_list(app, scratch, treat_as_code_string);
            String_Const_u8 ext = string_file_extension(file_name);
            for(i32 i = 0; i < extensions.count; ++i)
            {
                if(string_match(ext, extensions.strings[i]))
                {
                    treat_as_code = true;
                    break;
                }
            }
        }
    }
    
    // NOTE(rjf): Treat as code for *calc* and *peek* buffers.
    if(treat_as_code == false)
    {
        if(buffer_name.size > 0)
        {
            if(string_match(buffer_name, string_u8_litexpr("*calc*")))
            {
                treat_as_code = true;
            }
            else if(string_match(buffer_name, string_u8_litexpr("*peek*")))
            {
                treat_as_code = true;
            }
        }
    }
    
    // NOTE(rjf): Treat as code if we've identified the language of a file.
    if(treat_as_code == false)
    {
        F4_Language *language = F4_LanguageFromBuffer(app, buffer_id);
        if(language)
        {
            treat_as_code = true;
        }
    }
    
    String_ID file_map_id = vars_save_string_lit("keys_file");
    String_ID code_map_id = vars_save_string_lit("keys_code");
    
    Command_Map_ID map_id = (treat_as_code) ? (code_map_id) : (file_map_id);
    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    Command_Map_ID *map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
    *map_id_ptr = map_id;
    
    Line_Ending_Kind setting = guess_line_ending_kind_from_buffer(app, buffer_id);
    Line_Ending_Kind *eol_setting = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
    *eol_setting = setting;
    
    // NOTE(allen): Decide buffer settings
    b32 wrap_lines = true;
    b32 use_lexer = false;
    if(treat_as_code)
    {
        wrap_lines = def_get_config_b32(vars_save_string_lit("enable_code_wrapping"));
        use_lexer = true;
    }
    
    if(string_match(buffer_name, string_u8_litexpr("*compilation*")))
    {
        wrap_lines = false;
    }
    
    if (use_lexer)
    {
        ProfileBlock(app, "begin buffer kick off lexer");
        Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);
        *lex_task_ptr = async_task_no_dep(&global_async_system, F4_DoFullLex_ASYNC, make_data_struct(&buffer_id));
    }
    
    {
        b32 *wrap_lines_ptr = scope_attachment(app, scope, buffer_wrap_lines, b32);
        *wrap_lines_ptr = wrap_lines;
    }
    
    if (use_lexer)
    {
        buffer_set_layout(app, buffer_id, layout_virt_indent_index_generic);
    }
    else
    {
        if (treat_as_code)
        {
            buffer_set_layout(app, buffer_id, layout_virt_indent_literal_generic);
        }
        else{
            buffer_set_layout(app, buffer_id, layout_generic);
        }
    }
  
    vim_begin_buffer_inner(app, buffer_id);
    
    // no meaning for return
    return(0);
}

function void
custom_render_caller(Application_Links *app, Frame_Info frame_info, View_ID view_id){
  Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
  Face_ID face_id = get_face_id(app, 0);
  Face_Metrics face_metrics = get_face_metrics(app, face_id);
  f32 line_height = face_metrics.line_height;
  f32 digit_advance = face_metrics.decimal_digit_advance;

  Rect_f32 region = view_get_screen_rect(app, view_id);
  Rect_f32 prev_clip = draw_set_clip(app, region);

  Rect_f32 global_rect = global_get_screen_rectangle(app);
  f32 filebar_y = global_rect.y1 - 2.f*line_height - vim_cur_filebar_offset;
  if(region.y1 >= filebar_y){ region.y1 = filebar_y; }

  draw_rectangle_fcolor(app, region, 0.f, fcolor_id(defcolor_back));

  region = vim_draw_query_bars(app, region, view_id, face_id);

  // Font_Load_Location secondary_font_location = {};
  // secondary_font_location.file_name = string_u8_litexpr("C:\\Users\\guppy\\4cc\\build\\fonts\\CascadiaMono-Bold.ttf");
  //
  // Face_Description secondary_face_desc = {};
  // secondary_face_desc.font = secondary_font_location;
  // secondary_face_desc.parameters.pt_size = 16;
  // secondary_face_desc.parameters.hinting = false;
  //
  // Face_ID secondary_face_id = face_id_from_description(app, &secondary_face_desc);
  // if (secondary_face_id == 0)
  // {
  //     secondary_face_id = try_create_new_face(app, &secondary_face_desc);
  // }

  {
    Rect_f32_Pair pair = layout_file_bar_on_bot(region, line_height);
    pair.b = rect_split_top_bottom(pair.b, line_height).a;
    // vim_draw_filebar(app, view_id, buffer, frame_info, face_id, pair.b);
    custom_draw_filebar(app, view_id, buffer, frame_info, face_id, pair.b);
    region = pair.a;
  }

  // Draw borders
  if(region.x0 > global_rect.x0){
    Rect_f32_Pair border_pair = rect_split_left_right(region, 2.f);
    draw_rectangle_fcolor(app, border_pair.a, 0.f, fcolor_id(defcolor_margin));
    region = border_pair.b;
  }
  if(region.x1 < global_rect.x1){
    Rect_f32_Pair border_pair = rect_split_left_right_neg(region, 2.f);
    draw_rectangle_fcolor(app, border_pair.b, 0.f, fcolor_id(defcolor_margin));
    region = border_pair.a;
  }
  region.y0 += 3.f;

  if(show_fps_hud){
    Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
    draw_fps_hud(app, frame_info, face_id, pair.max);
    region = pair.min;
    animate_in_n_milliseconds(app, 1000);
  }

  b32 show_line_number_margins = def_get_config_b32(vars_save_string_lit("show_line_number_margins"));
  Rect_f32 line_number_rect = {};
  if(show_line_number_margins){
    Rect_f32_Pair pair = (vim_relative_numbers ?
                          vim_line_number_margin(app, buffer, region, digit_advance) :
                          layout_line_number_margin(app, buffer, region, digit_advance));
    line_number_rect = pair.min;
    region = pair.max;
  }

  Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);
  Buffer_Point_Delta_Result delta = delta_apply(app, view_id, frame_info.animation_dt, scroll);
  if(!block_match_struct(&scroll.position, &delta.point)){
    block_copy_struct(&scroll.position, &delta.point);
    view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
  }
  if(delta.still_animating){ animate_in_n_milliseconds(app, 0); }
  Buffer_Point buffer_point = scroll.position;
  Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);

  if(show_line_number_margins){
    if(vim_relative_numbers)
      vim_draw_rel_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
    else
      vim_draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
  }

  custom_render_buffer(app, view_id, face_id, buffer, text_layout_id, region);

  text_layout_free(app, text_layout_id);
  draw_set_clip(app, prev_clip);
}

function Rect_f32
custom_buffer_region(Application_Links *app, View_ID view_id, Rect_f32 region){

  Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
  Face_ID face_id = get_face_id(app, buffer);
  Face_Metrics metrics = get_face_metrics(app, face_id);
  f32 line_height = metrics.line_height;
  f32 digit_advance = metrics.decimal_digit_advance;

  Rect_f32 global_rect = global_get_screen_rectangle(app);
  f32 filebar_y = global_rect.y1 - 2.f*line_height - vim_cur_filebar_offset;
  if(region.y1 >= filebar_y){
    region.y1 = filebar_y;
  }

  Query_Bar *space[32];
  Query_Bar_Ptr_Array query_bars = {};
  query_bars.ptrs = space;
  if(get_active_query_bars(app, view_id, ArrayCount(space), &query_bars)){
    region = layout_query_bar_on_bot(region, line_height, query_bars.count).min;
  }

  b64 showing_file_bar = false;
  if(view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar){
    region = layout_file_bar_on_bot(region, line_height).min;
  }

  if(region.x0 > global_rect.x0){
    region = rect_split_left_right(region, 2.f).b;
  }
  if(region.x1 < global_rect.x1){
    region = rect_split_left_right_neg(region, 2.f).a;
  }
  region.y0 += 3.f;

  if(show_fps_hud){ region = layout_fps_hud_on_bottom(region, line_height).min; }

  b32 show_line_number_margins = def_get_config_b32(vars_save_string_lit("show_line_number_margins"));
  if(show_line_number_margins){
    region = (vim_relative_numbers ?
              vim_line_number_margin(app, buffer, region, digit_advance) :
              layout_line_number_margin(app, buffer, region, digit_advance)).max;
  }

  return region;
}

