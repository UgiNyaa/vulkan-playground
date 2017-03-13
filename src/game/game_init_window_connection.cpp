#include <game/game.hpp>

void Game::init_window_connection()
{
    init_window_connection_xcb();
}

void Game::init_window_connection_xcb()
{
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    connection = xcb_connect(NULL, &scr);
    check(connection != NULL);
    check(!xcb_connection_has_error(connection));

    setup = xcb_get_setup(connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0) xcb_screen_next(&iter);

    screen = iter.data;
}
