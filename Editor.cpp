#include "Editor.hpp"

bool Editor::bg_pressed;
float Editor::draw_step;

Editor::Editor()
{
    Point::init();
    segments = 0;
    draw_lines = true;//segments
    draw_pts = true;
    draw_gr = true;
    draw_cur = true;
    selected_index = -1;
    selectall_released = true;
    draw_con = false;
    pressall_released = true;
    key_addnew_released = true;
    key_addnew_released_curve = true;
    drawarea_released = true;

    for(int i = 0; i < 4; i++)
    {
        area_shapes[i] = sf::RectangleShape(sf::Vector2f(16, 16));
        area_shapes[i].setFillColor(sf::Color::Cyan);
        area_shapes[i].setOrigin(area_shapes[i].getLocalBounds().width/2, area_shapes[i].getLocalBounds().height/2);
        area_lines[i] = sf::RectangleShape(sf::Vector2f(11,11));
        area_lines[i].setFillColor(sf::Color::White);
    }
    bg_pressed = false;
    unsaved_changes = false;
    points_dragged = false;
    area_type = 0;
    history_iter = 0;
    draw_step = 0.04f;
    file_pr = "42657a6965724375727665456469746f722046696c65";//hex "EditorCurveApp File"
}

Editor & Editor::get_editor()
{
    static Editor Editor;
    return Editor;
}

void Editor::draw_pixel(sf::Vector2f pos, sf::Color color = Colors::get().getColor(Colors::PIX), float a = 1)
{
    sf::RectangleShape shape(sf::Vector2f(a, a));
    shape.setPosition(pos.x, pos.y);
    shape.setFillColor(color);
    App::get_app().get_sfml_window().draw(shape);
}

sf::Vector2i Editor::get_grid_bounds()
{
    sf::Vector2i gb(App::get_app().get_sfml_window().getSize());
    for(unsigned int i = 0; i < points.size(); i++)
    {
        if(points.at(i)->get_position().x > gb.x)
            gb.x = points.at(i)->get_position().x;
        if(points.at(i)->get_position().y > gb.y)
            gb.y = points.at(i)->get_position().y;
    }

    gb.x = (gb.x/Point::grid.x)*Point::grid.x+Point::grid.x+1;
    gb.y = (gb.y/Point::grid.y)*Point::grid.y+Point::grid.y+1;
    return gb;
}

void Editor::draw_grid()
{
    if(!draw_gr)
        return;

    if(Point::use_grid)
    {
        sf::Vector2i bounds = get_grid_bounds();
        unsigned int x = 0;
        for(int i = 0; i < bounds.x; i+=Point::grid.x)
        {
            grid_line.setSize(sf::Vector2f(1, bounds.y));
            grid_line.setFillColor(Colors::get().getColor(Colors::GRID));
            grid_line.setPosition(i, 0);
            App::get_app().get_sfml_window().draw(grid_line);
            x++;
        }

        for(int i = 0; i < bounds.y; i+=Point::grid.y)
        {
            grid_line.setSize(sf::Vector2f(bounds.x, 1));
            grid_line.setFillColor(Colors::get().getColor(Colors::GRID));
            grid_line.setPosition(0, i);
            App::get_app().get_sfml_window().draw(grid_line);
        }
    }
}

