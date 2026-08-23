#pragma once
#ifndef COMMANDS_CONFIG_H
#define COMMANDS_CONFIG_H

#define TOTALCOMMANDS 10
#define MAXCOMMANDPARAMS 10

enum CommandParamLength{
    SET = 4,
    SETS = 1,
    SETP = 1,
    DATE = 6,
    R1 = 1,
    R2 = 1,
    R3 = 1,
    R4 = 1,
    R5 = 1,
    R6 = 1
};
enum CommandParamLength commandParamLength;

char CommandList[TOTALCOMMANDS][10] = {
    "SET",
    "SETS",
    "SETP",
    "DATE",
    "R1",
    "R2",
    "R3",
    "R4",
    "R5",
    "R6"
};


#endif
