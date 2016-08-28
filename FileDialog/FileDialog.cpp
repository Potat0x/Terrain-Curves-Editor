#include "FileDialog.hpp"

sf::Image FileDialog::ExplorerItem::folder_icon;
sf::Image FileDialog::ExplorerItem::file_icon;
namespace
{
    bool checkDirectory(string dir);
    int dirExists(string path);
    string go_parent_path(string ep);
    int is_regular_file(const char *path);
}

void FileDialog::fileClicked(const string & filename)
{
    filename_entry->SetText(filename);
    filename_entry->SetCursorPosition(filename.size());
    restorePath();
    path_entry->SetState(sfg::Entry::State::NORMAL);
}

void FileDialog::update()
{
   if(refresh_list)
    {
        filename_entry->SetText("");
        path_entry->SetText(path);
        path_entry->SetCursorPosition(path.size());
        explorer_box->RemoveAll();

        ExplorerItem * tmp_ei;
        for(unsigned int i = 0; i < folders.size(); i++)
        {
            tmp_ei = new ExplorerItem(0, folders.at(i));
            items.push_back(tmp_ei);
            if(folders.at(i) != "." && folders.at(i) != "..")
            tmp_ei->connection_serial = tmp_ei->get()->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::listFiles, this, path+"\\"+folders.at(i)));
            else if(folders.at(i) == ".")
            tmp_ei->connection_serial = tmp_ei->get()->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::listFiles, this, path));
            else if(folders.at(i) == "..")
            tmp_ei->connection_serial = tmp_ei->get()->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::listFiles, this, go_parent_path(path)));
            tmp_ei = nullptr;
        }

        for(unsigned int i = 0; i < files.size(); i++)
        {
            tmp_ei = new ExplorerItem(1, files.at(i));
            items.push_back(tmp_ei);
            tmp_ei->connection_serial = tmp_ei->get()->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::fileClicked, this, files.at(i)));
            tmp_ei = nullptr;
        }

        //cout<<"Pack start"<<endl;
        //sf::Clock clock;
        //clock.restart();
        for(unsigned int i = 0; i < items.size(); i++)
        {
            if(items[i]->to_delete == false)
            explorer_box->Pack(items[i]->get());
        }
        //cout<<"Pack end: "<<clock.getElapsedTime().asSeconds()<<endl;
        //int iters = 0;
        unsigned int i = 0;
        unsigned int max = 0;

        while(true)
        {//iters++;
            if(items.at(i)->to_delete)
            {
                delete items.at(i);
                items.erase(items.begin()+i);
                //deleted++;
            }
            else max++;

            i = max;
            if(i == items.size())
                break;
        }
        refresh_list = false;
    }
}

void FileDialog::listFiles(string path_name)
{
    path = path_name;

    folders.clear();
    files.clear();

    for(unsigned int i = 0; i < items.size(); i++)
    {
        items.at(i)->to_delete = true;
    }

    struct dirent * file;
    DIR * path;
    //cout<<path_name<<endl;
    if((path = opendir(path_name.c_str())))
    {
        while((file = readdir(path)))
        {
            string fullpath(path_name);
            fullpath.append("\\");
            fullpath.append(file->d_name);

            if(is_regular_file(fullpath.c_str()) == 1 && a_type != SELECT_FOLDER)
            files.push_back(file->d_name);
            else if(is_regular_file(fullpath.c_str()) == 0)
            folders.push_back(file->d_name);
            //else cout<<"is_regular_file != 1, 0"<<endl;
        }

        closedir( path );
        refresh_list = true;
    }
    else
    {
        showError("listFiles opendir error\n"+path_name);
        return;
    }
}

