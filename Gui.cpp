#include "Gui.hpp"

class App;

int is_regular_file(const char *path);

void Gui::set_label()
{
    if(!filedialog.data.directory.empty())
    new_file_label->SetText(filedialog.data.directory+"\\"+new_file_entry->GetText());
    else
    new_file_label->SetText(new_file_entry->GetText());
}

void Gui::create_new_file_after_dialog()
{
    new_file_label->SetText(filedialog.data.directory);
    if(!filedialog.data.file_name.empty())
    new_file_entry->SetText(filedialog.data.file_name);
    set_label();
}

Gui::Gui(sf::RenderWindow & sfml_window)
{
    //desktop.GetEngine().LoadThemeFromFile("theme.css");
    filedialog.setRenderWindow(sfml_window);
    last_nonzero_step = 0.5f;
    //
    int buton_height = 26;
    main_menu_box = Box::Create(Box::Orientation::HORIZONTAL);
    main_menu_box->SetRequisition(sf::Vector2f(150, buton_height));
    main_menu_box->SetAllocation(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(150, buton_height)));

    file = Button::Create("File");
    main_menu_box->Pack(file);
    file->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::show_filemenu, this, true));

    edit = Button::Create("Edit");
    main_menu_box->Pack(edit);
    edit->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::show_editmenu, this, true));

    view = Button::Create("View");
    main_menu_box->Pack(view);
    view->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::show_viewmenu, this, true));

    settings = Button::Create("Settings");
    main_menu_box->Pack(settings);
    settings->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::show_settingsmenu, this, true));
    settings->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::restore_settings_from_tmp, this));

    help = Button::Create("Help");
    main_menu_box->Pack(help);
    help->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::show_help_dialog, this, true));

    about = Button::Create("About");
    about->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::show_about_dialog, this, true));
    main_menu_box->Pack(about);

    desktop.Add(main_menu_box);
    //
    file_box = Box::Create(Box::Orientation::VERTICAL);
    file_box->SetPosition(sf::Vector2f(file.get()->GetAllocation().left, file.get()->GetAllocation().height));

    create = Button::Create("New");
    create->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::show_new_file_window, this, true));
    file_box->Pack(create);
    open = Button::Create("Open");
    open->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::open_file, this));
    file_box->Pack(open);
    save = Button::Create("Save");
    save->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::save_file, this));
    file_box->Pack(save);
    save_as = Button::Create("Save as");
    save_as->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::save_file_as, this));
    file_box->Pack(save_as);
    generate = Button::Create("Export");
    generate->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::export_to_file, this));
    file_box->Pack(generate);
    //
    edit_box = Box::Create(Box::Orientation::VERTICAL);
    edit_box->SetPosition(sf::Vector2f(edit.get()->GetAllocation().left, edit.get()->GetAllocation().height));

    undo = Button::Create("Undo");
    undo->GetSignal(Button::OnLeftClick).Connect(bind(&Editor::undo, &Editor::get_Editor()));
    edit_box->Pack(undo);
    redo = Button::Create("Redo");
    redo->GetSignal(Button::OnLeftClick).Connect(bind(&Editor::redo, &Editor::get_Editor()));
    edit_box->Pack(redo);
    clear_history = Button::Create("Clear history");
    clear_history->GetSignal(Button::OnLeftClick).Connect(bind(&Editor::clear_changes_history, &Editor::get_Editor()));
    edit_box->Pack(clear_history);
    //
    view_box = Box::Create(Box::Orientation::VERTICAL);
    view_box->SetPosition(sf::Vector2f(view.get()->GetAllocation().left, view.get()->GetAllocation().height));

    link = CheckButton::Create("Link");
    view_box->Pack(link);

    curve = CheckButton::Create("Curve dots");
    view_box->Pack(curve);

    points = CheckButton::Create("Points");
    view_box->Pack(points);

    segments = CheckButton::Create("Segments");
    view_box->Pack(segments);

    grid = CheckButton::Create("Grid");
    view_box->Pack(grid);

    auto update_view_b = bind(&Gui::update_view, this);
    link->GetSignal(Button::OnLeftClick).Connect(update_view_b);
    curve->GetSignal(Button::OnLeftClick).Connect(update_view_b);
    points->GetSignal(Button::OnLeftClick).Connect(update_view_b);
    segments->GetSignal(Button::OnLeftClick).Connect(update_view_b);
    grid->GetSignal(Button::OnLeftClick).Connect(update_view_b);

    view_window = Window::Create();
    view_window->SetTitle("View");
    sf::Vector2f view_window_size(180, 135);
    view_window->SetRequisition(view_window_size);
    view_window->SetAllocation(sf::FloatRect(sf::Vector2f(view.get()->GetAllocation().left, view.get()->GetAllocation().height), view_window_size));

    view_window->SetStyle(Window::Style::BACKGROUND);
    view_window->Add(view_box);
    //
    settings_window = Window::Create();
    settings_window->SetTitle("Settings");
    sf::Vector2f settings_window_size(180, 81);
    settings_window->SetRequisition(settings_window_size);
    settings_window->SetAllocation(sf::FloatRect(sf::Vector2f(settings.get()->GetAllocation().left, settings.get()->GetAllocation().height), settings_window_size));
    settings_window->SetStyle(Window::Style::BACKGROUND);

    settings_box = Box::Create(Box::Orientation::VERTICAL);
    settings_box.get()->SetSpacing(3);

    for(unsigned int i = 0; i < 5; i++)
    {
        settings_lines[i] = Box::Create(Box::Orientation::HORIZONTAL);
        if(i == 1) continue;//curve step
        settings_box->Pack(settings_lines[i],false, false);
    }

    sf::Vector2f s_req(90, 20);

    use_grid = CheckButton::Create("Use grid");
    use_grid->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::update_settings, this));
    grid_value = Entry::Create();
    grid_value->GetSignal(Entry::OnText).Connect(bind(&Gui::update_settings, this));
    use_grid.get()->SetRequisition(s_req);
    grid_value.get()->SetRequisition(s_req);
    settings_lines[0]->Pack(use_grid);
    settings_lines[0]->Pack(grid_value);

    curve_step = Label::Create("Curve step");
    segment_step = Label::Create("Segment step");
    curve_step.get()->SetRequisition(s_req);
    segment_step.get()->SetRequisition(s_req);

    curve_step_entry = Entry::Create("b");
    curve_step_entry->GetSignal(Entry::OnText).Connect(bind(&Gui::update_settings, this));
    segment_step_entry = Entry::Create("c");
    segment_step_entry->GetSignal(Entry::OnText).Connect(bind(&Gui::update_settings, this));
    curve_step_entry.get()->SetRequisition(s_req);
    segment_step_entry.get()->SetRequisition(s_req);

    settings_lines[1]->Pack(curve_step);
    settings_lines[1]->Pack(curve_step_entry);

    settings_lines[2]->Pack(segment_step);
    settings_lines[2]->Pack(segment_step_entry);

    settings_lines[2]->Pack(segment_step);
    settings_lines[2]->Pack(segment_step_entry);

    s_sep = Separator::Create(Separator::Orientation::HORIZONTAL);
    s_sep.get()->SetRequisition(sf::Vector2f(curve_step_entry.get()->GetAllocation().width*2, 4));
    settings_lines[3]->Pack(s_sep);

    s_apply = Button::Create("Apply");
    s_apply.get()->SetRequisition(s_req);
    s_apply->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::apply_settings, this));
    s_close = Button::Create("Cancel");
    s_close.get()->SetRequisition(s_req);
    s_close->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::show_settingsmenu, this, true));
    s_close->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::restore_settings_from_tmp, this));

    settings_lines[4]->Pack(s_apply);
    settings_lines[4]->Pack(s_close);

    settings_window->Add(settings_box);

    std::vector<Widget::Ptr> settings_widgets = settings_box.get()->GetChildren();

    for(unsigned int i = 0; i < settings_widgets.size(); i++)
    {
        settings_widgets.at(i).get()->SetRequisition(sf::Vector2f(110, 20));
    }
    //info
    sf::Vector2f info_window_size(200, 100);
    info_window = Window::Create(Window::Style::TOPLEVEL);
    info_window->SetTitle("info_window");
    info_box = Box::Create(Box::Orientation::VERTICAL);
    info_box->SetSpacing(10);
    info_window->Add(info_box);
    info_window->SetAllocation(sf::FloatRect(0, 0, info_window_size.x, info_window_size.y));

    info_label = Label::Create("error message");
    info_box->Pack(info_label);
    info_button = Button::Create("ok");
    info_box->Pack(info_button);
    info_window->Show(false);
    info_button->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::hide_info, this));
    desktop.Add(info_window);
    //status bar
    sf::Vector2f status_window_size(300, 10);
    status_window = Window::Create(Window::Style::NO_STYLE );
    status_window->SetTitle("status_window");
    status_box = Box::Create(Box::Orientation::HORIZONTAL);
    status_box->SetSpacing(10);
    status_window->Add(status_box);

    //unsaved changes window
    sf::Vector2f uch_window_size(200, 100);
    uch_window = Window::Create(Window::Style::TOPLEVEL);
    uch_window->SetTitle("Unsaved changes");
    uch_window->Show(false);
    uch_box = Box::Create(Box::Orientation::VERTICAL);
    uch_box2 = Box::Create(Box::Orientation::HORIZONTAL);
    uch_box->SetSpacing(10);
    uch_window->Add(uch_box);

    uch_label = Label::Create("?");
    uch_box->Pack(uch_label);
    uch_box->Pack(uch_box2);

    uch_buttons[0] = Button::Create("yes");
    uch_buttons[1] = Button::Create("no");
    uch_buttons[2] = Button::Create("cancel");

    for(int i = 0; i < 3; i++)
    {
        uch_buttons[i]->SetRequisition(sf::Vector2f(80, 0));
        uch_box2->Pack(uch_buttons[i]);
    }

    uch_buttons[0]->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::save_and_exit_app, this));
    uch_buttons[1]->GetSignal(Button::OnLeftClick).Connect(bind(&sf::RenderWindow::close, &sfml_window));
    uch_buttons[2]->GetSignal(Button::OnLeftClick).Connect(bind(&Window::Show, uch_window, false));

    desktop.Add(uch_window);

    for(short int i = 0; i < 4; i++)
    {
        status_label[i] = Label::Create();

        if(i != 3)
        status_box->Pack(status_label[i]);

        status_label[i]->SetRequisition(sf::Vector2f(100, 1));
    }

    fps_label = Label::Create("FPS 999");
    status_label[0]->SetText("filename.txt");
    status_label[1]->SetText("segments: 3563");
    status_label[2]->SetText("points: 3499");
    status_label[3]->SetText("x = 94 y = 435");
    status_label[3]->SetRequisition(sf::Vector2f(80, 25));
    status_window->Show(true);
    status_window->SetAllocation(sf::FloatRect(sfml_window.getSize().x-status_window->GetRequisition().x, 0, status_window->GetRequisition().x, status_window->GetRequisition().y));
    status_label[3]->SetAllocation(sf::FloatRect(sfml_window.getSize().x-status_label[3]->GetAllocation().width, sfml_window.getSize().y-status_label[3]->GetAllocation().height, status_label[3]->GetAllocation().width, status_label[3]->GetAllocation().height));
    fps_label->SetRequisition(sf::Vector2f(80, 25));
    fps_label->SetAllocation(sf::FloatRect(0, sfml_window.getSize().y-fps_label->GetAllocation().height, fps_label->GetAllocation().width, fps_label->GetAllocation().height));
    //
    Context::Get().GetEngine().SetProperty("Window", "HandleSize", 0.0f);
    about_window = Window::Create();
    about_window->SetTitle("About");
    sf::Vector2f about_window_size(240, 200);
    about_window->SetRequisition(about_window_size);
    about_window->SetAllocation(sf::FloatRect(sf::Vector2f(sfml_window.getSize().x/2-about_window_size.x/2, sfml_window.getSize().y/2-about_window_size.y/2), settings_window_size));
    about_box = Box::Create(Box::Orientation::VERTICAL);
    about_window->Add(about_box);
    about_label_t = Label::Create("Terrain Curves Editor");
    about_label = Label::Create("Terrain editor for 2D games based on Bezier curves.\nVersion:    1.0\ngithub.com/Potat0x/Terrain-Curves-Editor\n\nCreated with SFML and SFGUI\npotat0x.cba.pl");
    about_box->Pack(about_label_t);
    about_box->Pack(about_label);
    //
    help_window = Window::Create();
    help_window->SetTitle("Help");
    help_box = Box::Create(Box::Orientation::VERTICAL);
    help_window->Add(help_box);
    sf::Vector2f help_window_size(200, 400);
    help_window->SetRequisition(help_window_size);
    help_window->SetAllocation(sf::FloatRect(sf::Vector2f(sfml_window.getSize().x/2-help_window_size.x/2, sfml_window.getSize().y/2-help_window_size.y/2), help_window_size));
    help_label = Label::Create("help");
    //
    testb = Button::Create("close");
    help_box->Pack(help_label);
    help_box->Pack(testb);
    testb->GetSignal(Button::OnLeftClick).Connect(bind(&Window::Show, help_window, false));
    // File menu
    sf::Vector2f nf_req(90, 10);
    new_file_window = Window::Create();
    new_file_window->SetTitle("Create new file");
    sf::Vector2f file_window_size(260, 130);

    new_file_window->SetRequisition(file_window_size);
    new_file_window->SetAllocation(sf::FloatRect(sf::Vector2f(sfml_window.getSize().x/2-file_window_size.x/2, sfml_window.getSize().y/2-file_window_size.y/2), file_window_size));

    new_file_box = Box::Create(Box::Orientation::VERTICAL);
    new_file_box.get()->SetSpacing(6);

    for(unsigned int i = 0; i < 4; i++)
    {
        new_file_box_lines[i] = Box::Create(Box::Orientation::HORIZONTAL);
        new_file_box->Pack(new_file_box_lines[i], false, false);
    }

    nf_apply = Button::Create("Create");
    nf_apply.get()->SetRequisition(nf_req);
    nf_apply->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::create_new_file_apply, this));

    nf_close = Button::Create("Close");
    nf_close.get()->SetRequisition(nf_req);
    nf_close->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::show_new_file_window, this, false));

    nf_browse = Button::Create("Browse");
    nf_browse.get()->SetRequisition(nf_req);
    nf_browse->GetSignal(Button::OnLeftClick).Connect(bind(&Gui::create_new_file, this));

    new_file_label = Label::Create(".");
    new_file_label.get()->SetRequisition(nf_req);
    new_file_box_lines[0]->Pack(new_file_label);

    new_file_entry = Entry::Create("file_name.txt");
    new_file_entry.get()->SetRequisition(nf_req);
    new_file_box_lines[1]->Pack(new_file_entry);
    new_file_entry->GetSignal(Entry::OnText).Connect(bind(&Gui::set_label, this));

    nf_sep = Separator::Create(Separator::Orientation::HORIZONTAL);
    nf_sep.get()->SetRequisition(nf_req);
    s_sep.get()->SetRequisition(sf::Vector2f(new_file_entry.get()->GetAllocation().width*2, 4));

    new_file_box_lines[2]->Pack(nf_sep);

    new_file_box_lines[3]->Pack(nf_apply);
    new_file_box_lines[3]->Pack(nf_close);
    new_file_box_lines[3]->Pack(nf_browse);

    new_file_window->Add(new_file_box);
    //
    fstream help_file;
    help_file.open("help.txt", ios::in);
    if(help_file.fail())
    {
        help_label->SetText("Failed to load help file\nhelp.txt");
    }
    else
    {
        string hs, tmp;
        while(getline(help_file, tmp))
        {
            hs += '\n'+tmp;
        }

        help_file.close();
        help_label->SetText(hs+"\n\n");
    }

    //
    desktop.Add(status_label[3]);
    desktop.Add(info_window);
    desktop.Add(fps_label);
    desktop.Add(status_window);
    desktop.Add(file_box);
    desktop.Add(edit_box);
    desktop.Add(view_window);
    desktop.Add(about_window);
    desktop.Add(settings_window);
    desktop.Add(help_window);
    desktop.Add(new_file_window);
    file_box.get()->Show(false);
    edit_box.get()->Show(false);
    view_window.get()->Show(false);
    settings_window.get()->Show(false);
    help_window.get()->Show(false);
    about_window.get()->Show(false);
    new_file_window.get()->Show(false);
    filedialog.setDesktop(desktop);

    load_b_view();
    update_clock.restart();
}

