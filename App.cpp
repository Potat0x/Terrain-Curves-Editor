#include "App.hpp"

App::App()
{
    bool fullscreen = false;
    sf::Vector2f winsize(800, 600);
    app_title = "Terrain Curves Editor";

    if(fullscreen)
    {
        window.create(sf::VideoMode::getDesktopMode(), app_title, sf::Style::Fullscreen);
    }
    else
    {
        window.create(sf::VideoMode(winsize.x, winsize.y), app_title);
        if(winsize.x < sf::VideoMode::getDesktopMode().width && winsize.y < sf::VideoMode::getDesktopMode().height)
            window.setPosition(sf::Vector2i((sf::VideoMode::getDesktopMode().width-winsize.x)/2, (sf::VideoMode::getDesktopMode().height-winsize.y)/2));
        else window.setPosition(sf::Vector2i(0, 0));
    }
    gui = make_shared<Gui>(window);
    Editor::get_editor().gui = gui;

    Inputs::win = &window;

    if(fullscreen)
        view = sf::View(sf::Vector2f(sf::VideoMode::getDesktopMode().width/2, sf::VideoMode::getDesktopMode().height/2), sf::Vector2f(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height));
    else view = sf::View(sf::Vector2f(winsize.x/2, winsize.y/2), sf::Vector2f(winsize.x, winsize.y));

    window.setVerticalSyncEnabled(true);
    window.resetGLStates();
}

App & App::get_app()
{
    static App App;
    return App;
}

string App::get_app_title()
{
    return app_title;
}

sf::RenderWindow & App::get_sfml_window()
{
    return window;
}

void App::run()
{
    Editor::get_editor().create_snapshot();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(!window.hasFocus())
                break;

            if(event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                close();
            else if(event.type == sf::Event::MouseWheelMoved)
                zoom_view(event.mouseWheel.delta);
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return)
                gui->filedialog.enterKeyPressed();
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
                {
                    gui->save_file();
                }
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
                {
                    gui->show_new_file_window(true);
                }
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
                {
                    gui->open_file();
                }
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
                {
                    gui->export_to_file();
                }
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
                {
                    gui->switch_grid_usage();
                }
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
                {
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
                    {
                        Editor::get_editor().redo();
                    }
                    else
                    {
                        Editor::get_editor().undo();
                    }
                }
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
                {
                    Editor::get_editor().copy();
                }
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::V)
            {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
                {
                    Editor::get_editor().paste();
                }
            }

            else if (event.type == sf::Event::Resized)
            {
                sf::Vector2f vc = view.getCenter();
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                view.reset(visibleArea);
                view.setCenter(vc);
                window.setView(view);
                gui->window_resize();
            }
            else gui->handle_event(event);
        }

        if(window.hasFocus())
        {
            window.setView(view);
            Inputs::getInputs().update();
            pan_view();

            Editor::get_editor().update();
            Editor::get_editor().draw();

            gui->draw();
            window.display();
            fps_counter.update();
            gui->update_fps(fps_counter.get());
        }
        else sf::sleep(sf::milliseconds(100));
    }
}

void App::close(bool confirmed)//confirmed = false
{
    if(confirmed)
        window.close();
    else if(Editor::get_editor().check_if_unsaved_changes())
    {
        gui->show_unsaved_ch_window(Editor::get_editor().get_current_filename());
    }
    else window.close();
}

void App::pan_view()
{
    if(Point::onmouse)
        return;

    if(Inputs::getInputs().getInputs().get(sf::Mouse::Middle) && pan_mouse_mid_released)
    {
        pan_mouse_mid_released = false;
        pan_start = Inputs::getInputs().mousePosOnWindow();
    }
    else if(Inputs::getInputs().get(sf::Mouse::Middle) == false)
    {
        pan_mouse_mid_released = true;
    }
    else if(Inputs::getInputs().get(sf::Mouse::Middle))
    {
        pan_diff = Inputs::getInputs().mousePosOnWindow() - pan_start;
        pan_start = Inputs::getInputs().mousePosOnWindow();
        view.setCenter(view.getCenter() - pan_diff);
    }
}

void App::zoom_view(float delta)
{
    if(delta < 0)
        view.zoom(1.05f);
    else if(delta > 0)
        view.zoom(0.95f);
}