FileDialog::FileDialog()
{
    path = ".";
    event = false;
    //cout<<getcwd(NULL,0)<<endl;
    refresh_list = false;
    sfml_window = nullptr;
    desktop_ = nullptr;
    ExplorerItem::loadImages();
    //sf::Clock clock;
    //clock.restart();

    //sf::Vector2f window_size(320, 700);
    dialog_window = Window::Create();
    dialog_window->SetTitle("FileDialog");
    //dialog_window->SetRequisition(window_size);

    dialog_main_box = Box::Create(Box::Orientation::VERTICAL);
    dialog_main_box->SetSpacing(7);
    dialog_window.get()->Add(dialog_main_box);

    top_box = Box::Create(Box::Orientation::HORIZONTAL);    //top
    dialog_main_box.get()->Pack(top_box, false, false);

    path_entry = Entry::Create(".");
    top_box->Pack(path_entry);
    //path_entry->GetSignal(path_entry.get()->HandleKeyEvent(sf::Keyboard::Left, true)).Connect(bind(&FileDialog::listFiles, this, path_entry.get()->GetText()));
    path_entry->GetSignal(Entry::OnRightClick).Connect(bind(&FileDialog::restorePath, this));

    go_path_button = Button::Create("go");
    go_path_button->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::changeDirectory, this));
    top_box->Pack(go_path_button,  false,true);
    go_parent_button = Button::Create("up");
    //top_box->Pack(go_parent_button,  false,true);
    //go_parent_button->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::goParentDirectory, this));

    explorer_window = ScrolledWindow::Create();             //middle
    explorer_window.get()->SetScrollbarPolicy( sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC| sfg::ScrolledWindow::VERTICAL_ALWAYS );
    dialog_main_box.get()->Pack(explorer_window, false, false);
    explorer_box = Box::Create(Box::Orientation::VERTICAL);
    explorer_window.get()->AddWithViewport(explorer_box);
    explorer_window.get()->SetRequisition( sf::Vector2f(400, 260) );

    bottom_box = Box::Create(Box::Orientation::VERTICAL);    //bot
    dialog_main_box.get()->Pack(bottom_box, false, false);

    filename_entry = Entry::Create("");
    bottom_box->Pack(filename_entry);

    bottom_box_buttons = Box::Create(Box::Orientation::HORIZONTAL);
    bottom_box.get()->Pack(bottom_box_buttons, false, false);

    apply = Button::Create("ok");
    close = Button::Create("close");
    bottom_box_buttons->Pack(apply);
    bottom_box_buttons->Pack(close);

    apply->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::applyEvent, this, false));
    close->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::hideWindow, this));

    //info_window
    sf::Vector2f info_window_size(200, 100);
    info_window = Window::Create(sfg::Window::Style::TOPLEVEL );
    info_window->SetTitle("Error");
    info_box = Box::Create(Box::Orientation::VERTICAL);
    info_box->SetSpacing(10);
    info_window->Add(info_box);
    info_window->SetAllocation(sf::FloatRect(0, 0, info_window_size.x, info_window_size.y));

    info_label = Label::Create("error message");
    info_box->Pack(info_label);
    info_button = Button::Create("ok");
    info_box->Pack(info_button);
    info_window->Show(false);
    info_button->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::hideError, this));

    //overwrite_window
    sf::Vector2f overwrite_window_size(300, 100);
    overwrite_window = Window::Create(sfg::Window::Style::TOPLEVEL);
    overwrite_window->SetTitle("Confirm file replace");
    overwrite_box = Box::Create(Box::Orientation::VERTICAL);
    overwrite_box->SetSpacing(10);
    overwrite_box_b = Box::Create(Box::Orientation::HORIZONTAL);
    //overwrite_box_b->SetSpacing(10);
    overwrite_window->Add(overwrite_box);
    overwrite_window->SetAllocation(sf::FloatRect(0, 0, overwrite_window_size.x, overwrite_window_size.y));

    overwrite_label = Label::Create("error message");
    overwrite_box->Pack(overwrite_label);
    overwrite_button_ok = Button::Create("yes");
    overwrite_button_cancel = Button::Create("no");
    overwrite_box_b->Pack(overwrite_button_ok);
    overwrite_box_b->Pack(overwrite_button_cancel);
    overwrite_window->Show(false);
    overwrite_box->Pack(overwrite_box_b);
    overwrite_button_cancel->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::hideOverwriteWindow, this));
    overwrite_button_ok->GetSignal(Button::OnLeftClick).Connect(bind(&FileDialog::applyEvent, this, true));
    hideWindow();
}

 void FileDialog::setRenderWindow(sf::RenderWindow & win)
 {
     sfml_window = &win;
 }

