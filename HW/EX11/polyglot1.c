#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

char *getMessage() {
    char *message = malloc(100 * sizeof(char));
    strcpy(message, "Hello world.");
    return message;
}

int main(int argc, char* argv[]) {
    int i;
    void* handle;
    char* message;

    for (i = 1; i < argc; i++) {
        handle = dlopen(argv[i], RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
        }        
        message = dlsym(handle, "getMessage");
        if (!message) {
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
        }       
        printf("%s\n", message);
        free(message);
        dlclose(handle);
    }
    
    return 0;
}
