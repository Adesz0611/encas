# Encas: Single-Header Ensight Gold Reader

Usage:
------
```c
#define ENCAS_IMPLEMENTATION
#include "encas.h"
#include <stdio.h>

int main(int argc, char **argv) {

    Encas_Case *encase = Encas_ReadCase("data.case");
    if (encase == NULL) {
        fprintf(stderr, "Couldn't load case file!\n");
        return EXIT_FAILURE;
    }
                                                       /* time_idx */
    Encas_MeshArray *mesh = Encas_LoadGeometry(encase,       0       );
    if (mesh == NULL) {
        fprintf(stderr, "Couldn't load geometry!\n");
        return EXIT_FAILURE;
    }
                                                    /* time_idx */  /* variable_idx */  /* part_idx */
    float *data = Encas_LoadVariableDataPart(encase,       0    ,         0         ,       0       );
    if (data == NULL) {
        printf("Couldn't load variable data!\n");
        return EXIT_FAILURE;
    }

    ENCAS_FREE(data);
    Encas_DeleteMeshArray(mesh);
    Encas_DeleteCase(encase);

    return 0;
}
```