void Editor::create_area()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && (Inputs::getInputs().get(sf::Mouse::Right) || Inputs::getInputs().get(sf::Mouse::Left)) )
    {
        if(drawarea_released)
        {
            if(Inputs::getInputs().get(sf::Mouse::Right))
                area_type = 1;
            else if(Inputs::getInputs().get(sf::Mouse::Left))
                area_type = 2;

            drawarea_start = Inputs::getInputs().mousePos();
            drawarea_released = false;
        }
        else
        {
            area_shapes[0].setPosition(drawarea_start);
            area_shapes[1].setPosition(drawarea_start.x, Inputs::getInputs().mousePos().y);
            area_shapes[2].setPosition(Inputs::getInputs().mousePos().x, drawarea_start.y);
            area_shapes[3].setPosition(Inputs::getInputs().mousePos());
        }
    }
    else if(!drawarea_released)
    {
        area_type = 0;
        drawarea_released = true;
        drawarea_start.x = 0;
        drawarea_start.y = 0;
        for(int i = 0; i < 4; i++)
            area_shapes[i].setPosition(drawarea_start);
    }

    float t = 1;
    area_lines[0].setSize(sf::Vector2f((area_shapes[3].getPosition().x-area_shapes[0].getPosition().x), t));
    area_lines[0].setPosition(area_shapes[0].getPosition());
    area_lines[1].setSize(sf::Vector2f(t, (area_shapes[0].getPosition().y-area_shapes[3].getPosition().y)));
    area_lines[1].setPosition(area_shapes[1].getPosition());
    area_lines[2].setSize(sf::Vector2f(t, (area_shapes[3].getPosition().y-area_shapes[2].getPosition().y)));
    area_lines[2].setPosition(area_shapes[2].getPosition());
    area_lines[3].setSize(sf::Vector2f((area_shapes[3].getPosition().x-area_shapes[1].getPosition().x), t));
    area_lines[3].setPosition(area_shapes[1].getPosition());

    for(int i = 0; i < 4; i++)
    {
        if(area_type == 1)
            area_lines[i].setFillColor(Colors::get().getColor(Colors::AREA_SELECTED));
        else if(area_type == 2)
            area_lines[i].setFillColor(Colors::get().getColor(Colors::AREA_PRESSED));
    }

    if(!drawarea_released && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && (Inputs::getInputs().get(sf::Mouse::Right) || Inputs::getInputs().get(sf::Mouse::Left)))
        if(area_shapes[0].getPosition().x != 0 && area_shapes[0].getPosition().y != 0)
            area_contain();
}

void Editor::area_contain()
{
    float w = area_shapes[3].getPosition().x-drawarea_start.x;
    float h = area_shapes[3].getPosition().y-drawarea_start.y;

    sf::Rect<float>rect(drawarea_start.x, drawarea_start.y, w, h);

    if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) )
    {
        for(unsigned int i = 0; i < points.size(); i++)
        {
            if(points.at(i)->selected)
                points.at(i)->set_selected(false);
            else if(points.at(i)->pressed)
                points.at(i)->set_pressed(false);
        }
    }

    for(unsigned int i = 0; i < points.size(); i++)
    {
        if(rect.contains(points.at(i)->get_position().x, points.at(i)->get_position().y))
        {
            if(area_type == 1)
                points.at(i)->set_selected(true);
            else if(area_type == 2)
                points.at(i)->set_pressed(true);
        }
    }
}

void Editor::draw_area()
{
    if(!drawarea_released)
    {
        for(int i = 0; i < 4; i++)
        {
            //area_shapes
            App::get_app().get_sfml_window().draw(area_lines[i]);
        }
    }
}

void Editor::select_all()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
    {
        if(selectall_released)
        {
            for(unsigned int i = 0; i < points.size(); i++)
            {
                if(!points.at(i)->selected)
                    points.at(i)->set_selected(true);
            }
        }
    }
    else
        selectall_released = true;
}

void Editor::press_all()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
    {
        if(pressall_released)
        {
            for(unsigned int i = 0; i < points.size(); i++)
            {
                if(!points.at(i)->pressed)
                    points.at(i)->set_pressed(true);
            }
        }
    }
    else
        pressall_released = true;
}

void Editor::check_connection(const shared_ptr<Point> & test)
{
    for(unsigned int p = 0; p < points.size(); p++)
    {
        for(unsigned int i = 0; i < points.at(p)->connected.size(); i++)
        {
            if(points.at(p)->connected.at(i).node_ptr1 == test || points.at(p)->connected.at(i).node_ptr2 == test)
            {
                points.at(p)->connected.begin();
                points.at(p)->connected.erase(points.at(p)->connected.begin()+i);
                check_connection(test);
            }
        }
    }
}

