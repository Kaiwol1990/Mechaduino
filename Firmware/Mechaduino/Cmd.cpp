#include <Arduino.h>
#include "Cmd.h"

// command line message buffer and pointer
static uint8_t msg[MAX_MSG_SIZE];
static uint8_t *msg_ptr;

// linked list for command table
static cmd_t *cmd_tbl_list, *cmd_tbl;

// text strings for command prompt (stored in flash)
const char cmd_prompt[] = "CMD >> ";
const char cmd_unrecog[] = "CMD: Command not recognized.";

static Stream* stream;




//Generate the main command prompt
void cmd_display()
{
  char buf[256];

  stream->println();

  strcpy_P(buf, cmd_prompt);
  stream->print(buf);
}

/**************************************************************************/
/*!
    Parse the command line. This function tokenizes the command input, then
    searches for the command table entry associated with the commmand. Once found,
    it will jump to the corresponding function.
*/
/**************************************************************************/
void cmd_parse(char *cmd)
{
  uint8_t argc, i = 0;
  char *argv[30];
  char buf[256];
  cmd_t *cmd_entry;

  fflush(stdout);

  // parse the command line statement and break it up into space-delimited
  // strings. the array of strings will be saved in the argv array.
  argv[i] = strtok(cmd, " ");
  do
  {
    argv[++i] = strtok(NULL, " ");
  } while ((i < 30) && (argv[i] != NULL));

  // save off the number of arguments for the particular command.
  argc = i;

  // parse the command table for valid command. used argv[0] which is the
  // actual command name typed in at the prompt
  for (cmd_entry = cmd_tbl; cmd_entry != NULL; cmd_entry = cmd_entry->next)
  {
    if (!strcmp(argv[0], cmd_entry->cmd))
    {
      cmd_entry->func(argc, argv);
      cmd_display();
      return;
    }
  }

  // command not recognized. print message and re-generate prompt.
  strcpy_P(buf, cmd_unrecog);
  stream->println(buf);
  stream->println("");

  cmd_display();
}

/**************************************************************************/
/*!
    This function processes the individual characters typed into the command
    prompt. It saves them off into the message buffer unless its a "backspace"
    or "enter" key.
*/
/**************************************************************************/
void cmd_handler(char c)
{
  switch (c)
  {
    case '\r':
      // terminate the msg and reset the msg ptr. then send
      // it to the handler for processing.
      *msg_ptr = '\0';
      stream->print("\r\n");
      cmd_parse((char *)msg);
      msg_ptr = msg;
      break;

    case '\n':
      // ignore newline characters. they usually come in pairs
      // with the \r characters we use for newline detection.
      break;

    case '\b':
      // backspace
      stream->print(c);
      if (msg_ptr > msg)
      {
        msg_ptr--;
      }
      break;

    default:
      // normal character entered. add it to the buffer
      stream->print(c);
      *msg_ptr++ = c;
      break;
  }
}


// if Serial available parse it to handler
void cmdPoll()
{
  while (stream->available())
  {
    cmd_handler(stream->read());
  }
}


// Initialize the command line interface
void cmdInit(Stream *str)
{
  stream = str;
  // init the msg ptr
  msg_ptr = msg;

  // init the command table
  cmd_tbl_list = NULL;

}

//Add a command to the command table.
void cmdAdd(const char *name, void (*func)(int argc, char **argv))
{
  // alloc memory for command struct
  cmd_tbl = (cmd_t *)malloc(sizeof(cmd_t));

  // alloc memory for command name
  char *cmd_name = (char *)malloc(strlen(name) + 1);

  // copy command name
  strcpy(cmd_name, name);

  // terminate the command name
  cmd_name[strlen(name)] = '\0';

  // fill out structure
  cmd_tbl->cmd = cmd_name;
  cmd_tbl->func = func;
  cmd_tbl->next = cmd_tbl_list;
  cmd_tbl_list = cmd_tbl;
}




// Checks if a argument was called and returns the integer after it
int32_t return_integer_argument(char **args, uint8_t arg_cnt, const char *identifier, int32_t default_value, int32_t min_value, int32_t max_value) {

  int32_t val = default_value;
  for ( uint8_t i = 1; i < arg_cnt; i++) {
    String argument = args[i];
    if (argument == identifier) {
      val = strtol(args[i + 1], NULL, 10);
    }
  }

  val = constrain(val, min_value, max_value);
  return val;
}


//Checks if a argument was called and returns the float after it
float return_float_argument(char **args, uint8_t arg_cnt, const char *identifier, float default_value, float min_value, float max_value) {

  float val = default_value;
  for ( byte i = 1; i < arg_cnt; i++) {
    String argument = args[i];
    if (argument == identifier) {
      val = strtof(args[i + 1], NULL);
    }
  }
  val = constrain(val, min_value, max_value);
  return val;
}


//Checks if a argument was called and returns the char value after it
String return_char_argument(char **args, uint8_t arg_cnt, const char *identifier, const String default_value) {

  String val = default_value;
  for ( byte i = 1; i < arg_cnt; i++) {
    String argument = args[i];
    if (argument == identifier) {
      val = args[i + 1];
    }
  }
  return val;
}





//Checks if a argument was called and returns the boolean value after it
bool return_bool_argument(char **args, uint8_t arg_cnt, const char *identifier, bool default_value) {

  bool val = default_value;
  for ( byte i = 1; i < arg_cnt; i++) {
    String argument = args[i];
    if (argument == identifier) {
      val = strtof(args[i + 1], NULL);
    }
  }
  val = constrain(val, 0, 1);
  return val;

}



//Checks if a argument was called and returns true if the argument was found
bool check_argument(char **args, uint8_t arg_cnt, const char *identifier) {

  for ( byte i = 1; i < arg_cnt; i++) {
    String argument = args[i];
    if (argument == identifier) {
      return true;
    }
  }
  return false;

}



//Insert a command to the Serial buffer
void insert_command(const char* command_string) {

  char buf[30];

  // copy command to buffer and parse it to cmd_handler 
  strcpy(buf, command_string);
  
  uint8_t msg_length = 0;
  while ((msg_length < 30) && buf[msg_length] != NULL) {
    cmd_handler( buf[msg_length]);
    msg_length++;
  }

  // attache carridge return to the end of the command
  cmd_handler(' ');
  cmd_handler('\r');

}

