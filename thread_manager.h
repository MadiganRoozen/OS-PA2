
extern void execute_commands(char *command);

extern void execute_commands(char *command);

void init_thread_manager(int total);

void enqueue_command(char *command);

void *process_command(void *arg);

void start_threads();

