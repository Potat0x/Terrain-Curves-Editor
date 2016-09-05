#include "functs.hpp"

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

bool create_new_file(const string & filename)
{
    ofstream file;
    file.open(filename);
    if(is_regular_file(filename.c_str()) == 1)
    {
        file.close();
        return true;
    }
    else
    {
        cout<<"CREATE FILE ERROR: "<<filename<<endl;
        return false;
    }
}

unsigned int r_255()
{
    return (rand() % 255 ) + 0;
}

sf::Color rand_color()
{
    return sf::Color(r_255(), r_255(), r_255());
}
