//
// Created by egolg on 18.11.2020.
//

#include "Akinator.h"

const char* DATABASE_VERSION = "0.0.0.0";
const int DATABASE_VERSION_LENGTH = strlen (DATABASE_VERSION);
const char* data = "database.txt";

Tree* readData (char* fileName, char** storage, String** text, char* tree_logs)
{
    assert (fileName);

    char* buffer = NULL;
    readToStorage (fileName, &buffer);
    int nStr = countSymb (buffer, '\n');
    String* lines = NULL;

    splitTextIntoLines (buffer, nStr, &lines);
    substituteSymb (buffer, '\n', '\0');

    char* beginOfLogicPart = NULL;
    char* endOfLogicPart = buffer;

    int currentNumberOfString = 0;

    for (; currentNumberOfString < nStr; ++currentNumberOfString)
    {
        if (!(beginOfLogicPart = strchr ((lines + currentNumberOfString)->chLine, '{')) ||
            !(strchr ((lines + currentNumberOfString)->chLine, '}')))
            continue;
        if (DescrConfirm (beginOfLogicPart + 1))
            return NULL;

        ++currentNumberOfString;
        break;
    }

    for (; currentNumberOfString < nStr; ++currentNumberOfString)
    {
        if (!(beginOfLogicPart = strchr((lines + currentNumberOfString)->chLine, '{')) ||
            !(strchr((lines + currentNumberOfString)->chLine, '}')))
            continue;

        if (VersionConfirm(beginOfLogicPart + 1))
            return NULL;
        ++currentNumberOfString;
        break;
    }

    for (; currentNumberOfString < nStr; ++currentNumberOfString)
    {
        if (!(beginOfLogicPart = strchr((lines + currentNumberOfString)->chLine, '{')) ||
            !(strchr((lines + currentNumberOfString)->chLine, '}')))
            continue;
        if (!LanguageConfirm(beginOfLogicPart + 1))
            return NULL;

        currentNumberOfString++;
        break;
    }

    if (currentNumberOfString == nStr)
        return NULL;


    Tree* database = (Tree*) calloc (1, sizeof (*database));
    TreeConstruct (database, tree_logs);

    if (!database)
        return database;

    database->root = TreeDataInterpreter (lines, &currentNumberOfString, nStr, NULL, database);

    *storage = buffer;
    *text = lines;

    return database;
}

TreeError DescrConfirm (char* descr)
{
    assert (descr);
    //I don`t know what description can contain
    return NO_TREE_ERRORS;
}

TreeError VersionConfirm (char* version)
{
    assert (version);

    if (strncmp (DATABASE_VERSION, version, DATABASE_VERSION_LENGTH) != 0)
        return WRONG_VERSION;

    return NO_TREE_ERRORS;
}

unsigned int LanguageConfirm (char* language)
{
    assert (language);

    if (strncmp (language, "RUS", 3))
        return 1;

    if (strncmp (language, "ENG", 3))
        return 2;

    return 0;
}

TreeMember* TreeDataInterpreter (String* lines, int* currentN, int nStr, TreeMember* parent, Tree* tree)
{
    assert (tree);
    assert (lines);
    assert (currentN);

    char* logicBegin = NULL;

    if (!(logicBegin = strchr ((lines + (*currentN)++)->chLine, '[')))
        return NULL;

    TreeMember* currentVertex = (TreeMember*) calloc (1, sizeof(*currentVertex));
    if (!currentVertex)
        return NULL;

    char* firstQuestion = NULL;
    char* firstAnswer = NULL;
    tree->size++;

    currentVertex->parent = parent;
    currentVertex->tree = tree;

    if (firstQuestion = strchr ((lines + *currentN)->chLine,'?'))
    {
        (*currentN)++;

        currentVertex->memberValue = firstQuestion + 1;
        currentVertex->rightChild = TreeDataInterpreter (lines, currentN, nStr, currentVertex, tree);
        currentVertex->leftChild = TreeDataInterpreter (lines, currentN, nStr, currentVertex, tree);

        firstQuestion = strchr (firstQuestion + 1,'?');
        *firstQuestion = '\0';

        if (strchr ((lines + (*currentN)++)->chLine,']'))
            return currentVertex;

        return NULL;
    }

    if (firstAnswer = strchr ((lines + *currentN)->chLine,'`'))
    {
        (*currentN)++;

        currentVertex->memberValue = firstAnswer + 1;
        currentVertex->rightChild = NULL;
        currentVertex->leftChild = NULL;

        firstAnswer = strchr (firstAnswer + 1,'`');
        *firstAnswer = '\0';

        if (strchr ((lines + (*currentN)++)->chLine,']'))
            return currentVertex;

        return NULL;
    }

    return NULL;
}

TreeError GameMode (Tree* tree)
{
    if (QuestionUser (tree->root))
    {
        printf ("database failed");
        return DOES_NOT_EXIST;
    }

    return NO_TREE_ERRORS;

}

