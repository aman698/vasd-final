#include "commands_config.h"
#include "commands_helper.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <EEPROM.h>

/*
   ================================== CALLBACK FUNCTIONS ===========================================
*/

int _cb_command_0_(int* CommandParams, int CommandParamLength) {
  int flag = -1;

  EEPROM.write(0, *(CommandParams));
  EEPROM.write(1, *(CommandParams + 1));
  EEPROM.write(2, *(CommandParams + 2));
  EEPROM.write(3, *(CommandParams + 3));
  if (EEPROM.read(3) >= 255) {
    EEPROM.write(3, 101);
  }

  return flag;
}
int _cb_command_1_(int* CommandParams, int CommandParamLength) {
  int flag = -1;

  //SERIAL_INTERFACE.println(*CommandParams);
  EEPROM.write(5, *CommandParams );
  if (EEPROM.read(5) == 255) {
    EEPROM.write(5, 1000);
  }

  return flag;
}
int _cb_command_2_(int* CommandParams, int CommandParamLength) {
  int flag = -1;

  EEPROM.write(4, *CommandParams );
  if (EEPROM.read(4) == 255) {
    EEPROM.write(4, 1000);
  }

  return flag;
}
int _cb_command_3_(int* CommandParams, int CommandParamLength) {
  int flag = -1;

  //  setTime(*(CommandParams + 4), *(CommandParams + 5), *(CommandParams + 6), *(CommandParams + 3), *(CommandParams + 2), *(CommandParams + 1));
  // rtc.adjust(DateTime(*(CommandParams + 1), *(CommandParams + 2), *(CommandParams + 3), *(CommandParams + 4), *(CommandParams + 5), *(CommandParams + 6)));

  return flag;
}
int _cb_command_4_(int* CommandParams, int CommandParamLength) {
  int flag = -1;


  return flag;
}
int _cb_command_5_(int* CommandParams, int CommandParamLength) {
  int flag = -1;

  return flag;
}
int _cb_command_6_(int* CommandParams, int CommandParamLength) {
  int flag = -1;



  return flag;
}
int _cb_command_7_(int* CommandParams, int CommandParamLength) {
  int flag = -1;



  return flag;
}
int _cb_command_8_(int* CommandParams, int CommandParamLength) {
  int flag = -1;



  return flag;
}
int _cb_command_9_(int* CommandParams, int CommandParamLength) {
  int flag = -1;


  return flag;
}

/*
   Array Of Pointers To Callback Functions
*/
int (*_command_cb_[TOTALCOMMANDS]) (int* CommandParams, int CommandParamLength) = {
  _cb_command_0_,
  _cb_command_1_,
  _cb_command_2_,
  _cb_command_3_,
  _cb_command_4_,
  _cb_command_5_,
  _cb_command_6_,
  _cb_command_7_,
  _cb_command_8_,
  _cb_command_9_
};

/*
   ============================== COMMAND FUNCTIONS ==================================
*/

/*
    This Fucntion Returns The Type Of Command As Integer
*/

int ClassifyCommand(char *Command) {
  int flag = -1;
  for (int i = 0; i <  TOTALCOMMANDS; i++) {
    
    if (strcmp(Command, CommandList[i])) {
      flag = i;
      break;
    }
  }
  return flag;
}
/*
    This Function Returns The Number of Parameters For A Specified Command
*/
int GetCommandParamLength(int CommandType) {
  int length = -1;
  switch (CommandType) {
    case 0:
      commandParamLength = SET;
      break;
    case 1:
      commandParamLength = SETS;
      break;
    case 2:
      commandParamLength = SETP;
      break;
    case 3:
      commandParamLength = DATE;
      break;
    case 4:
      commandParamLength = R1;
      break;
    case 5:
      commandParamLength = R2;
      break;
    case 6:
      commandParamLength = R3;
      break;
    case 7:
      commandParamLength = R4;
      break;
    case 8:
      commandParamLength = R5;
      break;
    case 9:
      commandParamLength = R6;
      break;
    default:
      return -1;
  }
  return commandParamLength;
}
/*
 * This Functions Returns The Parameters In A Specified Command As Array Of Integers
 */
int* GetCommandParams(char *Command, int ParamLength) {
  int *ParamContainer = (int*)malloc(ParamLength);
  for (int i = 0; Command != NULL;)
  {
    //printf("%s\n", Command);
    if (isdigit(*Command))
    {
      *(ParamContainer + i) = atoi(Command);
      i++;
    }
    Command = strtok (NULL, ",");
  }
  return ParamContainer;
}

/*
    This Function Executes The Callback For A Specified Command
*/
int PerformCommand(int CommandType, int* CommandParams, int CommandParamLength) {
  int flag = -1;
  switch (CommandType) {
    case 0:
      flag = (_command_cb_[0]) (CommandParams, CommandParamLength);
      break;
    case 1:
      flag = (_command_cb_[1]) (CommandParams, CommandParamLength);
      break;
    case 2:
      flag = (_command_cb_[2]) (CommandParams, CommandParamLength);
      break;
    case 3:
      flag = (_command_cb_[3]) (CommandParams, CommandParamLength);
      break;
    case 4:
      flag = (_command_cb_[4]) (CommandParams, CommandParamLength);
      break;
    case 5:
      flag = (_command_cb_[5]) (CommandParams, CommandParamLength);
      break;
    case 6:
      flag = (_command_cb_[6]) (CommandParams, CommandParamLength);
      break;
    case 7:
      flag = (_command_cb_[7]) (CommandParams, CommandParamLength);
      break;
    case 8:
      flag = (_command_cb_[8]) (CommandParams, CommandParamLength);
      break;
    case 9:
      flag = (_command_cb_[9]) (CommandParams, CommandParamLength);
      break;
    default:
      return flag;
  }
  return flag;
}
