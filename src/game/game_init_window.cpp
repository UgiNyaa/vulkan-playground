#include <game/game.hpp>

void Game::init_window()
{
    init_window_xcb();
}

void Game::init_window_xcb()
{
    uint32_t value_mask, value_list[32];

    window = xcb_generate_id(connection);

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window
    (
        connection,
        XCB_COPY_FROM_PARENT,
        window,
        screen->root,
        0, 0,
        width, height,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        value_mask,
        value_list
    );

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom
    (
        connection,
        1,
        12,
        "WM_PROTOCOLS"
    );
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply
    (
        connection,
        cookie,
        0
    );

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom
    (
        connection,
        1,
        16,
        "WM_DELETE_WINDOW"
    );
    atomWmDeleteWindow = xcb_intern_atom_reply(connection, cookie2, 0);

    xcb_change_property
    (
        connection,
        XCB_PROP_MODE_REPLACE,
        window,
        (*reply).atom,
        4,
        32,
        1,
        &(*atomWmDeleteWindow).atom
    );

    free(reply);

    xcb_map_window(connection, window);

    const uint32_t coords[] = {100, 100};
    xcb_configure_window
    (
        connection,
        window,
        XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
        coords
    );
    xcb_flush(connection);

    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(connection)))
        if ((e->response_type & ~0x80) == XCB_EXPOSE)
            break;
}