TreeError QuestionUser (TreeMember* current) {
    if (!current)
        return DOES_NOT_EXIST;

    txSpeak ("\vIs your character %s ? y/n\n", current->memberValue);

    char answ = 'a';
    while (answ != 'y' && answ != 'n') {
        fflush(stdin);
        scanf("%c", &answ);
    }

    if (answ == 'y') {
        if (current->rightChild)
            QuestionUser (current->rightChild);

        else {
            txSpeak ("\vAs I thought! That's why everyone calls me genious AI!\n");
        }
    } else if (answ == 'n') {
        if (current->leftChild)
            QuestionUser (current->leftChild);

        else
            AddNewCharacter (current);
    }
    return NO_TREE_ERRORS;
}

TreeError AddNewCharacter (TreeMember* current)
{
    txSpeak ("\vI don't know, who it is, name it and it's quality.\n");

    char *quality = (char *) calloc(20, sizeof(*quality));
    do
    {
        printf ("Quality which your character HAVE (your quaity can`t contain ? symbol)\n");
        fflush (stdin);
        gets (quality);
    } while (strchr (quality, '?'));

    char *name = (char *) calloc(20, sizeof(*name));

    do
    {
        printf ("Name your character (your characters name can`t contain ` symbol)\n");
        fflush (stdin);
        gets (name);
    } while (strchr (name, '`'));

    AddRightChild (current, name);
    AddLeftChild (current, current->memberValue);
    current->memberValue = quality;

    return NO_TREE_ERRORS;
}

int RewriteData (Tree* current)
{
    FILE* outputFile = fopen (data, "w");

    if (!outputFile)
        return 1;

    fprintf (outputFile, "{}\n");
    fprintf (outputFile, "{%s}\n", DATABASE_VERSION);
    fprintf (outputFile, "{ENG}\n");

    PreOrderWrite (current->root, outputFile, 0);

    fclose (outputFile);
}

void PreOrderWrite (TreeMember* current, FILE* data, int nTabs)
{
    for (int i = 0; i < nTabs * 2; ++i)
        fprintf (data, "\t");
    fprintf (data, "[\n");
    if (current->leftChild)
    {
        for (int i = 0; i < nTabs * 2 + 1; ++i)
            fprintf (data, "\t");
        fprintf (data, "?%s?\n", current->memberValue);
        PreOrderWrite (current->rightChild, data, nTabs + 1);
        PreOrderWrite (current->leftChild, data, nTabs + 1);
    }

    else
    {
        for (int i = 0; i < nTabs * 2 + 1; ++i)
            fprintf (data, "\t");
        fprintf (data, "`%s`\n", current->memberValue);
    }

    for (int i = 0; i < nTabs * 2; ++i)
        fprintf (data, "\t");

    fprintf (data, "]\n");
}

TreeError MemberData(Tree* tree)
{
    txSpeak ("\vEnter name of your character\n");
    char* name = (char*) calloc (20, sizeof(*name));
    fflush(stdin);
    gets (name);
    Stack* pointers;
    pointers = newStack();
    stackConstructor (pointers, 10);
    //pushIntoStack (pointers, "kostyl'");

    if (FindElem (tree->root, pointers, name) == NO_TREE_ERRORS)
    {
        txSpeak ("\v%s have these qualities: ", name);
        PrintQualities (pointers, tree->root);
        stackDestructor (pointers);
        return NO_TREE_ERRORS;
    }

    txSpeak ("\vThere is no such element");
    stackDestructor (pointers);
    return INCORRECT_VALUE;
}

TreeError FindElem (TreeMember* current, Stack* pointers, char* name)
{
    if (!strcmp (current->memberValue, name))
    {
        pushIntoStack (pointers, current->memberValue);
        return NO_TREE_ERRORS;
    }
    if (current->rightChild && (FindElem (current->rightChild, pointers, name) == NO_TREE_ERRORS))
    {
        pushIntoStack (pointers, current->memberValue);
        return NO_TREE_ERRORS;
    }
    if (current->leftChild && (FindElem (current->leftChild, pointers, name) == NO_TREE_ERRORS))
    {
        pushIntoStack (pointers, current->memberValue);
        return NO_TREE_ERRORS;
    }

    return INCORRECT_VALUE;
}

TreeError PrintQualities (Stack* pointers, TreeMember* currentVertex)
{
    char* previousName = popFromStack (pointers);
    char* currentName = popFromStack (pointers);

    while (GetStackSize (pointers) > 0)
    {
        if (currentVertex->leftChild->memberValue == currentName)
        {
            currentVertex = currentVertex->leftChild;
            txSpeak ("\vnot ");
        } else
            currentVertex = currentVertex->rightChild;

        txSpeak ("\v%s, ", previousName);
        previousName = currentName;
        currentName = popFromStack (pointers);
    }
    if (currentVertex->leftChild->memberValue == currentName)
        txSpeak ("\vnot ");
    txSpeak ("\v%s, ", previousName);

    return NO_TREE_ERRORS;
}

