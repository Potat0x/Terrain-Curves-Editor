#include "Point.hpp"

vector <shared_ptr<Point>>Point::pressed_ptrs;
vector <shared_ptr<Point>>Point::selected_ptrs;

shared_ptr<Point>Point::node_ptr;
sf::Vector2i Point::grid;

bool Point::after_group;
bool Point::use_grid;
bool Point::onmouse;

void Point::init()
{
    use_grid = true;
    after_group = false;
    grid = sf::Vector2i(40, 40);
}

Point::Point(const sf::Vector2f &pos)
{
    shape = sf::RectangleShape(sf::Vector2f(12, 12));
    shape.setFillColor(Colors::get().getColor(Colors::POINT));
    shape.setOrigin(shape.getLocalBounds().width/2, shape.getLocalBounds().height/2);
    shape.setPosition(pos);

    is_node = false;
    pressed = false;
    selected = false;
    lpm_pressed = false;
    lpm_released = true;
    ppm_pressed = false;
    ppm_released = true;
    mpm_released = true;

    Editor::get_Editor().unsaved_change();
}

Point::~Point()
{
    Editor::get_Editor().unsaved_change();
}

bool Point::select_press(bool can_press)
{
    if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && pressed_ptrs.size() == 1)
    {
        if(after_group == false)
            can_press = true;
        else
        {
            after_group = true;
            can_press = true;
        }
    }

    if(lpm_pressed == true && !Inputs::getInputs().get(sf::Mouse::Left))
    {
        lpm_released = true;
    }
    if(ppm_pressed == true && !Inputs::getInputs().get(sf::Mouse::Right))
    {
        ppm_released = true;
    }

    bool press = false;
    bool select = false;
    if(Inputs::getInputs().get(sf::Mouse::Left))
        press = true;
    else if(Inputs::getInputs().get(sf::Mouse::Right))
        select = true;
    else
    {
        lpm_pressed = false;
        ppm_pressed = false;
    }

    if(mouse_contain())
    {
        onmouse = true;
        if(is_node == false)
        {
            if(press && can_press)
            {
                if( (pressed_ptrs.size() == 1 && pressed_ptrs.at(0).get() == this) || pressed_ptrs.size() == 0 || (pressed_ptrs.size() > 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)))
                {
                    lpm_pressed = true;
                    setPressed(true);
                }
                else if(lpm_released && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    if(pressed)
                    {
                        lpm_pressed = true;
                        setPressed(false);
                    }
                    else
                    {
                        lpm_pressed = true;
                        setPressed(true);
                    }
                    lpm_released = false;

                }

                return false;
            }
            else if(select)
            {
                ppm_pressed = true;
                if(ppm_released)
                {
                    if(selected)
                    {
                        setSelected(false);
                    }
                    else
                    {
                        setSelected(true);
                    }
                    ppm_released = false;
                }
                return false;
            }
        }
        if(Inputs::getInputs().get(sf::Mouse::Middle))
        {
            if(pressed == false && selected == false && mpm_released)
            {
                if(is_node)
                {
                    is_node = false;
                    shape.setFillColor(Colors::get().getColor(Colors::POINT));
                    node_ptr = nullptr;
                }
                else
                {
                    is_node = true;
                    shape.setFillColor(Colors::get().getColor(Colors::NODE));
                    if(node_ptr.get() != nullptr && node_ptr.get() != this)
                    {
                        node_ptr->shape.setFillColor(Colors::get().getColor(Colors::POINT));
                        node_ptr->is_node = false;

                    }
                    node_ptr = shared_from_this();
                }
            }
            mpm_released = false;

            return is_node;
        }
        else
        {
            mpm_released = true;
        }

    }
    else
    {
        if(is_node == false)
        {
            if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            {
                if(select)
                {
                    setSelected(false);
                }
                else if(press)
                {
                    if(!lpm_pressed)
                    {
                        if(pressed_ptrs.size() > 1)
                            after_group = true;

                        setPressed(false);
                        lpm_pressed = false;
                    }
                }
            }
            return false;
        }
        else if(Inputs::getInputs().get(sf::Mouse::Middle))
        {
            node_ptr->shape.setFillColor(Colors::get().getColor(Colors::POINT));
            node_ptr->is_node = false;
            node_ptr = nullptr;
        }
    }
    return false;
}

bool Point::mouse_contain()
{
    sf::Vector2f mouse_pos(Inputs::getInputs().mousePos());
    sf::Rect<float>rect(shape.getPosition().x-shape.getLocalBounds().width/2, shape.getPosition().y-shape.getLocalBounds().height/2, shape.getLocalBounds().width, shape.getLocalBounds().height);

    if(rect.contains(mouse_pos.x, mouse_pos.y))
        return true;
    else return false;
}

