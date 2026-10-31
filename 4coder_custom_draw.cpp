
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