void Editor::delete_point(const shared_ptr<Point> & ptr)
{
    if(points.size()>0)
        for(unsigned int i = 0; i < points.size(); i++)
        {
            if(points.at(i) == ptr)
            {
                points.erase(points.begin()+i);
            }
        }
}

void Editor::delete_points()
{
    if(Point::selected_ptrs.size() > 0)
    {
        while(true)
        {
            if(Point::selected_ptrs.size() == 0)
            {
                create_snapshot();
                return;
            }

            shared_ptr<Point> ptr = Point::selected_ptrs.at(0);
            Point::erase_selected(ptr);
            Point::erase_pressed(ptr);
            check_connection(ptr);
            delete_point(ptr);
        }
    }
}

void Editor::add_curve_on_cursor()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Y) && key_addnew_released_curve == true)
    {
        shared_ptr<Point> ptr1 = make_shared<Point>(sf::Vector2f(Inputs::getInputs().mousePos().x-50, Inputs::getInputs().mousePos().y));
        points.push_back(ptr1);
        shared_ptr<Point> ptr2 = make_shared<Point>(sf::Vector2f(Inputs::getInputs().mousePos().x+50, Inputs::getInputs().mousePos().y));
        points.push_back(ptr2);
        shared_ptr<Point> ptr3 = make_shared<Point>(sf::Vector2f(Inputs::getInputs().mousePos().x, Inputs::getInputs().mousePos().y));
        points.push_back(ptr3);

        ptr3->connect_this(ptr1, ptr2);
        key_addnew_released_curve = false;
        create_snapshot();
    }
    else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
        key_addnew_released_curve = true;
}

void Editor::add_point_on_cursor()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::N) && key_addnew_released == true)
    {
        shared_ptr<Point> ptr = make_shared<Point>(Inputs::getInputs().mousePos());
        points.push_back(ptr);
        key_addnew_released = false;

        if(Point::use_grid == true)
            ptr->set_on_grid_position();

        create_snapshot();
    }
    else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::N))
        key_addnew_released = true;
}

void Editor::delete_all()
{
    history.clear();
    points.clear();
    Point::selected_ptrs.clear();
    Point::pressed_ptrs.clear();
}

void Editor::select_point()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        if(Point::connect())
            create_snapshot();
        sf::sleep(sf::milliseconds(400));
    }

    Point::onmouse = false;
    for(unsigned int i = 0; i < points.size(); i++)
    {
        points.at(i)->select_press(!bg_pressed);
    }

    if(Inputs::getInputs().get(sf::Mouse::Left))
    {
        bool diff = true;
        for(unsigned int i = 0; i < points.size(); i++)
        {
            if(points.at(i)->mouse_contain())
            {
                diff = false;
            }
        }

        if(diff && Point::pressed_ptrs.size() > 0 && !bg_pressed)
        {
            Point::calculate_diff();
            bg_pressed = true;
        }
        else if(bg_pressed && Inputs::getInputs().get(sf::Mouse::Left) && Point::pressed_ptrs.size() > 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        {
            points_dragged = true;
            Point::move_group();
        }

    }
    else bg_pressed = false;

    if(points_dragged && !Inputs::getInputs().get(sf::Mouse::Left))
    {
        points_dragged = false;
        create_snapshot();
    }
}

