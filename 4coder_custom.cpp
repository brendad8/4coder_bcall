
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#pragma warning(disable: 4042)
#pragma warning(disable: 4067)

#include <Windows.h>
#include <Ole2.h>
#include <oaidl.h>
#endif

#include "4coder_default_include.cpp"

#include "4coder_vim/4coder_vimrc.h"
#include "4coder_vim/4coder_vim_include.h"
#include "4coder_vim/4coder_vim_include.cpp"

#include "4coder_fleury/4coder_fleury_include.cpp"

#include "4coder_custom_bindings.cpp"
#include "4coder_custom_commands.cpp"
#include "4coder_custom_draw.cpp"
#include "4coder_custom_fonts.cpp"
#include "4coder_custom_hooks.cpp"

#if !defined(META_PASS)
#include "generated/managed_id_metadata.cpp"
#endif

VIM_TEXT_OBJECT_SIG(EXAMPLE_object_camel){
  Range_i64 range = {};
  Scratch_Block scratch(app);
  Range_i64 line_range = get_line_range_from_pos(app, buffer, cursor_pos);
  i64 s = line_range.min;
  u8 *line_text = push_buffer_range(app, scratch, buffer, line_range).str;
  u8 c = line_text[cursor_pos-s];
  if(!character_is_alpha_numeric(c)){ return {}; }
  cursor_pos += line_text[cursor_pos-s] == '_';
  range.min = range.max = cursor_pos;

  b32 valid=false;
  for(; range.min>0; range.min--){
    c = line_text[range.min-s];
    if(!character_is_alpha_numeric(c) || c == '_'){ valid = true; break; }
  }
  if(!valid){ return {}; }

  valid=false;
  for(; range.max>0; range.max++){
    c = line_text[range.max-s];
    if(!character_is_alpha_numeric(c) || c == '_'){ valid = true; break; }
  }
  if(!valid){ return {}; }

  range.min += (vim_state.params.clusivity != VIM_Inclusive || line_text[range.min-s] != '_');
  range.max -= (vim_state.params.clusivity != VIM_Inclusive || line_text[range.max-s] != '_');
  if(range.min >= range.max){ range = {}; }

  return range;
}

function void EXAMPLE_make_vim_request(Application_Links *app, EXAMPLE_Vim_Request request){
  vim_make_request(app, Vim_Request_Type(VIM_REQUEST_COUNT + request));
}

VIM_REQUEST_SIG(EXAMPLE_apply_title){
  Scratch_Block scratch(app);
  String_Const_u8 text = push_buffer_range(app, scratch, buffer, range);
  u8 prev = buffer_get_char(app, buffer, range.min-1);
  for(i32 i=0; i<text.size; i++){
    text.str[i] += u8(i32('A' - 'a')*((!character_is_alpha(prev) || prev == '_') &&
                                      character_is_lower(text.str[i])));
    prev = text.str[i];
  }
  buffer_replace_range(app, buffer, range, text);
  buffer_post_fade(app, buffer, 0.667f, range, fcolor_resolve(fcolor_id(defcolor_paste)));
}

VIM_REQUEST_SIG(EXAMPLE_apply_rot13){
  Scratch_Block scratch(app);
  String_Const_u8 text = push_buffer_range(app, scratch, buffer, range);
  for(i32 i=0; i<text.size; i++){
    if(0){}
    else if(character_is_lower(text.str[i])){
      text.str[i] = (text.str[i] + 13) - 26*(text.str[i] > ('z'-13));
    }
    else if(character_is_upper(text.str[i])){
      text.str[i] = (text.str[i] + 13) - 26*(text.str[i] > ('Z'-13));
    }
  }
  buffer_replace_range(app, buffer, range, text);
  buffer_post_fade(app, buffer, 0.667f, range, fcolor_resolve(fcolor_id(defcolor_paste)));
}
VIM_COMMAND_SIG(EXAMPLE_request_title){ EXAMPLE_make_vim_request(app, EXAMPLE_REQUEST_Title); }
VIM_COMMAND_SIG(EXAMPLE_request_rot13){ EXAMPLE_make_vim_request(app, EXAMPLE_REQUEST_Rot13); }

void 
custom_layer_init(Application_Links *app)
{
  default_framework_init(app);

  Scratch_Block scratch(app);
  String_Const_u8 bin_path = system_get_path(scratch, SystemPath_Binary);

  // NOTE(rjf): Small code font.
  {
      Face_ID face_that_should_totally_be_there = get_face_id(app, 0);

      Face_Description normal_code_desc = get_face_description(app, get_face_id(app, 0));
      
      Face_Description desc = {0};
      {
          desc.font.file_name =  push_u8_stringf(scratch, "%.*sfonts/CascadiaMono-Regular.ttf", string_expand(bin_path));
          desc.parameters.pt_size = normal_code_desc.parameters.pt_size - 1;
          desc.parameters.bold = 1;
          desc.parameters.italic = 1;
          desc.parameters.hinting = 0;
      }

      if(IsFileReadable(desc.font.file_name))
      {
          global_small_code_face = try_create_new_face(app, &desc);
      }
      else
      {
          global_small_code_face = face_that_should_totally_be_there;
      }
  }
  
  // *********************************************************************
  //  4coder_fleury setup
  // *********************************************************************
  
  {
    F4_Index_Initialize();
    F4_RegisterLanguages();
  }

  // TODO(bcall): figure out how to make keywords bold
  // custom_init_fonts(app);

  set_all_default_hooks(app);

  vim_request_vtable[VIM_REQUEST_COUNT + EXAMPLE_REQUEST_Title] = EXAMPLE_apply_title;
  vim_request_vtable[VIM_REQUEST_COUNT + EXAMPLE_REQUEST_Rot13] = EXAMPLE_apply_rot13;

  vim_text_object_vtable[VIM_TEXT_OBJECT_COUNT + EXAMPLE_OBJECT_camel] = {'_', (Vim_Text_Object_Func *)EXAMPLE_object_camel};
  vim_init(app);


  set_custom_hook(app, HookID_ViewEventHandler, vim_view_input_handler);
  set_custom_hook(app, HookID_BufferRegion, custom_buffer_region);
  set_custom_hook(app, HookID_RenderCaller, custom_render_caller);
  set_custom_hook(app, HookID_WholeScreenRenderCaller, vim_draw_whole_screen);
  set_custom_hook(app, HookID_Tick, custom_tick);
  set_custom_hook(app, HookID_SaveFile, vim_file_save_hook);
  set_custom_hook(app, HookID_BeginBuffer, custom_begin_buffer);
  set_custom_hook(app, HookID_ViewChangeBuffer, vim_view_change_buffer);

  Thread_Context *tctx = get_thread_context(app);
  mapping_init(tctx, &framework_mapping);
  String_ID global_map_id = vars_save_string_lit("keys_global");
  String_ID file_map_id = vars_save_string_lit("keys_file");
  String_ID code_map_id = vars_save_string_lit("keys_code");
  setup_essential_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
#if OS_MAC
  setup_mac_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
#else
  setup_default_mapping(&framework_mapping, global_map_id, file_map_id, code_map_id);
#endif

  setup_custom_bindings(app);

}
