
function void
custom_draw_filebar(Application_Links *app, View_ID view_id, Buffer_ID buffer, Frame_Info frame_info, Face_ID face_id, Rect_f32 bar)
{
  Scratch_Block scratch(app);
  String_Const_u8 unique_name = push_buffer_unique_name(app, scratch, buffer);

  draw_rectangle_fcolor(app, bar, 0.f, fcolor_id(defcolor_bar));

  f32 char_wid = get_face_metrics(app, face_id).normal_advance;
  Rect_f32 title_rect = bar;
  title_rect.x1 = bar.x0 + char_wid*unique_name.size;
  // draw_rectangle_fcolor(app, title_rect, 0.f, fcolor_id(defcolor_vim_filebar_pop));
  draw_rectangle_fcolor(app, title_rect, 0.f, fcolor_id(defcolor_bar));

  // Rect_f32 triangle_rect = title_rect;
  // f32 radius_fudge = 5.f;
  // triangle_rect.x0 = title_rect.x1 - radius_fudge*char_wid;
  // triangle_rect.x1 = title_rect.x1 + radius_fudge*char_wid;
  // draw_rectangle_fcolor(app, triangle_rect, radius_fudge*char_wid, fcolor_id(defcolor_vim_filebar_pop));

  FColor base_color = fcolor_id(defcolor_base);
  FColor pop2_color = fcolor_id(defcolor_pop2);

  i64 cursor_position = view_get_cursor_pos(app, view_id);
  Buffer_Cursor cursor = view_compute_cursor(app, view_id, seek_pos(cursor_position));

  u8 space[5];
  String_u8 str = Su8(space, 0, 4);

  Managed_Scope scope = buffer_get_managed_scope(app, buffer);
  Line_Ending_Kind *eol_kind = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
  switch(*eol_kind){
    case LineEndingKind_Binary:{ string_append(&str, string_u8_litexpr("bin"));  } break;
    case LineEndingKind_LF:    { string_append(&str, string_u8_litexpr("lf"));   } break;
    case LineEndingKind_CRLF:  { string_append(&str, string_u8_litexpr("crlf")); } break;
  }


  Vec2_f32 p = V2f32(title_rect.x1 + 4.5f*char_wid, bar.y0 + 3.f);
  p = draw_string(app, face_id, str.string, p, base_color);
  // p = draw_string(app, global_fonts.bold, str.string, p, base_color);

  str = Su8(space, 0, 5);
  Dirty_State dirty = buffer_get_dirty_state(app, buffer);
  if(dirty != 0){
    string_append(&str, string_u8_litexpr(" ["));
    if(HasFlag(dirty, DirtyState_UnsavedChanges))  string_append_character(&str, '+');
    if(HasFlag(dirty, DirtyState_UnloadedChanges)) string_append_character(&str, '!');
    string_append(&str, string_u8_litexpr("]"));
    draw_string(app, face_id, str.string, p, pop2_color);
  }

  p.x = Max(p.x + 5.f*char_wid, bar.x1 - char_wid*15.f);
  draw_string(app, face_id, push_stringf(scratch, "%d,%d", cursor.line, cursor.col), p, base_color);

  p.x = bar.x0 + 2.f;
  draw_string(app, face_id, unique_name, p, base_color);
  // draw_string(app, global_fonts.bold, unique_name, p, base_color);

  // TODO: vim_state.mode             == VIM_Visual "Selection: %d lines, %d chars"
  // TODO: vim_state.params.edit_type == EDIT_Block "Selection: %d x %d block, %d chars"

  p.x = bar.x1 - char_wid*3.5f;
  i64 N = buffer_get_size(app, buffer);
  String_Const_u8 pos_text = (cursor_position == 0 ? string_u8_litexpr("Top") :
                              cursor_position == N ? string_u8_litexpr("Bot") :
                              push_stringf(scratch, "%d%%", i64(100.f*cursor_position/(N))));
  draw_string(app, face_id, pos_text, p, base_color);
}