void Editor::move_curve()
{
    /* true -> edit | false -> no */
    if(Inputs::getInputs().get(sf::Mouse::Left))
    {
        selected_index = -1;
        sf::Vector2f mouse_pos(Inputs::getInputs().mousePos());

        if(Point::pressed_ptrs.size() == 1 && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        {
            for(unsigned int i = 0; i < points.size(); i++)
            {
                points.at(i)->move();
            }
            points_dragged = true;
        }
    }
}

void Editor::draw_line(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Color & color)
{
    a = abs(p1.x-p2.x);
    b = abs(p1.y-p2.y);
    c = sqrt(pow(a, 2) + pow(b, 2));
    drl_angle = atan2(p1.y-p2.y,p1.x-p2.x);

    line.setSize(sf::Vector2f(c, 1));
    line.setOrigin(line.getLocalBounds().width/2.0f, line.getLocalBounds().height/2.0f);
    line.setPosition((p1.x+p2.x)/2.0f, (p1.y+p2.y)/2.0f);
    line.setRotation(drl_angle* 180.0f / 3.1415926f);
    //line.setFillColor(rand_color());
    line.setFillColor(color);
    App::get_app().get_sfml_window().draw(line);
}

void Editor::draw_connections()
{
    if(draw_con)
        for(unsigned int i = 0; i < points.size(); i++)
        {
            for(unsigned int c = 0; c < points.at(i)->connected.size(); c++)
            {
                draw_line(points.at(i)->get_position(), points.at(i)->connected.at(c).node_ptr1->get_position(), Colors::get().getColor(Colors::LINK));
                draw_line(points.at(i)->get_position(), points.at(i)->connected.at(c).node_ptr2->get_position(), Colors::get().getColor(Colors::LINK));
            }
        }
}

void Editor::draw(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Vector2f& p3)
{
    sf::Vector2f p4 = p3;

    float x, y;

    sf::Vector2f last(0, 0);
    //float dist = 0;
    for(float t = 0; t <= 1; t +=draw_step)
    {
        x = pow(( 1 - t ), 3)*p1.x + 3 * pow(( 1 - t ), 2) * t * p2.x + 3*( 1 - t )*pow(t, 2) * p3.x+pow(t, 3)*p4.x;
        y = pow(( 1 - t ), 3)*p1.y + 3 * pow(( 1 - t ), 2) * t * p2.y + 3*( 1 - t )*pow(t, 2) * p3.y+pow(t, 3)*p4.y;

        if(draw_cur)
            draw_pixel(sf::Vector2f(x, y));//segment vertex

        if(t > 0)
        {
            if(draw_lines)
                draw_line(last, sf::Vector2f(x, y), Colors::get().getColor(Colors::CURVE));

            segments++;
            /*
            dist += abs(sqrt( pow(last.x-x, 2)  +  pow(last.y-y, 2) ));
            float a = abs(last.x-x);
            float b = abs(last.y-y);
            float c = sqrt(pow(a, 2) + pow(b, 2));
            */
        }
        last = sf::Vector2f(x, y);
    }
    /*drawing ideal curve
    for(float t = 0; t <= 1; t +=0.005f)
    {
        x = pow(( 1 - t ), 3)*p1.x + 3 * pow(( 1 - t ), 2) * t * p2.x + 3*( 1 - t )*pow(t, 2) * p3.x+pow(t, 3)*p4.x;
        y = pow(( 1 - t ), 3)*p1.y + 3 * pow(( 1 - t ), 2) * t * p2.y + 3*( 1 - t )*pow(t, 2) * p3.y+pow(t, 3)*p4.y;
        draw_pixel(sf::Vector2f(x, y));
    }*/
}

void Editor::draw_curve()
{
    sf::Vector2f p1, p2, p3;
    segments = 0;
    for(unsigned int p = 0; p < points.size(); p++)
    {
        for(unsigned int i = 0; i < points.at(p)->connected.size(); i++)
        {
            p1 = points.at(p)->connected.at(i).node_ptr1->get_position();
            p2 = points.at(p)->get_position();
            p3 = points.at(p)->connected.at(i).node_ptr2->get_position();
            draw(p1, p2, p3);
        }
    }
}

void Editor::draw_points()
{
    if(draw_pts)
        for(unsigned int i = 0; i < points.size(); i++)
            points.at(i)->draw(&App::get_app().get_sfml_window());

}

void Editor::create_idents()
{
    for(unsigned int i = 0; i < points.size(); i++)
        points.at(i)->ident = i;
}

shared_ptr<Point> Editor::get_point_by_id(unsigned int id, bool pasted_only)
{
    for(unsigned int i = 0; i < points.size(); i++)
    {
        if(points.at(i)->ident == id)
        {
            if(pasted_only && points.at(i)->pasted)
            {
                return points.at(i);
            }
            else if(!pasted_only)
            {
                return points.at(i);
            }
        }
    }
    return nullptr;
}

void Editor::save_segments_to_file(string filename)
{
    fstream file;
    file.open(filename, fstream::out);

    if(file.fail())
    {
        gui->show_info("Error", "Failed to open\n"+filename);
        return;
    }

    sf::Vector2f p1, p2, p3, p4;
    int fcnt = 0;
    for(unsigned int p = 0; p < points.size(); p++)
    {
        if(points.at(p)->connected.size() > 0)
        {
            for(unsigned int i = 0; i < points.at(p)->connected.size(); i++)
            {
                p1 = points.at(p)->connected.at(i).node_ptr1->get_position();
                p2 = points.at(p)->get_position();
                p3 = points.at(p)->connected.at(i).node_ptr2->get_position();
                p4 = p3;
                {
                    sf::Vector2f last(0, 0);
                    float x, y;
                    for(float t = 0; t <= 1; t +=draw_step)
                    {
                        x = pow(( 1 - t ), 3)*p1.x + 3 * pow(( 1 - t ), 2) * t * p2.x + 3*( 1 - t )*pow(t, 2) * p3.x+pow(t, 3)*p4.x;
                        y = pow(( 1 - t ), 3)*p1.y + 3 * pow(( 1 - t ), 2) * t * p2.y + 3*( 1 - t )*pow(t, 2) * p3.y+pow(t, 3)*p4.y;
                        if(t > 0)
                        {
                            file<<last.x<<" "<<last.y<<" "<<x<<" "<<y<<endl;
                            fcnt++;
                        }
                        last = sf::Vector2f(x, y);
                    }
                }
            }
        }
    }

    file.close();
    file.open(filename, fstream::in);

    int lines_count = 0;
    string line;
    while (getline(file, line))
        ++lines_count;

    if(fcnt == lines_count)
        gui->show_info("Export success", number_to_string<int>(lines_count)+" segments was exported to\n"+filename);
    else
        gui->show_info("Export invalid", number_to_string<int>(lines_count)+" != "+number_to_string<int>(fcnt));
    file.close();
}

bool Editor::save_to_file()
{
    create_idents();
    fstream file;
    file.open(current_filename, fstream::out);

    if(file.fail())
    {
        gui->show_info("Error", "Failed to open\n"+current_filename);
        return false;
    }

    unsaved_changes = false;
    update_title();

    file<<file_pr<<endl;
    file<<points.size()<<endl;

    for(unsigned int i = 0; i < points.size(); i++)
    {
        file<<points.at(i)->ident<<" "<<points.at(i)->get_position().x<<" "<<points.at(i)->get_position().y<<endl;
    }

    for(unsigned int i = 0; i < points.size(); i++)
    {
        if(points.at(i)->connected.size() > 0)
        {
            for(unsigned int c = 0; c < points.at(i)->connected.size(); c++)
            {
                file<<endl<<points.at(i)->ident<<"    "<<points.at(i)->connected.at(c).node_ptr1->ident<<" "<<points.at(i)->connected.at(c).node_ptr2->ident;
            }
        }
    }
    file.close();
    return true;
}

void Editor::load_from_file(string filename)
{
    fstream file;
    file.open(filename, fstream::in);
    string valid_file;

    if(file.fail())
    {
        gui->show_info("Error", "Failed to open\n"+filename);
        return;
    }

    file>>valid_file;
    if(valid_file != file_pr)
    {
        gui->show_info("Error", filename+"\nfile is invalid.");
        file.close();
        return;
    }

    delete_all();
    current_filename = filename;

    float x, y;
    int pts, p_id;
    file>>pts;

    for(int i = 0; i < pts; i++)
    {
        file>>p_id>>x>>y;
        shared_ptr<Point> ptr = make_shared<Point>(sf::Vector2f(x, y));
        ptr->ident = p_id;
        points.push_back(ptr);
    }

    while(true)
    {
        int id, p1, p2;
        file>>id>>p1>>p2;

        shared_ptr<Point> tmp_node = get_point_by_id(id);
        Point::node_ptr = tmp_node;
        shared_ptr<Point> tmp_points[2];
        tmp_points[0] = get_point_by_id(p1);
        tmp_points[1] = get_point_by_id(p2);

        if(tmp_points[0] == nullptr || tmp_points[1] == nullptr || tmp_node == nullptr)
        {
            //Point::node_ptr = nullptr;
        }
        else
        {
            tmp_points[0]->set_selected(true);
            tmp_points[1]->set_selected(true);

            if(tmp_node != nullptr)
                tmp_node->connect();

            tmp_points[0]->set_selected(false);
            tmp_points[1]->set_selected(false);
            Point::node_ptr = nullptr;
        }

        if(file.eof())
            break;
    }

    file.close();
    unsaved_changes = false;
    update_title();
}

void Editor::file_dialog_event()
{
    if(gui->filedialog.eventOccured())
    {
        if(current_operation == OPEN_FILE)
        {
            load_from_file(gui->filedialog.data.file_path);
        }
        else if(current_operation == CREATE_NEW_FILE)
        {
            gui->create_new_file_after_dialog();
        }
        else if(current_operation == SAVE_AS)
        {
            current_filename = gui->filedialog.data.file_path;
            save_to_file();
        }
        else if(current_operation == EXPORT)
        {
            save_segments_to_file(gui->filedialog.data.file_path);
        }
        else if(current_operation == SAVE_AND_EXIT)
        {
            current_filename = gui->filedialog.data.file_path;
            if(save_to_file())
            {
                App::get_app().get_sfml_window().close();
            }
        }
    }
}

void Editor::update_title()
{
    App::get_app().get_sfml_window().setTitle(App::get_app().get_app_title()+" - "+current_filename);
}

string Editor::get_current_filename()
{
    return current_filename;
}

bool Editor::check_if_unsaved_changes()
{
    return unsaved_changes;
}

void Editor::update()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Delete) || sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        delete_points();

    create_area();
    if(drawarea_released)
    {
        add_point_on_cursor();
        add_curve_on_cursor();

        select_all();
        press_all();

        select_point();
        move_curve();
    }

    gui->update(points.size(), segments, current_filename);
    file_dialog_event();
}