void Gui::export_to_file()
{
    file_box.get()->Show(false);
    filedialog.action(FileDialog::Action::SELECT_FOLDER_AND_TYPE_FILENAME);
    Editor::get_Editor().current_operation = Editor::GuiOperations::EXPORT;
}

void Gui::show_info(const string & title, const string & message)
{
    desktop.BringToFront(info_window);
    info_window->SetTitle(title);
    info_label->SetText(message);
    info_window->SetPosition(sf::Vector2f(App::getApp().get_sfml_window().getSize().x/2 - info_window->GetAllocation().width/2, App::getApp().get_sfml_window().getSize().y/2 - info_window->GetAllocation().height/2));
    info_window->Show(true);
}

void Gui::show_unsaved_ch_window(const string & filename)
{
    desktop.BringToFront(uch_window);
    uch_label->SetText("File "+filename+"\nis modified. Do you want to save changes?");
    uch_window->SetPosition(sf::Vector2f(App::getApp().get_sfml_window().getSize().x/2 - uch_window->GetAllocation().width/2, App::getApp().get_sfml_window().getSize().y/2 - uch_window->GetAllocation().height/2));
    uch_window->Show(true);
}

void Gui::window_resize()
{
    status_window->SetAllocation(sf::FloatRect(App::getApp().get_sfml_window().getSize().x-status_window->GetAllocation().width, 0, status_window->GetAllocation().width, status_window->GetAllocation().height));
    status_label[3]->SetAllocation(sf::FloatRect(App::getApp().get_sfml_window().getSize().x-status_label[3]->GetAllocation().width, App::getApp().get_sfml_window().getSize().y-status_label[3]->GetAllocation().height, status_label[3]->GetAllocation().width, status_label[3]->GetAllocation().height));
    fps_label->SetAllocation(sf::FloatRect(0, App::getApp().get_sfml_window().getSize().y-fps_label->GetAllocation().height, fps_label->GetAllocation().width, fps_label->GetAllocation().height));
}

