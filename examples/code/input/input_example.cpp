#include "debug_render.h"
#include "file_system.h"
#include "input.h"
#include "loader.h"
#include "memory.h"
#include "pen.h"
#include "pen_string.h"
#include "renderer.h"
#include "str/Str.h"
#include "threads.h"
#include "timer.h"

pen::window_creation_params pen_window{
    1280,   // width
    720,    // height
    4,      // MSAA samples
    "input" // window title / process name
};

typedef struct vertex
{
    float x, y, z, w;
} vertex;

typedef struct textured_vertex
{
    float x, y, z, w;
    float u, v;
} textured_vertex;

PEN_TRV pen::user_entry(void* params)
{
    // unpack the params passed to the thread and signal to the engine it ok to proceed
    pen::job_thread_params* job_params = (pen::job_thread_params*)params;
    pen::job*               p_thread_info = job_params->job_info;
    pen::semaphore_post(p_thread_info->p_sem_continue, 1);

    // initialise the debug render system
    put::dbg::init();

    // create 2 clear states one for the render target and one for the main screen, so we can see the difference
    static pen::clear_state cs = {
        0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0x00, PEN_CLEAR_COLOUR_BUFFER | PEN_CLEAR_DEPTH_BUFFER,
    };

    u32 clear_state = pen::renderer_create_clear_state(cs);

    // raster state
    pen::rasteriser_state_creation_params rcp;
    pen::memory_zero(&rcp, sizeof(rasteriser_state_creation_params));
    rcp.fill_mode = PEN_FILL_SOLID;
    rcp.cull_mode = PEN_CULL_BACK;
    rcp.depth_bias_clamp = 0.0f;
    rcp.sloped_scale_depth_bias = 0.0f;
    rcp.depth_clip_enable = true;

    u32 raster_state = pen::renderer_create_rasterizer_state(rcp);

    // cb
    pen::buffer_creation_params bcp;
    bcp.usage_flags = PEN_USAGE_DYNAMIC;
    bcp.bind_flags = PEN_BIND_CONSTANT_BUFFER;
    bcp.cpu_access_flags = PEN_CPU_ACCESS_WRITE;
    bcp.buffer_size = sizeof(float) * 16;
    bcp.data = (void*)nullptr;

    u32 cb_2d_view = pen::renderer_create_buffer(bcp);

    while (1)
    {
        // viewport
        pen::viewport vp = {0.0f, 0.0f, (f32)pen_window.width, (f32)pen_window.height, 0.0f, 1.0f};

        pen::renderer_set_rasterizer_state(raster_state);

        // bind back buffer and clear
        pen::renderer_set_viewport(vp);
        pen::renderer_set_targets(PEN_BACK_BUFFER_COLOUR, PEN_BACK_BUFFER_DEPTH);
        pen::renderer_clear(clear_state);

        put::dbg::add_text_2f(10.0f, 10.0f, vp, vec4f(0.0f, 1.0f, 0.0f, 1.0f), "%s", "Input Test");

        const pen::mouse_state& ms = pen::input_get_mouse_state();

        // mouse
        vec2f mouse_pos = vec2f((f32)ms.x, vp.height - (f32)ms.y);
        vec2f mouse_quad_size = vec2f(5.0f, 5.0f);
        put::dbg::add_quad_2f(mouse_pos, mouse_quad_size, vec4f::cyan());

        put::dbg::add_text_2f(10.0f, 20.0f, vp, vec4f(1.0f, 1.0f, 1.0f, 1.0f),
                              "mouse down : left %i, middle %i, right %i: mouse_wheel %i", ms.buttons[PEN_MOUSE_L],
                              ms.buttons[PEN_MOUSE_M], ms.buttons[PEN_MOUSE_R], ms.wheel);

        // key down
        Str key_msg = "key down: ";

        for (s32 key = 0; key < PK_ARRAY_SIZE; ++key)
        {
            if (pen::input_key(key))
            {
                key_msg.append("[");
                key_msg.append(pen::input_get_key_str(key));
                key_msg.append("]");
            }
        }
        key_msg.append("\n");

        put::dbg::add_text_2f(10.0f, 30.0f, vp, vec4f(1.0f, 1.0f, 1.0f, 1.0f), "%s", key_msg.c_str());

        Str ascii_msg = "character down: ";
        for (s32 key = 0; key < PK_ARRAY_SIZE; ++key)
        {
            if (pen::input_get_unicode_key(key))
            {
                ascii_msg.append("[");
                ascii_msg.append(key);
                ascii_msg.append("]");
            }
        }
        ascii_msg.append("\n");

        put::dbg::add_text_2f(10.0f, 40.0f, vp, vec4f(1.0f, 1.0f, 1.0f, 1.0f), "%s", ascii_msg.c_str());

        Str press_msg = "character press: ";
        for (s32 key = 0; key < PK_ARRAY_SIZE; ++key)
        {
            if (pen::input_is_key_pressed(key))
            {
                press_msg.append("[");
                key_msg.append(pen::input_get_key_str(key));
                press_msg.append("]");
            }
        }
        press_msg.append("\n");
        put::dbg::add_text_2f(10.0f, 50.0f, vp, vec4f(1.0f, 1.0f, 1.0f, 1.0f), "%s", press_msg.c_str());

        u32 num_gamepads = input_get_num_gamepads();
        put::dbg::add_text_2f(10.0f, 60.0f, vp, vec4f(1.0f, 1.0f, 1.0f, 1.0f), "Gamepads: %i", num_gamepads);

        f32 ypos = 70.0f;
        for (u32 i = 0; i < num_gamepads; ++i)
        {
            gamepad_state gs;
            pen::input_get_gamepad_state(i, gs);

            put::dbg::add_text_2f(10.0f, ypos, vp, vec4f(1.0f, 1.0f, 1.0f, 1.0f), "Vendor ID: [%i] : Product ID [%i]",
                                  gs.vendor_id, gs.product_id);
            ypos += 10.0f;

            for (u32 b = 0; b < 16; ++b)
            {
                put::dbg::add_text_2f(10.0f, ypos, vp, vec4f(1.0f, 1.0f, 1.0f, 1.0f), "Button: %i : [%i]", b, gs.button[b]);
                ypos += 10.0f;
            }

            f32 xpos = 150.0f;
            for (u32 r = 0; r < 4; ++r)
            {
                ypos = 80.0f;
                for (u32 a = 0; a < 16; ++a)
                {
                    u32 ai = r * 16 + a;
                    put::dbg::add_text_2f(xpos, ypos, vp, vec4f(1.0f, 1.0f, 1.0f, 1.0f), "Axis: %i : [%f]", ai, gs.axis[ai]);
                    ypos += 10.0f;
                }

                xpos += 150.0f;
            }
        }

        // create 2d view proj matrix
        float W = 2.0f / vp.width;
        float H = 2.0f / vp.height;
        float mvp[4][4] = {{W, 0.0, 0.0, 0.0}, {0.0, H, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {-1.0, -1.0, 0.0, 1.0}};
        pen::renderer_update_buffer(cb_2d_view, mvp, sizeof(mvp), 0);

        put::dbg::render_2d(cb_2d_view);

        put::dbg::render_2d(cb_2d_view);

        // present
        pen::renderer_present();
        pen::renderer_consume_cmd_buffer();

        // msg from the engine we want to terminate
        if (pen::semaphore_try_wait(p_thread_info->p_sem_exit))
        {
            break;
        }
    }

    // clean up mem here

    // signal to the engine the thread has finished
    pen::semaphore_post(p_thread_info->p_sem_terminated, 1);

    return PEN_THREAD_OK;
}