void Editor::unsaved_change()
{
    if(unsaved_changes == false)
    {
        unsaved_changes = true;
        App::get_app().get_sfml_window().setTitle(string(App::get_app().get_app_title()+" - ")+" *"+current_filename);
    }
}

void Editor::restore_workarea(const auto & h_item)
{
    points.clear();
    for(unsigned int i = 0; i < h_item.size(); i++)
    {
        points.push_back(get<0>(h_item.at(i)));
        points.back()->connected = get<1>(h_item.at(i));
        points.back()->set_position(get<2>(h_item.at(i)));
    }

    Point::selected_ptrs.clear();
    Point::pressed_ptrs.clear();
    for(unsigned int i = 0; i < points.size(); i++)
    {
        if(points.at(i)->selected)
            Point::selected_ptrs.push_back(points.at(i));
        if(points.at(i)->pressed)
            Point::pressed_ptrs.push_back(points.at(i));
    }
}

void Editor::create_snapshot()
{
    if(history_iter+1 != history.size() && history.size() > 0)
    {
        if(history_iter+1 < history.size())
            history.erase(history.begin()+history_iter+1, history.end());
    }

    vector<tuple<shared_ptr<Point>, vector<Link>, sf::Vector2f>>history_el;
    for(unsigned int i = 0; i < points.size(); i++)
    {
        history_el.push_back(make_tuple(points.at(i), points.at(i)->connected, points.at(i)->get_position()));
    }
    history.push_back(history_el);
    history_iter = history.size()-1;
}

