#pragma once

/* Pure GUI helpers.  Keep this header independent from raylib so the state
 * transitions can be tested without opening a graphical window. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef uint64_t GUI_Id;

typedef struct {
    GUI_Id id;
    float x;
    float y;
    float width;
    float height;
} GUI_NavItem;

static inline GUI_Id GUI_IdHashBytes(GUI_Id seed, const void *data, size_t size)
{
    const unsigned char *bytes = (const unsigned char *)data;
    GUI_Id hash = seed != 0 ? seed : UINT64_C(14695981039346656037);
    for (size_t i = 0; i < size; ++i) {
        hash ^= (GUI_Id)bytes[i];
        hash *= UINT64_C(1099511628211);
    }
    return hash != 0 ? hash : 1;
}

static inline GUI_Id GUI_IdHashString(GUI_Id seed, const char *text)
{
    return GUI_IdHashBytes(seed, text != NULL ? text : "", text != NULL ? strlen(text) : 0);
}

static inline float GUI_ClampScroll(float value, float content_size, float viewport_size)
{
    float maximum = content_size > viewport_size ? content_size - viewport_size : 0.0f;
    if (value < 0.0f) return 0.0f;
    if (value > maximum) return maximum;
    return value;
}

static inline bool GUI_Utf8IsContinuation(unsigned char byte)
{
    return (byte & 0xc0u) == 0x80u;
}

static inline int GUI_Utf8Next(const char *text, int length, int byte_index)
{
    int next = byte_index < length ? byte_index + 1 : length;
    while (next < length && GUI_Utf8IsContinuation((unsigned char)text[next])) next++;
    return next;
}

static inline int GUI_Utf8Prev(const char *text, int byte_index)
{
    int previous = byte_index > 0 ? byte_index - 1 : 0;
    while (previous > 0 && GUI_Utf8IsContinuation((unsigned char)text[previous])) previous--;
    return previous;
}

static inline int GUI_Utf8Encode(int codepoint, char output[4])
{
    if (codepoint < 0 || codepoint > 0x10ffff || (codepoint >= 0xd800 && codepoint <= 0xdfff)) return 0;
    if (codepoint <= 0x7f) {
        output[0] = (char)codepoint;
        return 1;
    }
    if (codepoint <= 0x7ff) {
        output[0] = (char)(0xc0 | (codepoint >> 6));
        output[1] = (char)(0x80 | (codepoint & 0x3f));
        return 2;
    }
    if (codepoint <= 0xffff) {
        output[0] = (char)(0xe0 | (codepoint >> 12));
        output[1] = (char)(0x80 | ((codepoint >> 6) & 0x3f));
        output[2] = (char)(0x80 | (codepoint & 0x3f));
        return 3;
    }
    output[0] = (char)(0xf0 | (codepoint >> 18));
    output[1] = (char)(0x80 | ((codepoint >> 12) & 0x3f));
    output[2] = (char)(0x80 | ((codepoint >> 6) & 0x3f));
    output[3] = (char)(0x80 | (codepoint & 0x3f));
    return 4;
}

static inline void GUI_TextDeleteRange(char *text, int *cursor, int *anchor)
{
    int first = *cursor < *anchor ? *cursor : *anchor;
    int last = *cursor > *anchor ? *cursor : *anchor;
    if (first != last) memmove(text + first, text + last, strlen(text + last) + 1);
    *cursor = first;
    *anchor = first;
}

static inline bool GUI_TextInsertUtf8(char *text, int capacity, int *cursor, int *anchor,
                                      const char *insert, int insert_size)
{
    int length = (int)strlen(text);
    int selected = *cursor > *anchor ? *cursor - *anchor : *anchor - *cursor;
    if (insert_size < 0 || length - selected + insert_size >= capacity) return false;
    GUI_TextDeleteRange(text, cursor, anchor);
    length = (int)strlen(text);
    memmove(text + *cursor + insert_size, text + *cursor, (size_t)(length - *cursor + 1));
    memcpy(text + *cursor, insert, (size_t)insert_size);
    *cursor += insert_size;
    *anchor = *cursor;
    return true;
}

static inline bool GUI_TextBackspace(char *text, int *cursor, int *anchor)
{
    if (*cursor != *anchor) {
        GUI_TextDeleteRange(text, cursor, anchor);
        return true;
    }
    if (*cursor <= 0) return false;
    int previous = GUI_Utf8Prev(text, *cursor);
    memmove(text + previous, text + *cursor, strlen(text + *cursor) + 1);
    *cursor = previous;
    *anchor = previous;
    return true;
}

static inline bool GUI_TextDelete(char *text, int *cursor, int *anchor)
{
    if (*cursor != *anchor) {
        GUI_TextDeleteRange(text, cursor, anchor);
        return true;
    }
    int length = (int)strlen(text);
    if (*cursor >= length) return false;
    int next = GUI_Utf8Next(text, length, *cursor);
    memmove(text + *cursor, text + next, strlen(text + next) + 1);
    return true;
}

static inline GUI_Id GUI_NavFind(const GUI_NavItem *items, int count, GUI_Id current,
                                 float direction_x, float direction_y)
{
    const GUI_NavItem *origin = NULL;
    for (int i = 0; i < count; ++i) if (items[i].id == current) origin = &items[i];
    if (origin == NULL) return count > 0 ? items[0].id : 0;

    float origin_x = origin->x + origin->width * 0.5f;
    float origin_y = origin->y + origin->height * 0.5f;
    float best_score = 3.4e38f;
    GUI_Id best = current;
    for (int i = 0; i < count; ++i) {
        if (items[i].id == current) continue;
        float dx = items[i].x + items[i].width * 0.5f - origin_x;
        float dy = items[i].y + items[i].height * 0.5f - origin_y;
        float primary = dx * direction_x + dy * direction_y;
        if (primary <= 0.0f) continue;
        float cross = dx * direction_y - dy * direction_x;
        if (cross < 0.0f) cross = -cross;
        float score = primary + cross * 4.0f;
        if (score < best_score) {
            best_score = score;
            best = items[i].id;
        }
    }
    return best;
}