function void
custom_draw_whole_screen(Application_Links *app, Frame_Info frame_info){
  Rect_f32 region = global_get_screen_rectangle(app);
  Vec2_f32 center = rect_center(region);
  draw_set_clip(app, region);

  Face_ID face_id = get_face_id(app, 0);
  Face_Metrics metrics = get_face_metrics(app, face_id);
  f32 line_height = metrics.line_height;
  f32 char_wid = metrics.normal_advance;

  // NOTE(BYP): Drawing the cursor for view transitions
  if(vim_cur_cursor_pos != vim_nxt_cursor_pos){
    Vec2_f32 cursor_dim = V2f32(9.f, (vim_state.mode == VIM_Insert ? 4.f : 18.f));
    Rect_f32 cursor_rect = Rf32_xy_wh(vim_cur_cursor_pos - cursor_dim, cursor_dim);
    u64 cursor_roundness_100 = def_get_config_u64(app, vars_save_string_lit("cursor_roundness"));
    f32 roundness = char_wid*cursor_roundness_100*0.01f;
    draw_rectangle_fcolor(app, cursor_rect, roundness, fcolor_id(defcolor_cursor, default_cursor_sub_id()));
  }

  ARGB_Color back_color = fcolor_resolve(fcolor_id(defcolor_back));

  // NOTE(BYP): Drawing the back of the filebar lister
  Rect_f32 back_rect = vim_get_bottom_rect(app);
  if(vim_cur_filebar_offset > vim_nxt_filebar_offset){
    draw_rectangle(app, back_rect, 0.f, back_color);
    draw_rectangle_fcolor(app, rect_split_top_bottom_neg(back_rect, 4.f).b, 0.f, get_item_margin_color(UIHighlight_Active));
  }

  draw_rectangle(app, rect_split_top_bottom_neg(region, 2.f*line_height).b, 0.f, back_color);

  Vec2_f32 bot_left = {region.x0 + 4.f, region.y1 - 1.5f*line_height};
  String_Const_u8 bot_string = vim_get_bot_string();

  if(vim_use_bottom_cursor){
    Vec2_f32 p = draw_string(app, face_id, vim_bot_text.string, bot_left, finalize_color(defcolor_text_default, 0));
    if(vim_show_cursor(app)){
      if(vim_bot_text.size){ p.x -= 0.37f*(p.x - bot_left.x)/vim_bot_text.size; }
      draw_string(app, face_id, string_u8_litexpr("|"), p, finalize_color(defcolor_text_default, 0));
    }
  }else{
    draw_string(app, face_id, bot_string, bot_left, finalize_color(defcolor_text_default, 0));
  }

  if(vim_lister_view_id == 0){
    /// NOTE(BYP): This is kinda a hacky pseudo-view
    if(vim_show_buffer_peek && rect_height(back_rect) > 0.f){
      Vim_Buffer_Peek_Entry *entry = vim_buffer_peek_list + vim_buffer_peek_index;
      Buffer_Identifier buffer_iden = entry->buffer_id;
      Buffer_ID buffer = buffer_identifier_to_id(app, buffer_iden);
      vim_set_bottom_text(SCu8((u8 *)buffer_iden.name, buffer_iden.name_len));

      f32 ratio_diff = entry->nxt_ratio - entry->cur_ratio;
      entry->cur_ratio += ratio_diff*frame_info.animation_dt*20.f;

      Buffer_Point buffer_point = {};
      i64 line_count = buffer_get_line_count(app, buffer);
      //buffer_point.line_number = i64(entry->cur_ratio*(line_count+1) - rect_height(back_rect)/line_height);
      buffer_point.pixel_shift.y = line_height*entry->cur_ratio*(line_count+1) - rect_height(back_rect);

      FColor peek_back_color = fcolor_id(defcolor_back);
      draw_rectangle_fcolor(app, back_rect, 0.f, peek_back_color);
      Rect_f32_Pair pair = rect_split_top_bottom_neg(back_rect, 4.f);
      draw_rectangle_fcolor(app, pair.b, 0.f, get_item_margin_color(UIHighlight_Active));
      back_rect = rect_split_left_right(pair.a, 4.f).b;
      Rect_f32 prev_clip = draw_set_clip(app, back_rect);

      Text_Layout_ID text_layout_id = text_layout_create(app, buffer, back_rect, buffer_point);
      Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);

      paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
      paint_fade_ranges(app, text_layout_id, buffer);
      draw_text_layout_default(app, text_layout_id);

      text_layout_free(app, text_layout_id);
      draw_set_clip(app, prev_clip);
    }
  }

  Vec2_f32 bot_right = {region.x1 - 4.f - char_wid*vim_keystroke_text.size, bot_left.y};
  FColor chord_color = fcolor_id(defcolor_vim_chord_unresolved);
  if(vim_state.chord_resolved){
    chord_color = (vim_state.chord_resolved & bit_2 ?  fcolor_id(defcolor_vim_chord_error) : fcolor_id(defcolor_vim_chord_text));
  }
  draw_string(app, face_id, vim_keystroke_text.string, bot_right, chord_color);
}

