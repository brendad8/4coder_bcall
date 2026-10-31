
struct defer__t{};
template <class F> struct defer_t{ F f; ~defer_t(){ f(); } };
template <class F> defer_t<F> operator^(defer__t _, F f){ return {f}; }
#define defer  auto glue(_defer_obj_, __LINE__)  =  defer__t{} ^ [&]()

CUSTOM_COMMAND_SIG(custom_jump_to_definition)
CUSTOM_DOC("Jump to the definition in the code index matching an identifier at the cursor")
{
  View_ID view = get_active_view(app, Access_Visible);
  if (view == 0){ return; }

  Scratch_Block scratch(app);
  String_Const_u8 query = push_token_or_word_under_active_cursor(app, scratch);

  code_index_lock();
  defer{ code_index_unlock(); };
  Code_Index_Note_List* list = code_index__list_from_string(query);

  // Prefer function definitions over declarations
  for (Code_Index_Note *note = list->first; note != 0; note = note->next_in_hash){
    if (!string_match(query, note->text)){ continue; }
    if (note->note_kind == CodeIndexNote_Function && note->parent != 0){
      Code_Index_Nest* scope = note->parent->next;
      if (scope != 0 && scope->kind == CodeIndexNest_Scope){
        point_stack_push_view_cursor(app, view);
        jump_to_location(app, view, note->file->buffer, note->pos.first);
        return;
      }
    }
  }

  for (Code_Index_Note *note = list->first; note != 0; note = note->next_in_hash){
    if (!string_match(query, note->text)){ continue; }
    point_stack_push_view_cursor(app, view);
    jump_to_location(app, view,  note->file->buffer, note->pos.first);
    return;
  }
}

CUSTOM_COMMAND_SIG(noh)
CUSTOM_DOC("Vim: Clear search") { vim_clear_search(app); }

CUSTOM_COMMAND_SIG(search_next)
CUSTOM_DOC("Vim: Goto next search entry") { vim_to_pattern_inner(app, 0); }

CUSTOM_COMMAND_SIG(search_prev)
CUSTOM_DOC("Vim: Goto prev search entry") { vim_to_pattern_inner(app, BufferSeekString_Backward); }

