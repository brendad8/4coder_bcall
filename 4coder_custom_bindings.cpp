
VIM_COMMAND_SIG(vim_submode_rbracket){ vim_state.sub_mode = SUB_RBracket; vim_state.chord_resolved = false; }
VIM_COMMAND_SIG(vim_submode_lbracket){ vim_state.sub_mode = SUB_LBracket; vim_state.chord_resolved = false; }

function void custom_bindings(Application_Links *app)
{
  // Normal, Insert, Visual
  // These could be local variables but I like the highlighting
#define N bit_1
#define I bit_2
#define V bit_3
#define MAP 0

  VimBind(N|V|MAP, vim_submode_g,                            KeyCode_G);
  VimBind(N|V|MAP, vim_submode_z,                            KeyCode_Z);
  VimBind(N|V|MAP, vim_submode_leader,                       KeyCode_Space);
  VimBind(N|V|MAP, vim_submode_rbracket,                     KeyCode_RightBracket);
  VimBind(N|V|MAP, vim_submode_lbracket,                     KeyCode_LeftBracket);


  VimBind(N|MAP, vim_open_file_in_quotes,      SUB_Leader,   KeyCode_F);
  VimBind(N|MAP, vim_interactive_open_or_new,  SUB_Leader,   KeyCode_E);
  VimBind(N|MAP, quick_swap_buffer,            SUB_RBracket, KeyCode_B);

  VimBind(N|MAP, qol_jump_to_definition,                     KeyMod_Sft|KeyCode_K);
  VimBind(N|MAP, vim_goto_definition,                        KeyMod_Alt|KeyCode_K);

  VimBind(N|MAP, vim_next_4coder_jump,                       KeyCode_N);
  VimBind(N|MAP, vim_prev_4coder_jump,                       KeyMod_Sft|KeyCode_N);

  // TODO(bcall): if indentint/outdenting in visual mode then stay in visual mode
  VimBind(N|V|MAP, vim_request_indent,                       KeyCode_Tab);
  VimBind(N|V|MAP, vim_request_outdent,                      KeyMod_Sft|KeyCode_Tab);

  VimBind(N|MAP, vim_lowercase,               SUB_G,         KeyCode_U);
  VimBind(N|MAP, vim_uppercase,               SUB_G,         KeyMod_Sft|KeyCode_U);

  VimBind(V|MAP, vim_lowercase,                              KeyCode_U);
  VimBind(V|MAP, vim_uppercase,                              KeyMod_Sft|KeyCode_U);

  // VimBind(N|V|MAP, vim_search_identifier,         (Ctl|Sft|KeyCode_8));
  // VimBind(N|V|MAP, vim_search_identifier,             (Sft|KeyCode_8));
  // VimBind(N|V|MAP, vim_clear_search,          SUB_Leader,  KeyCode_Space);
  // VimBind(N|V|MAP, vim_start_search_forward,               KeyCode_ForwardSlash);
  // VimBind(N|V|MAP, vim_start_search_backward,         (Sft|KeyCode_ForwardSlash));
  // VimBind(N|V|MAP, vim_to_next_pattern,                    KeyCode_N);
  // VimBind(N|V|MAP, vim_to_prev_pattern,               (Sft|KeyCode_N));
  // VimBind(N|MAP, vim_in_next_pattern,        SUB_G,      KeyCode_N);
  // VimBind(N|MAP, vim_in_prev_pattern,        SUB_G, (Sft|KeyCode_N));
  //
  // VimBind(N|MAP, vim_prev_jump,                     (Ctl|KeyCode_O));
  // VimBind(N|MAP, vim_next_jump,                     (Ctl|KeyCode_I));
  //
  // /// Screen Adjust Binds
  // VimBind(N|V|MAP, vim_half_page_up,                (Ctl|KeyCode_U));
  // VimBind(N|V|MAP, vim_half_page_down,              (Ctl|KeyCode_D));
  // VimBind(N|V|MAP, vim_whole_page_up,               (Ctl|KeyCode_B));
  // VimBind(N|V|MAP, vim_whole_page_down,             (Ctl|KeyCode_F));
  // VimBind(N|V|MAP, vim_line_down,               (Ctl|Sft|KeyCode_Y));
  // VimBind(N|V|MAP, vim_line_down,                   (Ctl|KeyCode_Y));
  // VimBind(N|V|MAP, vim_line_up,                 (Ctl|Sft|KeyCode_E));
  // VimBind(N|V|MAP, vim_line_up,                     (Ctl|KeyCode_E));
  // VimBind(N|V|MAP, vim_scroll_screen_top,         SUB_Z, KeyCode_T);
  // VimBind(N|V|MAP, vim_scroll_screen_mid,         SUB_Z, KeyCode_Z);
  // VimBind(N|V|MAP, vim_scroll_screen_bot,         SUB_Z, KeyCode_B);
  //
  // /// Miscellaneous Binds
  // VimBind(N|V|MAP, vim_set_mark,                         KeyCode_M);
  // VimBind(N|V|MAP, vim_goto_mark,                        KeyCode_Tick);
  // VimBind(N|V|MAP, vim_goto_mark,                        KeyCode_Quote);
  // VimBind(N|V|MAP, vim_toggle_macro,                     KeyCode_Q);
  // VimBind(N|V|MAP, vim_play_macro,                  (Sft|KeyCode_2));
  //
  // /// Window Binds
  VimBind(N|MAP, vim_change_x0, KeyMod_Ctl|KeyCode_H);
  VimBind(N|MAP, vim_change_y1, KeyMod_Ctl|KeyCode_J);
  VimBind(N|MAP, vim_change_y0, KeyMod_Ctl|KeyCode_K);
  VimBind(N|MAP, vim_change_x1, KeyMod_Ctl|KeyCode_L);


#undef I
#undef N
#undef V
#undef MAP
}
