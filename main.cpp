#include "cmake-build-debug/Akinator.h"

int main()
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    char* buffer;
    String* lines;
    Tree* tree = readData ("database.txt", &buffer, &lines, "graph.dot");
    GameMenu (tree);
    DestructTree (tree);
    free (lines);
    free (buffer);
    return 0;
}
