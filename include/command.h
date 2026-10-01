#ifndef __COMMAND_H__
#define __COMMAND_H__

void cli_printf(const char *format, ...);
void process_cmd(const char *cmd);
void handle_cmd_interface();

#endif // !__COMMAND_H__