bool Point::check_connection()
{
    for(unsigned int i = 0; i < node_ptr->connected.size(); i++)
    {
        if((node_ptr->connected.at(i).node_ptr1 == selected_ptrs.at(0) && node_ptr->connected.at(i).node_ptr2 == selected_ptrs.at(1))  ||
                (node_ptr->connected.at(i).node_ptr1 == selected_ptrs.at(1) && node_ptr->connected.at(i).node_ptr2 == selected_ptrs.at(0))
          )
        {
            return true;
        }
    }

    return false;
}

void Point::connect_this(const shared_ptr<Point>& ptr1, const shared_ptr<Point>& ptr2)
{
    this->connected.push_back(Link(ptr1, ptr2));
}

bool Point::connect()
{
    if(selected_ptrs.size() != 2)
        return false;

    if(node_ptr != nullptr && selected_ptrs.size() == 2)
    {
        if(check_connection() == false)
        {
            Editor::get_Editor().unsaved_change();
            node_ptr->connected.push_back(Link(selected_ptrs.at(0), selected_ptrs.at(1)));
            cout<<"connect() true";
            return true;
        }
    }

    return false;
}

void Point::erase_pressed(const shared_ptr<Point>& ptr)
{
    for(unsigned int i = 0; i < pressed_ptrs.size(); i++)
    {
        if(pressed_ptrs.at(i) == ptr)
        {
            pressed_ptrs.erase(pressed_ptrs.begin()+i);
            return;
        }
    }
}

void Point::erase_selected(const shared_ptr<Point>& ptr)
{
    if(selected_ptrs.size() > 0)
    {
        for(unsigned int i = 0; i < selected_ptrs.size(); i++)
        {
            if(selected_ptrs.at(i) == ptr)
            {
                selected_ptrs.erase(selected_ptrs.begin()+i);
                return;
            }
        }
    }
}

void Point::setPressed(bool p)
{
    if(p)
    {
        if(pressed == false)
        {
            pressed_ptrs.push_back(shared_from_this());
            //pressed_count++;
        }

        pressed = true;
        shape.setFillColor(Colors::get().getColor(Colors::POINT_PRESSED));
    }
    else
    {
        if(pressed == true)
        {
            erase_pressed(shared_from_this());
            //pressed_count--;
        }

        pressed = false;
        shape.setFillColor(Colors::get().getColor(Colors::POINT));
        if(selected)
            shape.setFillColor(Colors::get().getColor(Colors::POINT_SELECTED));
    }
}

void Point::setSelected(bool s)
{
    if(s)
    {
        if(selected == false)
        {
            selected_ptrs.push_back(shared_from_this());
        }
        selected = true;
        shape.setFillColor(Colors::get().getColor(Colors::POINT_SELECTED));
    }
    else
    {
        if(selected == true)
        {
            erase_selected(shared_from_this());
        }

        selected = false;
        shape.setFillColor(Colors::get().getColor(Colors::POINT));
        if(pressed)
            shape.setFillColor(Colors::get().getColor(Colors::POINT_PRESSED));
    }
}

sf::Vector2f Point::getPosition()
{
    return shape.getPosition();
}

void Point::setPosition(const sf::Vector2f & pos)
{
    shape.setPosition(pos);
}

void Point::draw(sf::RenderWindow * win)
{
    win->draw(shape);
}

void Point::set_on_grid_position()
{
    sf::Vector2f mouse = Inputs::getInputs().mousePos();

    int mx, my, gx, gy;
    mx = mouse.x+grid.x/2;
    gx = grid.x;
    my = mouse.y+grid.y/2;
    gy = grid.y;

    sf::Vector2i pos;
    pos.x = (mx/gx)*gx;
    pos.y = (my/gy)*gy;

    shape.setPosition(sf::Vector2f(pos));
}

void Point::move()
{
    if(pressed)
    {
        Editor::get_Editor().unsaved_change();

        if(use_grid == true)
            set_on_grid_position();
        else
        {
            shape.setPosition(Inputs::getInputs().mousePos());
        }

    }
}

void Point::calculate_diff()
{
    if(pressed_ptrs.size() > 0)
    {
        sf::Vector2f mouse_pos = Inputs::getInputs().mousePos();

        for(unsigned int i = 0; i < pressed_ptrs.size(); i++)
        {
            pressed_ptrs.at(i)->diff.x = mouse_pos.x - pressed_ptrs.at(i)->getPosition().x;
            pressed_ptrs.at(i)->diff.y = mouse_pos.y - pressed_ptrs.at(i)->getPosition().y;
        }
    }
}

void Point::move_group()
{
    sf::Vector2f mouse_pos = Inputs::getInputs().mousePos();
    Editor::get_Editor().unsaved_change();
    for(unsigned int i = 0; i < pressed_ptrs.size(); i++)
    {
        pressed_ptrs.at(i)->shape.setPosition(sf::Vector2f(mouse_pos.x-pressed_ptrs.at(i)->diff.x, mouse_pos.y-pressed_ptrs.at(i)->diff.y));
    }
}