void Gui::hide_info()
{
    info_window->Show(false);
}

void Gui::save_and_exit_app()
{
    uch_window->Show(false);
    if(Editor::get_Editor().current_filename.empty() == false)
    {
        if(Editor::get_Editor().save_to_file())
        {
            App::getApp().close(true);
        }
    }
    else
    {
        save_file_as();
        Editor::get_Editor().current_operation = Editor::GuiOperations::SAVE_AND_EXIT;
    }
}

void Gui::save_file()
{
    if(Editor::get_Editor().current_filename.empty() == false)
    {
        Editor::get_Editor().save_to_file();
        file_box.get()->Show(false);
    }
    else save_file_as();
}

void Gui::save_file_as()
{
    file_box.get()->Show(false);
    filedialog.action(FileDialog::Action::SELECT_FOLDER_AND_TYPE_FILENAME);
    Editor::get_Editor().current_operation = Editor::GuiOperations::SAVE_AS;
}

void Gui::open_file()
{
    file_box.get()->Show(false);
    filedialog.action(FileDialog::Action::SELECT_FILE);
    Editor::get_Editor().current_operation = Editor::GuiOperations::OPEN_FILE;
}

void Gui::load_b_view()
{
    if(Editor::get_Editor().draw_con)
    link.get()->SetActive(true);
    else link.get()->SetActive(false);

    if(Editor::get_Editor().draw_cur)
    curve.get()->SetActive(true);
    else curve.get()->SetActive(false);

    if(Editor::get_Editor().draw_pts)
    points.get()->SetActive(true);
    else points.get()->SetActive(false);

    if(Editor::get_Editor().draw_lines)
    segments.get()->SetActive(true);
    else segments.get()->SetActive(false);

    if(Editor::get_Editor().draw_gr)
    grid.get()->SetActive(true);
    else grid.get()->SetActive(false);

    grid_value.get()->SetText("grid_value");
}