void FileDialog::showError(const string & message)
{
    desktop_->BringToFront(info_window);
    info_label->SetText(message);
    info_window->SetPosition(sf::Vector2f(dialog_window->GetAllocation().left + dialog_window->GetAllocation().width/2 - info_window->GetAllocation().width/2, dialog_window->GetAllocation().top + dialog_window->GetAllocation().height/2 - info_window->GetAllocation().height/2));
    info_window->Show(true);
    dialog_window->SetState(Widget::State::INSENSITIVE);
}

void FileDialog::hideError()
{
    info_window->Show(false);
    dialog_window->SetState(Widget::State::NORMAL);
}

void FileDialog::showOverwriteWindow(const string & message)
{
    desktop_->BringToFront(overwrite_window);
    overwrite_label->SetText(message+"\nfile already exist. Do you want to replace?\nIf you confirm this file will be lost!");
    overwrite_window->SetPosition(sf::Vector2f(dialog_window->GetAllocation().left + dialog_window->GetAllocation().width/2 - overwrite_window->GetAllocation().width/2, dialog_window->GetAllocation().top + dialog_window->GetAllocation().height/2 - overwrite_window->GetAllocation().height/2));
    overwrite_window->Show(true);
    dialog_window->SetState(Widget::State::INSENSITIVE);
}

void FileDialog::hideOverwriteWindow()
{
    overwrite_window->Show(false);
    dialog_window->SetState(Widget::State::NORMAL);
}

void FileDialog::restorePath()
{
    path_entry->SetText(path);
    path_entry->SetCursorPosition(path.size());
}

void FileDialog::hideWindow()
{
    dialog_window->Show(false);
    overwrite_window->Show(false);
    dialog_window->SetState(Widget::State::INSENSITIVE);
}

void FileDialog::action(Action action_type)
{
    if(desktop_ == nullptr)
    {
        cout<<"FileDialog error: desktop not set. Use setDesktop(Desktop & desktop)"<<endl;
        return;
    }

    a_type = action_type;
    dialog_window->SetState(Widget::State::NORMAL);
    listFiles(path);
    dialog_window->Show(true);
    if(sfml_window != nullptr){}
    //sf::RenderWindow()
    //www.create(sf::VideoMode(8, 8), "");
//    sfml_window->getSize();
    //dialog_window->SetPosition(sf::Vector2f(sfml_window->getSize().x/2 - dialog_window->GetAllocation().width/2, sfml_window->getSize().y/2 - dialog_window->GetAllocation().height/2));
    desktop_->BringToFront(dialog_window);

    if(action_type == SELECT_FILE)
    {
        dialog_window->SetTitle("Select file");
    }
    else if(action_type == SELECT_FOLDER)
    {
        dialog_window->SetTitle("Select folder");
    }
    else if(action_type == SELECT_FOLDER_AND_TYPE_FILENAME)
    {
        dialog_window->SetTitle("Select folder and type filename");
    }
}

void FileDialog::enterKeyPressed()
{
    if(dialog_window->IsGloballyVisible())
    {
        if(path_entry->HasFocus())
        changeDirectory();
        else if(filename_entry->HasFocus())
        applyEvent(false);
    }
}

void FileDialog::returnReady()
{
    event = true;
    data = Data(path, filename_entry->GetText(), path+"\\"+filename_entry->GetText());
    a_type = NONE;
}

