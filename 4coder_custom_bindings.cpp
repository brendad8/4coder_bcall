
VIM_COMMAND_SIG(vim_submode_rbracket){ vim_state.sub_mode = SUB_RBracket; vim_state.chord_resolved = false; }
VIM_COMMAND_SIG(vim_submode_lbracket){ vim_state.sub_mode = SUB_LBracket; vim_state.chord_resolved = false; }

function void setup_custom_bindings(Application_Links *app)
{
  // Normal, Insert, Visual
  #define N bit_1
  #define I bit_2
  #define V bit_3
  #define MAP 0

  //*****************************************************************************
  // Default Bindings
  //*****************************************************************************

  // Mode Binds
  VimBind(MAP, vim_normal_mode,                              KeyCode_Escape);
  VimBind(N|V|MAP, vim_modal_i,                              KeyCode_I);
  VimBind(N|V|MAP, vim_modal_a,                              KeyCode_A);
  VimBind(N|V|MAP, vim_command_mode,                         KeyMod_Sft|KeyCode_Semicolon);
  VimBind(N|MAP,   vim_insert_begin,                         KeyMod_Sft|KeyCode_I);
  VimBind(N|MAP,   vim_insert_end,                           KeyMod_Sft|KeyCode_A);
  VimBind(N|MAP,   vim_replace_mode,                         KeyMod_Sft|KeyCode_R);
  VimBind(N|V|MAP, vim_visual_mode,                          KeyCode_V);
  VimBind(N|V|MAP, vim_visual_mode,                          KeyMod_Sft|KeyCode_V);
  VimBind(N|V|MAP, vim_visual_mode,                          KeyMod_Ctl|KeyCode_V);
  VimBind(N|MAP,   vim_prev_visual,               SUB_G,     KeyCode_V);
  VimBind(N|MAP,   vim_newline_below,                        KeyCode_O);
  VimBind(N|MAP,   vim_newline_above,                        KeyMod_Sft|KeyCode_O);
  VimBind(V|MAP,   vim_visual_insert,                        KeyMod_Sft|KeyCode_A);
  VimBind(V|MAP,   vim_visual_insert,                        KeyMod_Sft|KeyCode_I);
  VimBind(I|MAP,   vim_insert_command,                       KeyMod_Ctl|KeyMod_Sft|KeyCode_O);

  // SubMode Binds
  VimBind(N|V|MAP, vim_submode_g,                            KeyCode_G);
  VimBind(N|V|MAP, vim_submode_z,                            KeyCode_Z);
  VimBind(N|V|MAP, vim_submode_leader,                       KeyCode_Space);
  VimBind(N|V|MAP, vim_submode_rbracket,                     KeyCode_RightBracket);
  VimBind(N|V|MAP, vim_submode_lbracket,                     KeyCode_LeftBracket);

  // Save, Undo, Redo
  VimBind(N|MAP, save,                                       KeyMod_Ctl|KeyCode_S);
  VimBind(N|MAP, undo,                                       KeyCode_U);
  VimBind(N|MAP, undo,                                       KeyMod_Ctl|KeyCode_Z);
  VimBind(N|MAP, redo,                                       KeyMod_Ctl|KeyCode_R);

  // Edit Binds
  VimBind(N|V|MAP, vim_request_yank,                         KeyCode_Y);
  VimBind(N|V|MAP, vim_request_delete,                       KeyCode_D);
  VimBind(N|V|MAP, vim_request_change,                       KeyCode_C);
  VimBind(N|V|MAP, vim_delete_end,                           KeyMod_Sft|KeyCode_D);
  VimBind(N|V|MAP, vim_change_end,                           KeyMod_Sft|KeyCode_C);
  VimBind(N|V|MAP, vim_yank_end,                             KeyMod_Sft|KeyCode_Y);
  VimBind(V|MAP,   vim_replace_range_next,                   KeyCode_R);

  VimBind(N|MAP,   vim_paste_before,                         KeyMod_Sft|KeyCode_P);
  VimBind(N|MAP,   vim_paste_after,                          KeyCode_P);
  VimBind(N|MAP,   vim_backspace_char,                       KeyMod_Sft|KeyCode_X);
  VimBind(N|MAP,   vim_delete_char,                          KeyCode_X);
  VimBind(N|MAP,   vim_replace_next_char,                    KeyCode_R);
  VimBind(N|V|MAP, vim_combine_line,                         KeyMod_Sft|KeyCode_J);
  VimBind(N|V|MAP, vim_combine_line,         SUB_G,          KeyMod_Sft|KeyCode_J);
  VimBind(N|MAP,   vim_last_command,                         KeyCode_Period);
  VimBind(N|V|MAP, vim_select_register,                      KeyMod_Sft|KeyCode_Quote);
  VimBind(N|MAP,   vim_toggle_char,                          KeyMod_Sft|KeyCode_Tick);
  VimBind(I|MAP,   vim_select_register,                      KeyMod_Ctl|KeyCode_R);
  VimBind(I|MAP,   vim_delete_to_begin,                      KeyMod_Ctl|KeyCode_U);
  VimBind(V|MAP,   vim_move_selection_up,                    KeyMod_Alt|KeyCode_K);
  VimBind(V|MAP,   vim_move_selection_down,                  KeyMod_Alt|KeyCode_J);

  // Digit Binds
  VimBind(N|V|MAP, vim_modal_0,                              KeyCode_0);
  VimBind(N|V|MAP, vim_digit,                                KeyCode_1);
  VimBind(N|V|MAP, vim_digit,                                KeyCode_2);
  VimBind(N|V|MAP, vim_digit,                                KeyCode_3);
  VimBind(N|V|MAP, vim_digit,                                KeyCode_4);
  VimBind(N|V|MAP, vim_digit,                                KeyCode_5);
  VimBind(N|V|MAP, vim_digit,                                KeyCode_6);
  VimBind(N|V|MAP, vim_digit,                                KeyCode_7);
  VimBind(N|V|MAP, vim_digit,                                KeyCode_8);
  VimBind(N|V|MAP, vim_digit,                                KeyCode_9);
  VimBind(N|V|MAP, vim_digit_del,                            KeyCode_Backspace);
  VimBind(N|V|MAP, vim_digit_del,                            KeyCode_Delete);

  // Movement Binds
  VimBind(N|V|MAP, vim_left,                                 KeyCode_H);
  VimBind(N|V|MAP, vim_down,                                 KeyCode_J);
  VimBind(N|V|MAP, vim_up,                                   KeyCode_K);
  VimBind(N|V|MAP, vim_right,                                KeyCode_L);
  VimBind(N|V|MAP, vim_end_line,                             KeyMod_Sft|KeyCode_4);
  VimBind(N|V|MAP, vim_begin_line,                           KeyMod_Sft|KeyCode_6);
  VimBind(N|V|MAP, vim_forward_word,                         KeyCode_W);
  VimBind(N|V|MAP, vim_forward_WORD,                         KeyMod_Sft|KeyCode_W);
  VimBind(N|V|MAP, vim_backward_word,                        KeyCode_B);
  VimBind(N|V|MAP, vim_backward_WORD,                        KeyMod_Sft|KeyCode_B);
  VimBind(N|V|MAP, vim_forward_end,                          KeyCode_E);
  VimBind(N|V|MAP, vim_forward_END,                          KeyMod_Sft|KeyCode_E);
  VimBind(N|V|MAP, vim_backward_end,         SUB_G,          KeyCode_E);
  VimBind(N|V|MAP, vim_backward_END,         SUB_G,          KeyMod_Sft|KeyCode_E);

  VimBind(N|V|MAP, vim_file_top,             SUB_G,          KeyCode_G);
  VimBind(N|V|MAP, vim_goto_line,                            KeyMod_Sft|KeyCode_G);
  VimBind(N|V|MAP, vim_goto_column,                          KeyMod_Sft|KeyCode_BackwardSlash);
  VimBind(N|V|MAP, vim_modal_percent,                        KeyMod_Sft|KeyCode_5);
  VimBind(N|V|MAP, vim_bounce,                               KeyMod_Ctl|KeyCode_5);
  VimBind(N|V|MAP, vim_set_seek_char,                        KeyCode_F);
  VimBind(N|V|MAP, vim_set_seek_char,                        KeyCode_T);
  VimBind(N|V|MAP, vim_set_seek_char,                        KeyMod_Sft|KeyCode_F);
  VimBind(N|V|MAP, vim_set_seek_char,                        KeyMod_Sft|KeyCode_T);
  VimBind(N|V|MAP, vim_seek_char_forward,                    KeyCode_Semicolon);
  VimBind(N|V|MAP, vim_seek_char_backward,                   KeyCode_Comma);
  VimBind(N|V|MAP, vim_paragraph_up,                         KeyMod_Sft|KeyCode_LeftBracket);
  VimBind(N|V|MAP, vim_paragraph_down,                       KeyMod_Sft|KeyCode_RightBracket);
  VimBind(N|V|MAP, vim_screen_top,                           KeyMod_Sft|KeyCode_H);
  VimBind(N|V|MAP, vim_screen_bot,                           KeyMod_Sft|KeyCode_L);
  VimBind(N|V|MAP, vim_screen_mid,                           KeyMod_Sft|KeyCode_M);
  VimBind(V|MAP,   cursor_mark_swap,                         KeyCode_O);
  VimBind(V|MAP,   vim_block_swap,                           KeyMod_Sft|KeyCode_O);

  VimBind(N|V|MAP, vim_search_identifier,                    KeyMod_Ctl|KeyMod_Sft|KeyCode_8);
  VimBind(N|V|MAP, vim_search_identifier,                    KeyMod_Sft|KeyCode_8);
  VimBind(N|V|MAP, vim_clear_search,          SUB_Leader,    KeyCode_Space);
  VimBind(N|V|MAP, vim_start_search_forward,                 KeyCode_ForwardSlash);
  VimBind(N|V|MAP, vim_start_search_backward,                KeyMod_Sft|KeyCode_ForwardSlash);
  // VimBind(N|V|MAP, vim_to_next_pattern,                      KeyCode_N);
  // VimBind(N|V|MAP, vim_to_prev_pattern,                      KeyMod_Sft|KeyCode_N);
  VimBind(N|MAP, vim_in_next_pattern,        SUB_G,          KeyCode_N);
  VimBind(N|MAP, vim_in_prev_pattern,        SUB_G,          KeyMod_Sft|KeyCode_N);

  VimBind(N|MAP, vim_prev_jump,                              KeyMod_Ctl|KeyCode_O);
  VimBind(N|MAP, vim_next_jump,                              KeyMod_Ctl|KeyCode_I);

  // Screen Adjust Binds
  VimBind(N|V|MAP, vim_half_page_up,                         KeyMod_Ctl|KeyCode_U);
  VimBind(N|V|MAP, vim_half_page_down,                       KeyMod_Ctl|KeyCode_D);
  VimBind(N|V|MAP, vim_whole_page_up,                        KeyMod_Ctl|KeyCode_B);
  VimBind(N|V|MAP, vim_whole_page_down,                      KeyMod_Ctl|KeyCode_F);
  VimBind(N|V|MAP, vim_line_down,                            KeyMod_Ctl|KeyMod_Sft|KeyCode_Y);
  VimBind(N|V|MAP, vim_line_down,                            KeyMod_Ctl|KeyCode_Y);
  VimBind(N|V|MAP, vim_line_up,                              KeyMod_Ctl|KeyMod_Sft|KeyCode_E);
  VimBind(N|V|MAP, vim_line_up,                              KeyMod_Ctl|KeyCode_E);
  VimBind(N|V|MAP, vim_scroll_screen_top,         SUB_Z,     KeyCode_T);
  VimBind(N|V|MAP, vim_scroll_screen_mid,         SUB_Z,     KeyCode_Z);
  VimBind(N|V|MAP, vim_scroll_screen_bot,         SUB_Z,     KeyCode_B);

  // Miscellaneous Binds
  VimBind(N|V|MAP, vim_set_mark,                             KeyCode_M);
  VimBind(N|V|MAP, vim_goto_mark,                            KeyCode_Tick);
  VimBind(N|V|MAP, vim_goto_mark,                            KeyCode_Quote);
  VimBind(N|V|MAP, vim_toggle_macro,                         KeyCode_Q);
  VimBind(N|V|MAP, vim_play_macro,                           KeyMod_Sft|KeyCode_2);

  //*****************************************************************************
  // My Bindings
  //*****************************************************************************

  VimBind(N|MAP, vim_open_file_in_quotes,      SUB_Leader,   KeyCode_F);
  VimBind(N|MAP, vim_interactive_open_or_new,  SUB_Leader,   KeyCode_E);
  VimBind(N|MAP, quick_swap_buffer,            SUB_RBracket, KeyCode_B);

  VimBind(N|MAP, custom_jump_to_definition,                  KeyMod_Sft|KeyCode_K);
  // VimBind(N|MAP, qol_jump_to_definition,                     KeyMod_Sft|KeyCode_K);
  // VimBind(N|MAP, vim_goto_definition,                        KeyMod_Alt|KeyCode_K);

  VimBind(N|MAP, vim_next_4coder_jump,                       KeyCode_N);
  VimBind(N|MAP, vim_prev_4coder_jump,                       KeyMod_Sft|KeyCode_N);

  // TODO(bcall): if indentint/outdenting in visual mode then stay in visual mode
  VimBind(N|V|MAP, vim_request_indent,                       KeyCode_Tab);
  VimBind(N|V|MAP, vim_request_outdent,                      KeyMod_Sft|KeyCode_Tab);

  VimBind(N|MAP, vim_lowercase,               SUB_G,         KeyCode_U);
  VimBind(N|MAP, vim_uppercase,               SUB_G,         KeyMod_Sft|KeyCode_U);
  VimBind(V|MAP, vim_lowercase,                              KeyCode_U);
  VimBind(V|MAP, vim_uppercase,                              KeyMod_Sft|KeyCode_U);
  
  VimBind(I|MAP, word_complete_drop_down,                    KeyCode_Tab);
  
  VimBind(N|V|MAP, search_next,                              KeyCode_N);
  VimBind(N|V|MAP, search_prev,                              KeyMod_Sft|KeyCode_N);

  // Window Binds
  VimBind(N|MAP, vim_change_x0, KeyMod_Ctl|KeyCode_H);
  VimBind(N|MAP, vim_change_y1, KeyMod_Ctl|KeyCode_J);
  VimBind(N|MAP, vim_change_y0, KeyMod_Ctl|KeyCode_K);
  VimBind(N|MAP, vim_change_x1, KeyMod_Ctl|KeyCode_L);

  //*****************************************************************************
  // Untouched/Changed Defaults
  //*****************************************************************************

  // VimBind(MAP, vim_inc_buffer_peek,                 (KeyMod_Ctl|KeyCode_RightBracket));
  // VimBind(MAP, vim_dec_buffer_peek,                 (KeyMod_Ctl|KeyCode_LeftBracket));
  // VimBind(MAP, vim_toggle_show_buffer_peek,         (KeyMod_Ctl|KeyCode_Return));
  // VimBind(MAP, vim_toggle_show_buffer_peek,     (KeyMod_Ctl|KeyMod_Sft|KeyCode_Return));
  // VimBind(MAP, vim_scoll_buffer_peek_up,        (KeyMod_Ctl|KeyMod_Sft|KeyCode_U));
  // VimBind(MAP, vim_scoll_buffer_peek_down,      (KeyMod_Ctl|KeyMod_Sft|KeyCode_D));

  // VimBind(N|V|MAP, set_mark,                        (KeyMod_Ctl|KeyCode_Space));
  // VimBind(N|MAP, vim_open_file_in_quotes,         SUB_G, KeyCode_F);
  // VimBind(N|MAP, vim_interactive_open_or_new,   (KeyMod_Ctl|KeyMod_Sft|KeyCode_O));
  // VimBind(N|MAP, quick_swap_buffer,             (KeyMod_Sft|KeyMod_Ctl|KeyCode_6));
  // VimBind(N|MAP, vim_goto_definition,               (KeyMod_Ctl|KeyCode_W));
  // VimBind(N|MAP, vim_next_4coder_jump,              (KeyMod_Alt|KeyCode_N));
  // VimBind(N|MAP, vim_prev_4coder_jump,          (KeyMod_Sft|KeyMod_Alt|KeyCode_N));
  // VimBind(N|MAP, vim_first_4coder_jump,         (KeyMod_Sft|KeyMod_Alt|KeyCode_M));
  // VimBind(N|I|MAP, view_buffer_other_panel,         (KeyMod_Ctl|KeyCode_1));
  // VimBind(N|I|MAP, swap_panels,                     (KeyMod_Ctl|KeyCode_2));

  // VimBind(N|V|MAP, vim_request_auto_indent,              KeyCode_Equal);
  // VimBind(N|V|MAP, vim_lowercase,            SUB_G,      KeyCode_U);
  // VimBind(N|V|MAP, vim_uppercase,            SUB_G, (KeyMod_Sft|KeyCode_U));
  // VimBind(N|V|MAP, vim_toggle_case,          SUB_G, (KeyMod_Sft|KeyCode_Tick));
  // VimBind(N|V|MAP, vim_request_indent,              (KeyMod_Sft|KeyCode_Period));
  // VimBind(N|V|MAP, vim_request_outdent,             (KeyMod_Sft|KeyCode_Comma));
  // VimBind(N|V|MAP, vim_request_fold,              SUB_Z, KeyCode_F);
  // VimBind(N|V|MAP, fold_toggle_cursor,            SUB_Z, KeyCode_A);
  // VimBind(N|V|MAP, fold_pop_cursor,               SUB_Z, KeyCode_D);
  // VimBind(V|MAP,   vim_toggle_case,                 (KeyMod_Sft|KeyCode_Tick));
  // VimBind(V|MAP,   vim_lowercase,                        KeyCode_U);
  // VimBind(V|MAP,   vim_uppercase,                   (KeyMod_Sft|KeyCode_U));

  #undef I
  #undef N
  #undef V
  #undef MAP
}
