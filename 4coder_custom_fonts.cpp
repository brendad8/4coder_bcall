
// In your custom layer header or globals area
struct Custom_Fonts {
    Face_ID regular;
    Face_ID bold;
    Face_ID italic;
};

global Custom_Fonts global_fonts = {};


CUSTOM_COMMAND_SIG(custom_init_fonts)
CUSTOM_DOC("Custom initialization to load multiple fonts.")
{
    Scratch_Block scratch(app);

    Font_Load_Location secondary_font_location = {};
    secondary_font_location.file_name = string_u8_litexpr("C:\\Users\\guppy\\4cc\\build\\fonts\\CascadiaMono-Bold.ttf");

    Face_Description secondary_face_desc = {};
    secondary_face_desc.font = secondary_font_location;
    secondary_face_desc.parameters.pt_size = 16;
    secondary_face_desc.parameters.hinting = false;

    Face_ID secondary_face_id = face_id_from_description(app, &secondary_face_desc);
    if (secondary_face_id == 0)
    {
      secondary_face_id = try_create_new_face(app, &secondary_face_desc);
    }
    global_fonts.bold = secondary_face_id;

}