void FileDialog::applyEvent(bool confirm)
{
    if(a_type == SELECT_FILE)
    {
        if(is_regular_file(string(path+"\\"+filename_entry->GetText()).c_str()) == 1)
        {
            returnReady();
            hideWindow();
        }
        else
        {
            event = false;
            showError(path+"\\"+filename_entry->GetText()+"\ndoes not exist.");
        }

    }
    else if(a_type == SELECT_FOLDER)
    {
        if(is_regular_file(path.c_str()) == 0)//1==file
        {
            if(filename_entry->GetText().isEmpty())
            {
                if(dirExists(path) == 0)
                {
                    //cout<<"SELECT_FOLDER OK "<<data.directory<<endl;
                    returnReady();
                    hideWindow();
                }
            }
            else
            {
                if(dirExists(path+"\\"+filename_entry->GetText()) == 0)
                {
                    showError(path+"\\"+filename_entry->GetText()+"\nis a directory.");
                }
                else if(confirm == false && is_regular_file(string(path+"\\"+filename_entry->GetText()).c_str()) == 1)
                {
                    showOverwriteWindow(path+"\\"+filename_entry->GetText());
                }
                else
                {
                    returnReady();
                    hideWindow();
                }
            }
        }
    }
    else if (a_type == SELECT_FOLDER_AND_TYPE_FILENAME)
    {
        if(!filename_entry->GetText().isEmpty())
        {
            if(dirExists(path) == 0)
            {
                if(confirm == false && is_regular_file(string(path+"\\"+filename_entry->GetText()).c_str()) == 1)
                {
                    showOverwriteWindow(path+"\\"+filename_entry->GetText());

                }
                else if(dirExists(path+"\\"+filename_entry->GetText()) == 0)
                {
                    showError(path+"\\"+filename_entry->GetText()+"\nis a directory.");
                }
                else
                {
                    returnReady();
                    hideWindow();
                }
            }
            else
            {
                showError(path+"path is invalid or cannot access");
            }

        }
        else
        {
            showError("Enter file name");
        }
    }
}

bool FileDialog::eventOccured()
{
    if(event)
    {
        event = false;
        a_type = Action::NONE;
        return true;
    }
    return false;
}

void FileDialog::changeDirectory()
{
    path_entry->SetState(sfg::Entry::State::NORMAL);
    if(checkDirectory(path_entry->GetText()))
        listFiles(path_entry->GetText());
    else
        showError(path_entry->GetText()+"\nis not a directory or access denied.");
}

void FileDialog::setDesktop(Desktop & desktop)
{
    desktop_ = &desktop;
    desktop_->Add(dialog_window);
    desktop_->Add(info_window);
    desktop_->Add(overwrite_window);
}

//**********ExplorerItem**********
FileDialog::ExplorerItem::ExplorerItem(int type, string name)
{
    to_delete = false;
    box = Box::Create(Box::Orientation::HORIZONTAL);
    box->SetSpacing(5);
    label = Label::Create(name);
    folder_img = Image::Create(folder_icon);
    file_img = Image::Create(file_icon);

    if(type == 0)
    box->Pack(folder_img);
    else if(type == 1)
    box->Pack(file_img);
    //else cout<<"UNRECOGNIZED FILE TYPE"<<endl;
    box->Pack(label, false, true);
    box->SetRequisition(sf::Vector2f(2000, 26));
}

Box::Ptr FileDialog::ExplorerItem::get()
{
    return box;
}

void FileDialog::ExplorerItem::loadImages()
{
    bool fail = false;
    if(!folder_icon.loadFromFile("FileDialog\\foldericon.png"))
        fail = true;
    if(!file_icon.loadFromFile("FileDialog\\fileicon.png"))
        fail = true;

    /*if(fail)
        cout<<"load image error"<<endl;
    else
        cout<<"load image OK"<<endl;*/
}
//**********filesystem functions**********
namespace
{

bool checkDirectory(string dir)
{
    struct stat info;
    if( stat( dir.c_str(), &info ) != 0 )
    {
        //cout<<"cannot access"<<dir<<endl;
        return false;
    }
    else if( info.st_mode & S_IFDIR )
    {
        //cout<<"is a directory"<<dir<<endl;
        return true;
    }
    else
    {
        //cout<<"is no directory"<<dir<<endl;
        return false;
    }
}

int dirExists(string path)
{
    struct stat info;
    if(stat( path.c_str(), &info ) != 0)
        return 1;//access problem
    else if(info.st_mode & S_IFDIR)
        return 0;//ok
    else
        return 2;//no dir
}

string go_parent_path(string ep)
{
    int pos = ep.find_last_of('\\');
    //cout<<"pos "<<pos<<endl;
    if(ep.size()>2&&ep.substr(ep.size()-2, 2) == "..")
    return ep+"\\..";
    else if(ep == ".")
    return ep+"\\..";
    else
    return ep.substr(0, pos);
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

}//namespace