function void
f4_custom_draw_whole_screen(Application_Links *app, View_ID view_id, Face_ID face_id,
                Buffer_ID buffer, Text_Layout_ID text_layout_id,
                Rect_f32 rect, Frame_Info frame_info)
{
    Scratch_Block scratch(app);
    ProfileScope(app, "[Fleury] Render Buffer");
    
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    Rect_f32 prev_clip = draw_set_clip(app, rect);
    
    // NOTE(allen): Token colorizing
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    if(token_array.tokens != 0)
    {
        F4_SyntaxHighlight(app, text_layout_id, &token_array);
        
        // NOTE(allen): Scan for TODOs and NOTEs
        b32 use_comment_keywords = def_get_config_b32(vars_save_string_lit("use_comment_keywords"));
        if(use_comment_keywords)
        {
            Comment_Highlight_Pair pairs[] =
            {
                {str8_lit("NOTE"), finalize_color(defcolor_comment_pop, 0)},
                {str8_lit("TODO"), finalize_color(defcolor_comment_pop, 1)},
                {def_get_config_string(scratch, vars_save_string_lit("user_name")), finalize_color(fleury_color_comment_user_name, 0)},
            };
            draw_comment_highlights(app, buffer, text_layout_id,
                                    &token_array, pairs, ArrayCount(pairs));
        }
    }
    else
    {
        Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
        paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
    }
    
    i64 cursor_pos = view_correct_cursor(app, view_id);
    view_correct_mark(app, view_id);
    
    // NOTE(allen): Scope highlight
    b32 use_scope_highlight = def_get_config_b32(vars_save_string_lit("use_scope_highlight"));
    if (use_scope_highlight){
        Color_Array colors = finalize_color_array(defcolor_back_cycle);
        draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }
    
    // NOTE(rjf): Brace highlight
    {
        Color_Array colors = finalize_color_array(fleury_color_brace_highlight);
        if(colors.count >= 1 && F4_ARGBIsValid(colors.vals[0]))
        {
            F4_Brace_RenderHighlight(app, buffer, text_layout_id, cursor_pos,
                                     colors.vals, colors.count);
        }
    }
    
    // NOTE(allen): Line highlight
    {
        b32 highlight_line_at_cursor = def_get_config_b32(vars_save_string_lit("highlight_line_at_cursor"));
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        if(highlight_line_at_cursor && (is_active_view || buffer == compilation_buffer))
        {
            i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
            draw_line_highlight(app, text_layout_id, line_number,
                                fcolor_id(defcolor_highlight_cursor_line));
        }
    }
    
    // NOTE(rjf): Error/Search Highlight
    {
        b32 use_error_highlight = def_get_config_b32(vars_save_string_lit("use_error_highlight"));
        b32 use_jump_highlight = def_get_config_b32(vars_save_string_lit("use_jump_highlight"));
        if (use_error_highlight || use_jump_highlight){
            // NOTE(allen): Error highlight
            String_Const_u8 name = string_u8_litexpr("*compilation*");
            Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
            if (use_error_highlight){
                draw_jump_highlights(app, buffer, text_layout_id, compilation_buffer,
                                     fcolor_id(defcolor_highlight_junk));
            }
            
            // NOTE(allen): Search highlight
            if (use_jump_highlight){
                Buffer_ID jump_buffer = get_locked_jump_buffer(app);
                if (jump_buffer != compilation_buffer){
                    draw_jump_highlights(app, buffer, text_layout_id, jump_buffer,
                                         fcolor_id(defcolor_highlight_white));
                }
            }
        }
    }
    
    // NOTE(rjf): Error annotations
    {
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        F4_RenderErrorAnnotations(app, buffer, text_layout_id, compilation_buffer);
    }
    
    // NOTE(jack): Token Occurance Highlight
    if (!def_get_config_b32(vars_save_string_lit("f4_disable_cursor_token_occurance"))) 
    {
        ProfileScope(app, "[Fleury] Token Occurance Highlight");
        
        // NOTE(jack): Get the active cursor's token string
        Buffer_ID active_cursor_buffer = view_get_buffer(app, active_view, Access_Always);
        i64 active_cursor_pos = view_get_cursor_pos(app, active_view);
        Token_Array active_cursor_buffer_tokens = get_token_array_from_buffer(app, active_cursor_buffer);
        Token_Iterator_Array active_cursor_it = token_iterator_pos(0, &active_cursor_buffer_tokens, active_cursor_pos);
        Token *active_cursor_token = token_it_read(&active_cursor_it);
        
        String_Const_u8 active_cursor_string = string_u8_litexpr("");
        if(active_cursor_token)
        {
            active_cursor_string = push_buffer_range(app, scratch, active_cursor_buffer, Ii64(active_cursor_token));
            
            // Loop the visible tokens
            Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
            i64 first_index = token_index_from_pos(&token_array, visible_range.first);
            Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);
            for (;;)
            {
                Token *token = token_it_read(&it);
                if(!token || token->pos >= visible_range.one_past_last)
                {
                    break;
                }
                
                if (token->kind == TokenBaseKind_Identifier)
                {
                    Range_i64 token_range = Ii64(token);
                    String_Const_u8 token_string = push_buffer_range(app, scratch, buffer, token_range);
                    
                    // NOTE(jack) If this is the buffers cursor token, highlight it with an Underline
                    if (range_contains(token_range, view_get_cursor_pos(app, view_id)))
                    {
                        F4_RenderRangeHighlight(app, view_id, text_layout_id,
                                                token_range, F4_RangeHighlightKind_Underline,
                                                fcolor_resolve(fcolor_id(fleury_color_token_highlight)));
                    }
                    // NOTE(jack): If the token matches the active buffer token. highlight it with a Minor Underline
                    else if(active_cursor_token->kind == TokenBaseKind_Identifier && 
                            string_match(token_string, active_cursor_string))
                    {
                        F4_RenderRangeHighlight(app, view_id, text_layout_id,
                                                token_range, F4_RangeHighlightKind_MinorUnderline,
                                                fcolor_resolve(fcolor_id(fleury_color_token_minor_highlight)));
                        
                    } 
                }
                
                if(!token_it_inc_non_whitespace(&it))
                {
                    break;
                }
            }
        }
    }
    // NOTE(jack): if "f4_disable_cursor_token_occurance" is set, just highlight the cusror 
    else
    {
        ProfileScope(app, "[Fleury] Token Highlight");
        
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, cursor_pos);
        Token *token = token_it_read(&it);
        if(token && token->kind == TokenBaseKind_Identifier)
        {
            F4_RenderRangeHighlight(app, view_id, text_layout_id,
                                    Ii64(token->pos, token->pos + token->size),
                                    F4_RangeHighlightKind_Underline,
                                    fcolor_resolve(fcolor_id(fleury_color_token_highlight)));
        }
    }
    
    // NOTE(rjf): Flashes
    {
        F4_RenderFlashes(app, view_id, text_layout_id);
    }
    
    // NOTE(allen): Color parens
    if(def_get_config_b32(vars_save_string_lit("use_paren_helper")))
    {
        Color_Array colors = finalize_color_array(defcolor_text_cycle);
        draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }
    
    // NOTE(rjf): Divider Comments
    {
        F4_RenderDividerComments(app, buffer, view_id, text_layout_id);
    }
    
    // NOTE(rjf): Cursor Mark Range
    if(is_active_view && fcoder_mode == FCoderMode_Original)
    {
        F4_HighlightCursorMarkRange(app, view_id);
    }
    
    // NOTE(allen): Cursor shape
    Face_Metrics metrics = get_face_metrics(app, face_id);
    u64 cursor_roundness_100 = def_get_config_u64(app, vars_save_string_lit("cursor_roundness"));
    f32 cursor_roundness = metrics.normal_advance*cursor_roundness_100*0.01f;
    f32 mark_thickness = (f32)def_get_config_u64(app, vars_save_string_lit("mark_thickness"));
    
    // NOTE(rjf): Cursor
    switch (fcoder_mode)
    {
        case FCoderMode_Original:
        {
            F4_Cursor_RenderEmacsStyle(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness, frame_info);
        }break;
        
        case FCoderMode_NotepadLike:
        {
            F4_Cursor_RenderNotepadStyle(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness,
                                         mark_thickness, frame_info);
            break;
        }
    }
    
    // NOTE(rjf): Brace annotations
    // {
    //     F4_Brace_RenderCloseBraceAnnotation(app, buffer, text_layout_id, cursor_pos);
    // }
    
    // NOTE(rjf): Brace lines
    // {
    //     F4_Brace_RenderLines(app, buffer, view_id, text_layout_id, cursor_pos);
    // }
    
    // NOTE(allen): put the actual text on the actual screen
    draw_text_layout_default(app, text_layout_id);
    
    // NOTE(rjf): Interpret buffer as calc code, if it's the calc buffer.
    {
        Buffer_ID calc_buffer_id = get_buffer_by_name(app, string_u8_litexpr("*calc*"), AccessFlag_Read);
        if(calc_buffer_id == buffer)
        {
            F4_CLC_RenderBuffer(app, buffer, view_id, text_layout_id, frame_info);
        }
    }
    
    // NOTE(rjf): Draw calc comments.
    {
        F4_CLC_RenderComments(app, buffer, view_id, text_layout_id, frame_info);
    }
    
    draw_set_clip(app, prev_clip);
    
    // NOTE(rjf): Draw tooltips and stuff.
    if(active_view == view_id)
    {
        // NOTE(rjf): Position context helper
        {
            F4_PosContext_Render(app, view_id, buffer, text_layout_id, cursor_pos);
        }
        
        // NOTE(rjf): Draw tooltip list.
        {
            Mouse_State mouse = get_mouse_state(app);
            
            Rect_f32 view_rect = view_get_screen_rect(app, view_id);
            
            Face_ID tooltip_face_id = global_small_code_face;
            Face_Metrics tooltip_face_metrics = get_face_metrics(app, tooltip_face_id);
            
            Rect_f32 tooltip_rect =
            {
                (f32)mouse.x + 16,
                (f32)mouse.y + 16,
                (f32)mouse.x + 16,
                (f32)mouse.y + 16 + tooltip_face_metrics.line_height + 8,
            };
            
            for(int i = 0; i < global_tooltip_count; ++i)
            {
                String_Const_u8 string = global_tooltips[i].string;
                tooltip_rect.x1 = tooltip_rect.x0;
                tooltip_rect.x1 += get_string_advance(app, tooltip_face_id, string) + 4;
                
                if(tooltip_rect.x1 > view_rect.x1)
                {
                    f32 difference = tooltip_rect.x1 - view_rect.x1;
                    tooltip_rect.x1 = (float)(int)(tooltip_rect.x1 - difference);
                    tooltip_rect.x0 = (float)(int)(tooltip_rect.x0 - difference);
                }
                
                F4_DrawTooltipRect(app, tooltip_rect);
                
                draw_string(app, tooltip_face_id, string,
                            V2f32(tooltip_rect.x0 + 4,
                                  tooltip_rect.y0 + 4),
                            global_tooltips[i].color);
            }
        }
    }
    
    // NOTE(rjf): Draw inactive rectangle
    if(is_active_view == 0)
    {
        Rect_f32 view_rect = view_get_screen_rect(app, view_id);
        ARGB_Color color = fcolor_resolve(fcolor_id(fleury_color_inactive_pane_overlay));
        if(F4_ARGBIsValid(color))
        {
            draw_rectangle(app, view_rect, 0.f, color);
        }
    }
    
    // NOTE(rjf): Render code peek.
    {
        if(!view_get_is_passive(app, view_id) &&
           !is_active_view)
        {
            F4_CodePeek_Render(app, view_id, face_id, buffer, frame_info);
        }
    }
    
    // NOTE(rjf): Draw power mode.
    {
        F4_PowerMode_RenderBuffer(app, view_id, face_id, frame_info);
    }
    
}