void Gui::switch_grid_usage()
{
    load_tmp_settings();
    use_grid.get()->SetActive(!use_grid.get()->IsActive());
    Editor::get_Editor().draw_gr = use_grid.get()->IsActive();
    update_settings();
}

void Gui::update_view()
{
    if(link.get()->IsActive())
    Editor::get_Editor().draw_con = true;
    else Editor::get_Editor().draw_con = false;

    if(curve.get()->IsActive())
    Editor::get_Editor().draw_cur = true;
    else Editor::get_Editor().draw_cur = false;

    if(points.get()->IsActive())
    Editor::get_Editor().draw_pts = true;
    else Editor::get_Editor().draw_pts = false;

    if(segments.get()->IsActive())
    Editor::get_Editor().draw_lines = true;
    else Editor::get_Editor().draw_lines = false;

    if(grid.get()->IsActive())
    Editor::get_Editor().draw_gr = true;
    else Editor::get_Editor().draw_gr = false;
}

void Gui::apply_settings()
{
    update_settings();
    show_settingsmenu(false);
}

void Gui::restore_settings_from_tmp()
{
    if(!settings_window.get()->IsGloballyVisible())
    {
        Point::use_grid = tmp_use_grid;
        Point::grid.x = tmp_grid;
        Point::grid.y = tmp_grid;
        Editor::draw_step = tmp_segment_step;
    }
}

