/**
 * @file subleqc.exe
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-06-10
 *
 * This is the compiler(?) for turning SUBLEQ assembly into a plain binary that
 * is to be interpreted by subleq.exe.
 */

#include <stdio.h>

int main(void)
{
    int Program[] = {
        // BEGIN Instructions.
        6, 7, -1,
        0, 0, -1, // End of program tryptich.
        // BEGIN Data.
        1, -1, 0
    };

    FILE *Binary = fopen("data/binary.x", "w");

    fwrite((void *) Program,
           sizeof(int),
           sizeof(Program) / sizeof(int),
           Binary);

    fclose(Binary);

    return 0;
}
