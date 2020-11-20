//
// Created by egolg on 18.11.2020.
//

#ifndef TREE_DED_AKINATOR_H
#define TREE_DED_AKINATOR_H

#include "IsE_Tree.h"
#include "IsE_Stack.h"
#include "IsE_text.h"
#define TX_USE_SPEAK
#include "TXLib.h"

Tree* readData (char* fileNameInput, char** storage, String** text, char* treeLogs);

TreeError DescrConfirm (char* descr);

TreeError VersionConfirm (char* version);

unsigned int LanguageConfirm (char* language);

TreeMember* TreeDataInterpreter (String* lines, int* currentN, int nStr, TreeMember* parent, Tree* tree);

TreeError GameMode (Tree* tree);

TreeError QuestionUser (TreeMember* current);

int RewriteData (Tree* current);

void PreOrderWrite (TreeMember* current, FILE* data, int nTabs);

TreeError MemberData (Tree* tree);

TreeError FindElem (TreeMember* current, Stack* pointers, char* name);

TreeError MemberComparator (Tree* tree);

TreeError GraphicDumpUSER (Tree* thou);

void DrawVertexUSER (FILE* graph_logs, TreeMember* current);

TreeError AddNewCharacter (TreeMember* current);

TreeError PrintQualities (Stack* pointers, TreeMember* current);

int GameMenu (Tree* tree);

#endif //TREE_DED_AKINATOR_H