void Editor::clear_changes_history()
{
    history.clear();
    create_snapshot();
}

void Editor::undo()
{
    history_iter--;

    if(history_iter < 0)
        history_iter = 0;
    else if(history_iter >= history.size())
        history_iter = history.size()-1;

    if(history.size() > 0)
    {
        restore_workarea(history.at(history_iter));
    }
}

void Editor::redo()
{
    history_iter++;

    if(history_iter < 0)
        history_iter = 0;
    else if(history_iter >= history.size())
        history_iter = history.size()-1;

    if(history.size() > 0)
    {
        restore_workarea(history.at(history_iter));
    }
}

void Editor::copy()
{
    if(Point::selected_ptrs.size() == 0)
        return;

    stash_points.clear();
    stash_connections.clear();
    create_idents();

    for(int i = 0; i < Point::selected_ptrs.size(); i++)
    {
        stash_points.push_back(make_tuple(Point::selected_ptrs.at(i)->ident, Point::selected_ptrs.at(i)->get_position()));
        for(unsigned int c = 0; c < Point::selected_ptrs.at(i)->connected.size(); c++)
        {
            stash_connections.push_back(make_tuple(Point::selected_ptrs.at(i)->ident, Point::selected_ptrs.at(i)->connected.at(c).node_ptr1->ident, Point::selected_ptrs.at(i)->connected.at(c).node_ptr2->ident));
        }
    }
}