TreeError MemberComparator (Tree* tree)
{
    txSpeak ("\vEnter name of first character\n");
    char* firstName = (char*) calloc (20, sizeof(*firstName));
    fflush(stdin);
    gets (firstName);
    Stack* firstPointers;
    firstPointers = newStack();
    stackConstructor (firstPointers, 10);
    //pushIntoStack (firstPointers, "kostyl1");

    txSpeak ("\vEnter name of second character\n");
    char* secondName = (char*) calloc (20, sizeof(*secondName));
    fflush(stdin);
    gets (secondName);
    Stack* secondPointers;
    secondPointers = newStack();
    stackConstructor (secondPointers, 10);
    //pushIntoStack (secondPointers, "kostyl2");

    if (FindElem (tree->root, firstPointers, firstName) || FindElem (tree->root, secondPointers, secondName))
    {
        stackDestructor (firstPointers);
        stackDestructor (secondPointers);
        free (firstName);
        free (secondName);
        txSpeak ("\vone of your characters is not in this database\n");
        return INCORRECT_VALUE;
    }

    txSpeak ("\v%s and %s are both ", firstName, secondName);

    char* firstNameCurr  = popFromStack (firstPointers);
    char* secondNameCurr = popFromStack (secondPointers);
    char* firstNameNext  = popFromStack (firstPointers);
    char* secondNameNext = popFromStack (secondPointers);
    TreeMember* current  = tree->root;

    while (firstNameNext == secondNameNext)
    {
        if (current->leftChild->memberValue == firstNameNext)
        {
            txSpeak ("\vnot ");
            current = current->leftChild;
        } else
            current = current->rightChild;

        txSpeak ("\v%s, ", firstNameCurr);
        firstNameCurr = firstNameNext;
        secondNameCurr = secondNameNext;
        firstNameNext = popFromStack (firstPointers);
        secondNameNext = popFromStack (secondPointers);
    }

    TreeMember* temp = current;

    txSpeak ("\v\nBut %s also is ", firstName);
    pushIntoStack (firstPointers, firstNameNext);
    pushIntoStack (firstPointers, firstNameCurr);
    PrintQualities (firstPointers, current);
    stackDestructor (firstPointers);

    txSpeak ("\vit is in database.\nAnd %s is ", secondName);
    pushIntoStack (secondPointers, secondNameNext);
    pushIntoStack (secondPointers, secondNameCurr);
    PrintQualities (secondPointers, current);
    stackDestructor (secondPointers);
    txSpeak ("\vit is in database.\n");

    free (firstName);
    free (secondName);

    return NO_TREE_ERRORS;
}

TreeError GraphicDumpUSER (Tree* thou)
{
    if (!thou)
        printf ("there is no TREE. Executed on line");

    FILE* graph_logs = nullptr;
    if(!(graph_logs = fopen (thou->graph_logs_name, "w")))
        return LOGS_ENABLE;

    fprintf (graph_logs, "digraph LIST{\n");
    fprintf (graph_logs, "\t" "rankdir = TB;\n");

    DrawVertexUSER (graph_logs, thou->root);

    fprintf (graph_logs, "}\n");

    fclose (graph_logs);
    char command[100];
    sprintf (command, "dot -Tpng %s -o myGraph.png", thou->graph_logs_name);
    system (command);
    system ("start myGraph.png");

    return NO_TREE_ERRORS;
}

void DrawVertexUSER (FILE* graph_logs, TreeMember* current)
{
    if (!current)
        return;

    if (current->leftChild)
    {
        fprintf (graph_logs, "\"%s\" -> \"%s\"[label=\"no\"];\n", current->memberValue, current->leftChild->memberValue);
        DrawVertexUSER (graph_logs, current->leftChild);
    }
    if (current->rightChild)
    {
        fprintf (graph_logs, "\"%s\" -> \"%s\"[label=\"yes\"];\n", current->memberValue, current->rightChild->memberValue);
        DrawVertexUSER (graph_logs, current->rightChild);
    }
}

int GameMenu (Tree* tree)
{
    while (1)
    {
        printf ("Enter game mode\n");
        printf ("[1] play akinator\n");
        printf ("[2] print data about character\n");
        printf ("[3] compare two characters\n");
        printf ("[4] show data\n");
        printf ("[5] exit with saving\n");
        printf ("[6] exit without saving\n");
        fflush (stdin);
        int choice = 0;
        scanf ("%d", &choice);
        switch (choice)
        {
            case 1: GameMode (tree); break;
            case 2: MemberData (tree); break;
            case 3: MemberComparator (tree); break;
            case 4: GraphicDumpUSER (tree); break;
            case 5: RewriteData (tree); return 0;
            case 6:  return 0;
            default:
                txSpeak ("\vEnter is not correct, you should write a number from 1 to 6");
        }
    }
}