void Gui::load_tmp_settings()
{
    tmp_use_grid = Point::use_grid;
    tmp_grid = Point::grid.x;
    tmp_segment_step = Editor::draw_step;

    use_grid.get()->SetActive(Point::use_grid);
    grid_value->SetText(number_to_string<int>(Point::Point::grid.x));
    segment_step_entry->SetText(number_to_string<float>(Editor::draw_step));
}

void Gui::update_settings()
{
    if(use_grid.get()->IsActive())
    Point::use_grid = true;
    else Point::use_grid = false;

    int grid = string_to_number<int>(grid_value.get()->GetText());
    if(grid < 1)
        grid = 1;
    Point::grid.x = grid;
    Point::grid.y = grid;

    Editor::draw_step = string_to_number<float>(segment_step_entry->GetText());
    if(Editor::draw_step <= 0)
    {
        Editor::draw_step = last_nonzero_step;
    }
    else if(Editor::draw_step > 1)
    {
        Editor::draw_step = 1.0f;
        segment_step_entry->SetText(number_to_string<float>(Editor::draw_step));
    }

    last_nonzero_step = Editor::draw_step;
}

void Gui::show_new_file_window(bool s)
{
    new_file_window.get()->Show(s);
    if(s)
    show_filemenu(false);
}

void Gui::create_new_file_apply()
{
    string fn;
    if(!filedialog.data.directory.empty())
    fn = filedialog.data.directory+"\\"+new_file_entry.get()->GetText();
    else
    fn = new_file_entry.get()->GetText();

    if(is_regular_file(fn.c_str()) == 1)
    {
        //showOverwriteWindow(path+"\\"+filename_entry->GetText());
        show_info("Error", fn+"\nalready exist.");
    }
    else if(::create_new_file(fn))
    {
        cout<<"create file ok"<<fn<<endl;
        show_new_file_window(false);
        Editor::get_Editor().current_filename = fn;
        Editor::get_Editor().update_title();
    }
    else show_info("Error", fn+"\nCreate file error.");
}