function void
custom_render_buffer(Application_Links *app, View_ID view_id, Face_ID face_id, Buffer_ID buffer, Text_Layout_ID text_layout_id, Rect_f32 rect)
{
  ProfileScope(app, "render buffer");
  
  Scratch_Block scratch(app);

  View_ID active_view = get_active_view(app, Access_Always);
  b32 is_active_view = (active_view == view_id);
  Rect_f32 prev_clip = draw_set_clip(app, rect);

  Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);

  Face_Metrics metrics = get_face_metrics(app, face_id);
  u64 cursor_roundness_100 = def_get_config_u64(app, vars_save_string_lit("cursor_roundness"));
  f32 cursor_roundness = metrics.normal_advance*cursor_roundness_100*0.01f;
  f32 mark_thickness = (f32)def_get_config_u64(app, vars_save_string_lit("mark_thickness"));


  Token_Array token_array = get_token_array_from_buffer(app, buffer);
  if(token_array.tokens != 0)
  {
      F4_SyntaxHighlight(app, text_layout_id, &token_array);
      
      // NOTE(allen): Scan for TODOs and NOTEs
      b32 use_comment_keywords = def_get_config_b32(vars_save_string_lit("use_comment_keywords"));
      if(use_comment_keywords)
      {
          Comment_Highlight_Pair pairs[] =
          {
              {str8_lit("NOTE"), finalize_color(defcolor_comment_pop, 0)},
              {str8_lit("TODO"), finalize_color(defcolor_comment_pop, 1)},
              {def_get_config_string(scratch, vars_save_string_lit("user_name")), finalize_color(fleury_color_comment_user_name, 0)},
          };
          draw_comment_highlights(app, buffer, text_layout_id,
                                  &token_array, pairs, ArrayCount(pairs));
      }
  }
  else
  {
      Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
      paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
  }
  
  i64 cursor_pos = view_correct_cursor(app, view_id);
  view_correct_mark(app, view_id);

  // NOTE(rjf): Brace highlight
  {
    Color_Array colors = finalize_color_array(fleury_color_brace_highlight);
    if(colors.count >= 1 && F4_ARGBIsValid(colors.vals[0]))
    {
        F4_Brace_RenderHighlight(app, buffer, text_layout_id, cursor_pos,
                                 colors.vals, colors.count);
    }
  }

  b32 highlight_line_at_cursor = def_get_config_b32(vars_save_string_lit("highlight_line_at_cursor"));
  if (highlight_line_at_cursor && is_active_view)
  {
    i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
    draw_line_highlight(app, text_layout_id, line_number, fcolor_id(defcolor_highlight_cursor_line));
  }

  b32 use_scope_highlight = def_get_config_b32(vars_save_string_lit("use_scope_highlight"));
  if (use_scope_highlight)
  {
    Color_Array colors = finalize_color_array(defcolor_back_cycle);
    draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
  }

  b32 use_error_highlight = def_get_config_b32(vars_save_string_lit("use_error_highlight"));
  b32 use_jump_highlight  = def_get_config_b32(vars_save_string_lit("use_jump_highlight"));
  if(use_error_highlight || use_jump_highlight)
  {
    Buffer_ID comp_buffer = get_buffer_by_name(app, string_u8_litexpr("*compilation*"), Access_Always);
    if(use_error_highlight)
    {
      draw_jump_highlights(app, buffer, text_layout_id, comp_buffer, fcolor_id(defcolor_highlight_junk));
    }

    if(use_jump_highlight)
    {
      Buffer_ID jump_buffer = get_locked_jump_buffer(app);
      if(jump_buffer != comp_buffer)
      {
        draw_jump_highlights(app, buffer, text_layout_id, jump_buffer, fcolor_id(defcolor_highlight_white));
      }
    }
  }

  b32 use_paren_helper = def_get_config_b32(vars_save_string_lit("use_paren_helper"));
  if(use_paren_helper)
  {
    // Color_Array colors = finalize_color_array(defcolor_text_cycle);
    Color_Array colors = finalize_color_array(fleury_color_brace_highlight);
    draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
  }

  b64 show_whitespace = false;
  view_get_setting(app, view_id, ViewSetting_ShowWhitespace, &show_whitespace);
  if (show_whitespace)
  {
    if (token_array.tokens == 0)
    {
      draw_whitespace_highlight(app, buffer, text_layout_id, cursor_roundness);
    }
    else
    {
      draw_whitespace_highlight(app, text_layout_id, &token_array, cursor_roundness);
    }
  }

  if (is_active_view && vim_state.mode == VIM_Visual)
  {
    vim_draw_visual_mode(app, view_id, buffer, face_id, text_layout_id);
  }

  vim_draw_search_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);

  switch (fcoder_mode)
  {
    case FCoderMode_Original:
    vim_draw_cursor(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness); break;
    case FCoderMode_NotepadLike:
    draw_notepad_style_cursor_highlight(app, view_id, buffer, text_layout_id, cursor_roundness); break;
  }

  // NOTE(rjf): Brace annotations
  // {
  //     F4_Brace_RenderCloseBraceAnnotation(app, buffer, text_layout_id, cursor_pos);
  // }

  paint_fade_ranges(app, text_layout_id, buffer);
  draw_text_layout_default(app, text_layout_id);

  vim_draw_after_text(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness);

  draw_set_clip(app, prev_clip);
}

