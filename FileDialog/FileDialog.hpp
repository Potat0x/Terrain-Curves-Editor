/*
    FileDialog window feature for SFGUI
    https://github.com/Potat0x/SFGUI-FileDialog
    http://potat0x.cba.pl
*/

#ifndef file_dialog
#define file_dialog
#include <SFGUI/Widgets.hpp>
#include <sys/stat.h>
#include <dirent.h>

using namespace sfg;
using namespace std;

class FileDialog
{
public:
    enum Action
    {
        NONE,
        SELECT_FILE,
        SELECT_FOLDER,
        SELECT_FOLDER_AND_TYPE_FILENAME
    };

    class Data  //data structure for user
    {
        public:          //    example
        string directory;//    C:\Windows\System32
        string file_name;//    cmd.exe
        string file_path;//    C:\Windows\System32\cmd.exe

        Data(){}
        Data(string dir, string fn, string fp): directory(dir), file_name(fn), file_path(fp)  {}
    };
    Data data;

    FileDialog();
    void setDesktop(Desktop & desktop);//pass desktop to filedialog. Required before using dialog
    void setRenderWindow(sf::RenderWindow & win);//if you use it FileDialog will be centered in RenderWindow
    void action(Action action_type);//run FileDialog window
    bool eventOccured();//true if user click ok button and entered data are correct
    void enterKeyPressed();//call it when enter key is pressed, allow to activate text entries via Enter key
    void update();

private:
    Action a_type;
    bool event;
    string path;
    Desktop * desktop_;
    sf::RenderWindow * sfml_window;

    vector <string> folders, files;
    void listFiles(string path_name);

    class ExplorerItem;
    vector <ExplorerItem*> items;

    Window::Ptr dialog_window;
    Box::Ptr dialog_main_box;

    Box::Ptr top_box;                   //top
    Entry::Ptr path_entry;
    Button::Ptr go_parent_button;
    Button::Ptr go_path_button;
    void changeDirectory();
    void goParentDirectory();
    void restorePath();

    Box::Ptr explorer_box;                //middle
    ScrolledWindow::Ptr explorer_window;
    bool refresh_list;
    void fileClicked(const string & filename);

    Box::Ptr bottom_box;                //bottom
    Box::Ptr bottom_box_buttons;
    Entry::Ptr filename_entry;
    Button::Ptr apply, close;
    void applyEvent(bool confirm);
    void returnReady();
    void hideWindow();

    Window::Ptr info_window;            //info
    Box::Ptr info_box;
    Label::Ptr info_label;
    Button::Ptr info_button;
    void showError(const string & message);
    void hideError();

    Window::Ptr overwrite_window;            //overwrite
    Box::Ptr overwrite_box;
    Box::Ptr overwrite_box_b;
    Label::Ptr overwrite_label;
    Button::Ptr overwrite_button_ok;
    Button::Ptr overwrite_button_cancel;
    void showOverwriteWindow(const string & message);
    void hideOverwriteWindow();

private:
    class ExplorerItem
    {
        static sf::Image folder_icon, file_icon;

        Box::Ptr box;
        Image::Ptr folder_img, file_img;
        Label::Ptr label;

    public:
        bool to_delete;
        unsigned int connection_serial;

        ExplorerItem(int type, string name);
        static void loadImages();
        Box::Ptr get();
    };
};

#endif // file_dialog
