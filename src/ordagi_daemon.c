#include <stdio.h>
void ordagi_daemon_stub(void) {}

#ifdef DAEMON_MAIN
int main(void) {
    printf("OrdAGI Daemon (stub)\n");
    return 0;
}
#else
int daemon_main(void) {
    printf("OrdAGI Daemon (stub)\n");
    return 0;
}
#endif
