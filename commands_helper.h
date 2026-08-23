#pragma once
#ifndef COMMANDS_HELPER_H
#define COMMANDS_HELPER_H

int ClassifyCommand(char*);
int GetCommandParamLength(int);
int* GetCommandParams(char*, int);
int PerformCommand(int, int*, int);

#endif