void Editor::paste()
{
    if(stash_points.size() == 0)
        return;

    int left_i = 0, right_i = 0, top_i = 0, bot_i = 0;
    int left, right, top, bot, left_start, top_start;
    sf::Vector2f mid;

    for(int i = 0; i < stash_points.size(); i++)
    {
        if(get<1>(stash_points.at(i)).x < get<1>(stash_points.at(left_i)).x)
            left_i = i;
        if(get<1>(stash_points.at(i)).x > get<1>(stash_points.at(right_i)).x)
            right_i = i;
        if(get<1>(stash_points.at(i)).y < get<1>(stash_points.at(top_i)).y)
            top_i = i;
        if(get<1>(stash_points.at(i)).y > get<1>(stash_points.at(bot_i)).y)
            bot_i = i;
    }

    left = get<1>(stash_points.at(left_i)).x;
    right = get<1>(stash_points.at(right_i)).x;
    top = get<1>(stash_points.at(top_i)).y;
    bot = get<1>(stash_points.at(bot_i)).y;
    mid = sf::Vector2f((right-left)/2+left, (bot-top)/2+top);
    left_start = mid.x - (right-left)/2;
    top_start = mid.y - (bot-top)/2;

    for(int i = 0; i < stash_points.size(); i++)
    {
        points.push_back(make_shared<Point>(sf::Vector2f(get<1>(stash_points.at(i)).x - left_start + Inputs::getInputs().mousePos().x - (right-left)/2, get<1>(stash_points.at(i)).y - top_start+Inputs::getInputs().mousePos().y - (bot-top)/2)));
        points.back()->pasted = true;
        points.back()->ident = get<0>(stash_points.at(i));
    }

    for(int i = 0; i < stash_connections.size(); i++)
    {
        int id, p1, p2;
        id = get<0>(stash_connections.at(i));
        p1 = get<1>(stash_connections.at(i));
        p2 = get<2>(stash_connections.at(i));

        shared_ptr<Point> tmp_node = get_point_by_id(id, true);
        Point::node_ptr = tmp_node;
        shared_ptr<Point> tmp_points[2];
        tmp_points[0] = get_point_by_id(p1, true);
        tmp_points[1] = get_point_by_id(p2, true);

        if(tmp_points[0] != nullptr && tmp_points[1] != nullptr && tmp_node != nullptr)
        {
            tmp_node->connected.push_back(Link(tmp_points[0], tmp_points[1]));
            Point::node_ptr = nullptr;
        }
    }

    for(unsigned int i = 0; i < points.size(); i++)
        points.at(i)->pasted = false;

    create_snapshot();
    unsaved_change();
}

void Editor::draw()
{
    App::get_app().get_sfml_window().clear(Colors::get().getColor(Colors::BACKGROUND));
    draw_grid();
    draw_curve();
    draw_area();
    draw_points();
    draw_connections();
}
