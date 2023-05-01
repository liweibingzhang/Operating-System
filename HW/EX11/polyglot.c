#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
typedef char* (*getMessage_t)();
int main(int argc, char *argv[])
{
    getMessage_t getmessage = NULL;
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <shared-object-file> ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++)
    {
        // open the shared object
        char *a = "./";
        char *str = argv[i];
        strcat(a, str);
        void *handle = dlopen(a, RTLD_LAZY);
        if (handle == NULL)
        {
            fprintf(stderr, "Failed to open %s: %s\n", argv[i], dlerror());
        }

        // get the function pointer for getMessage()
        getmessage = (getMessage_t)dlsym(handle, "getMessage");
        if (getmessage == NULL)
        {
            fprintf(stderr, "Failed to find getMessage() symbol in %s: %s\n", argv[i], dlerror());
            dlclose(handle);
            continue;
        }
        char *message = getmessage();
        printf("%s\n", message);

        // clean up
        free(getmessage);
        dlclose(handle);
    }

    return 0;
}
