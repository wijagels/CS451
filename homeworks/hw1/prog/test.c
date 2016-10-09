#include "hw1.h"

int main() {
    int ret;
    ret = str_manip("aBcAbc@defCba", "ABC");
    printf("Return code: %d\n", ret);

    ret = str_manip("", "");
    printf("Return code: %d\n", ret);

    ret = str_manip(0, "");
    printf("Return code: %d\n", ret);

    ret =
        myprintf("This is CS %d%c%d%s", 451, '/', 551, " Systems Programming.");
    printf("Return code: %d\n", ret);

    ret = MYMSG("CS %d/%d\n", 451, 551);
    printf("Return code: %d\n", ret);
}