void Gui::create_new_file()
{
    filedialog.action(FileDialog::Action::SELECT_FOLDER);
    Editor::get_Editor().current_operation = Editor::GuiOperations::CREATE_NEW_FILE;
}

void Gui::show_help_dialog(bool s)
{
    if(!help_window.get()->IsGloballyVisible())
    {
        help_window.get()->Show(s);
    } else help_window.get()->Show(false);

    if(s)
    {
        show_editmenu(false);
        show_viewmenu(false);
        show_filemenu(false);
        show_settingsmenu(false);
        show_about_dialog(false);
    }
}

void Gui::show_about_dialog(bool s)
{
    if(!about_window.get()->IsGloballyVisible())
    {
        about_window.get()->Show(s);
    } else about_window.get()->Show(false);

    if(s)
    {
        show_viewmenu(false);
        show_editmenu(false);
        show_filemenu(false);
        show_settingsmenu(false);
        show_help_dialog(false);
    }
}

void Gui::show_settingsmenu(bool s)
{
    if(!settings_window.get()->IsGloballyVisible())
    {
        settings_window.get()->Show(s);
        load_tmp_settings();
    } else
    {
        settings_window.get()->Show(false);
    }

    if(s)
    {
        show_editmenu(false);
        show_viewmenu(false);
        show_filemenu(false);
        show_about_dialog(false);
        show_help_dialog(false);
    }
}

