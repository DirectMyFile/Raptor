#include "console.h"

#include <liblox/string.h>

#include "commands.h"

#define CONSOLE_BUFFER_SIZE 1024
#define CONSOLE_CMD_MAX_SIZE 64

typedef struct debug_console {
    size_t cursor;
    hashmap_t* commands;
    char buff[CONSOLE_BUFFER_SIZE];
} debug_console_t;

static debug_console_t* console = NULL;

static void debug_console_trigger(tty_t* tty) {
    char cmd[CONSOLE_CMD_MAX_SIZE];
    memset(cmd, 0, CONSOLE_CMD_MAX_SIZE);
    char args[CONSOLE_BUFFER_SIZE];
    memset(args, 0, CONSOLE_BUFFER_SIZE);

    char* str = console->buff;

    while (*str == ' ') {
        str++;
    }

    size_t index = 0;
    while (*str != ' ' && index <= (CONSOLE_CMD_MAX_SIZE - 1)) {
        cmd[index++] = *(str++);
    }

    if (index == 0) {
        return;
    }

    while (*str == ' ') {
        str++;
    }

    index = 0;
    while (*str != '\0') {
        args[index++] = *(str++);
    }

    if (*cmd == '\0') {
        return;
    }

    if (!hashmap_has(console->commands, cmd)) {
        tty_printf(tty, "Unknown Command: %s\n", cmd);
        return;
    }

    debug_console_command_t handle = hashmap_get(console->commands, cmd);
    handle(tty, args);
}

static void debug_console_handle_data(tty_t* tty, const uint8_t* buffer, size_t size) {
    unused(tty);

    if (size == 0) {
        return;
    }

    if (size == 1 && buffer[0] == '\b') {
        if (console->cursor > 0) {
            console->cursor--;
            console->buff[console->cursor] = '\0';
        } else {
            console->buff[0] = '\0';
        }
        return;
    }

    if ((console->cursor + size + 1) > CONSOLE_BUFFER_SIZE) {
        console->cursor = 0;
        console->buff[0] = '\0';
        return;
    }

    for (size_t index = 0; index < size; index++) {
        console->buff[console->cursor] = buffer[index];
        console->cursor++;
    }

    if (console->buff[console->cursor - 1] == '\n') {
        console->buff[console->cursor - 1] = '\0';
        tty->execute_post_write = false;
        debug_console_trigger(tty);
        tty->execute_post_write = true;
        tty_write_string(tty, "> ");

        console->cursor = 0;
        *console->buff = '\0';
    }
}

static void debug_console_post_write(tty_t* tty, const uint8_t* buffer, size_t size) {
    unused(tty);

    if (size == 0) {
        return;
    }

    if (buffer[size - 1] == '\n') {
        tty_write_string(tty, "> ");
    }
}

void debug_console_init(void) {
    console = zalloc(sizeof(debug_console_t));
    console->commands = hashmap_create(10);
}

static void debug_help(tty_t* tty, const char* input) {
    unused(input);

    list_t* keys = hashmap_keys(console->commands);
    list_for_each(node, keys) {
        tty_printf(tty, "- %s\n", node->value);
    }
    list_free(keys);
}

static void debug_crash(tty_t* tty, const char* input) {
    unused(input);
    unused(tty);

    memcpy(NULL, NULL, 1);
}

void debug_console_start(void) {
    {
        debug_console_register_command("help", debug_help);
        debug_console_register_command("crash", debug_crash);

        debug_init_commands();
    }

    list_t* ttys = tty_get_all();
    list_for_each(node, ttys) {
        tty_t* tty = node->value;
        if (tty->flags.allow_debug_console) {
            tty->handle_read = debug_console_handle_data;
            tty->post_write = debug_console_post_write;
            tty_write_string(tty, "[[Raptor Debug Console Started]]\n> ");
        }
    }
    list_free(ttys);
}

void debug_console_register_command(char* name, debug_console_command_t cmd) {
    if (console == NULL) {
        return;
    }

    hashmap_set(console->commands, name, cmd);
}
