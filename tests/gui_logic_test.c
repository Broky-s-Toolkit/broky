#include <assert.h>
#include <stdio.h>
#include "gui/_logic.h"

static void test_ids(void)
{
    GUI_Id a = GUI_IdHashString(0, "alpha");
    GUI_Id b = GUI_IdHashString(0, "beta");
    assert(a != 0 && b != 0 && a != b);
    assert(a == GUI_IdHashString(0, "alpha"));
}

static void test_utf8_editing(void)
{
    char text[32] = { 'a', (char)0xc3, (char)0xb1, (char)0xf0, (char)0x9f,
        (char)0x99, (char)0x82, 'z', '\0' };
    int length = (int)strlen(text);
    assert(GUI_Utf8Next(text, length, 1) == 3);
    assert(GUI_Utf8Next(text, length, 3) == 7);
    assert(GUI_Utf8Prev(text, 7) == 3);

    int cursor = 7;
    int anchor = 7;
    assert(GUI_TextBackspace(text, &cursor, &anchor));
    assert((unsigned char)text[1] == 0xc3 && (unsigned char)text[2] == 0xb1);
    assert(text[3] == 'z' && text[4] == '\0');
    assert(cursor == 3);

    char encoded[4];
    int encoded_size = GUI_Utf8Encode(0x20ac, encoded);
    assert(encoded_size == 3);
    assert(GUI_TextInsertUtf8(text, (int)sizeof(text), &cursor, &anchor, encoded, encoded_size));
    assert((unsigned char)text[3] == 0xe2 && (unsigned char)text[4] == 0x82
        && (unsigned char)text[5] == 0xac && text[6] == 'z');

    anchor = 1;
    assert(GUI_TextDelete(text, &cursor, &anchor));
    assert(strcmp(text, "az") == 0);
}

static void test_scroll_and_navigation(void)
{
    assert(GUI_ClampScroll(-4.0f, 100.0f, 20.0f) == 0.0f);
    assert(GUI_ClampScroll(90.0f, 100.0f, 20.0f) == 80.0f);
    assert(GUI_ClampScroll(10.0f, 10.0f, 20.0f) == 0.0f);

    GUI_NavItem items[] = {
        { 1, 0, 0, 10, 10 }, { 2, 20, 0, 10, 10 }, { 3, 0, 20, 10, 10 }
    };
    assert(GUI_NavFind(items, 3, 1, 1, 0) == 2);
    assert(GUI_NavFind(items, 3, 1, 0, 1) == 3);
    assert(GUI_NavFind(items, 3, 2, -1, 0) == 1);
}

int main(void)
{
    test_ids();
    test_utf8_editing();
    test_scroll_and_navigation();
    puts("gui_logic_test: ok");
    return 0;
}