void Gui::show_viewmenu(bool s)
{
    if(!view_window.get()->IsGloballyVisible())
    {
        view_window.get()->Show(s);
    } else view_window.get()->Show(false);

    if(s)
    {
        show_filemenu(false);
        show_editmenu(false);
        show_settingsmenu(false);
        show_about_dialog(false);
        show_help_dialog(false);
    }
}

void Gui::show_filemenu(bool s)
{
    if(!file_box.get()->IsGloballyVisible())
    {
        file_box.get()->Show(s);
    } else file_box.get()->Show(false);

    if(s)
    {
        show_editmenu(false);
        show_viewmenu(false);
        show_settingsmenu(false);
        show_about_dialog(false);
        show_help_dialog(false);
    }
}

void Gui::show_editmenu(bool s)
{
    if(!edit_box.get()->IsGloballyVisible())
    {
        edit_box.get()->Show(s);
    } else edit_box.get()->Show(false);

    if(s)
    {
        show_filemenu(false);
        show_viewmenu(false);
        show_settingsmenu(false);
        show_about_dialog(false);
        show_help_dialog(false);
    }
}

void Gui::handle_event(sf::Event & event)
{
    desktop.HandleEvent(event);
}

void Gui::update(const unsigned int & points, const unsigned int & segments, const string & filename)
{
    filedialog.update();
    desktop.Update(update_clock.restart().asSeconds());

    sf::Vector2f mouse_pos_area = App::getApp().get_sfml_window().mapPixelToCoords(static_cast<sf::Vector2i>(Inputs::getInputs().mousePosOnWindow()));
    status_label[0]->SetText(filename);
    status_label[1]->SetText("points: "+number_to_string<int>(points));
    status_label[2]->SetText("segments: "+number_to_string<int>(segments));
    status_label[3]->SetText(number_to_string<int>(round(mouse_pos_area.x))+", "+number_to_string<int>(round(mouse_pos_area.y))+" px");
}

void Gui::update_fps(const int & fps)
{
    fps_label->SetText("   FPS "+number_to_string<int>(fps));
}

void Gui::draw()
{
    sfgui.Display(App::getApp().get_sfml_window());
}
